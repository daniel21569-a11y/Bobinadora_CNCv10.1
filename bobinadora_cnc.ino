// =========================================================================
// BOBINADORA CNC CON LVGL - VERSIÃ"N OPTIMIZADA DUAL-CORE
// VERSIÃ"N: 10.1 - SIN INTERFERENCIAS EN MOTORES
// =========================================================================
// CAMBIOS CLAVE:
// - Core 1: SOLO timing crítico de pulsos (motor_task_optimized.h)
// - Core 0: UI, Serial, cálculos, todo lo demás
// - Eliminados Serial.print del Core 1
// - Optimización de loops críticos
// =========================================================================

#include "config.h"
#include "persistence.h"
#include "ui_screens.h"
#include "ui_components.h"
#include "ui_handlers.h"
#include "motor_task_optimized.h"  // <<<< NUEVA TAREA OPTIMIZADA
#include <lvgl.h>
#include <PINS_JC4827W543.h>
#include "TAMC_GT911.h"
#include <Wire.h>
#include <freertos/task.h>

// Sistema
ConfigTransformador Sistema::config_transformador;
ConfigNidoAbeja Sistema::config_nido_abeja;
EstadoSistema Sistema::estado;

// Hardware
TAMC_GT911 touchController(
    Hardware::Touch::SDA_PIN, 
    Hardware::Touch::SCL_PIN, 
    Hardware::Touch::INT_PIN, 
    Hardware::Touch::RST_PIN, 
    Hardware::SCREEN_WIDTH, 
    Hardware::SCREEN_HEIGHT
);

static lv_display_t *disp;
static lv_color_t *disp_draw_buf;

// =========================================================================
// VARIABLES PARA DEBUG Y LOGGING (CORE 0)
// =========================================================================
struct DebugInfo {
    unsigned long ultima_vuelta_registrada = 0;
    unsigned long ultima_capa_registrada = 0;
    unsigned long tiempo_ultimo_log = 0;
    bool bobinado_iniciado = false;
};
static DebugInfo debug_info;

// =========================================================================
// FUNCIONES DE HARDWARE (CORE 0)
// =========================================================================
void init_hardware() {
    pinMode(Hardware::Motor::STEP_X_PIN, OUTPUT);
    pinMode(Hardware::Motor::DIR_X_PIN, OUTPUT);
    pinMode(Hardware::Motor::ENABLE_X_PIN, OUTPUT);
    pinMode(Hardware::Motor::LIMIT_X_PIN, INPUT_PULLUP);
    pinMode(Hardware::Motor::STEP_Y_PIN, OUTPUT);
    pinMode(Hardware::Motor::DIR_Y_PIN, OUTPUT);
    pinMode(Hardware::Motor::ENABLE_Y_PIN, OUTPUT);
    pinMode(Hardware::Motor::LIMIT_Y_PIN, INPUT_PULLUP);
    digitalWrite(Hardware::Motor::ENABLE_X_PIN, HIGH);
    digitalWrite(Hardware::Motor::ENABLE_Y_PIN, HIGH);
    Serial.println("Hardware inicializado");
}

void UIHandlers::homing_ejes() {
    Serial.println(">>> INICIANDO HOMING <<<");
    Sistema::estado.estado = EstadoBobinado::HOMING;
    Sistema::estado.movimiento_manual_activo = true;
    
    digitalWrite(Hardware::Motor::ENABLE_X_PIN, LOW);
    digitalWrite(Hardware::Motor::ENABLE_Y_PIN, LOW);
    delay(50);
    
    digitalWrite(Hardware::Motor::DIR_X_PIN, LOW);
    int pasos = 0;
    const int MAX_PASOS = Hardware::Motor::PASOS_POR_MM_X * 500;
    
    while (pasos < MAX_PASOS) {
        bool limite = !digitalRead(Hardware::Motor::LIMIT_X_PIN);
        if (limite) {
            Serial.println("Límite detectado!");
            break;
        }
        digitalWrite(Hardware::Motor::STEP_X_PIN, HIGH);
        delayMicroseconds(Hardware::Motor::STEP_PULSE_US);
        digitalWrite(Hardware::Motor::STEP_X_PIN, LOW);
        delayMicroseconds(Hardware::Motor::HOMING_SPEED_DELAY);
        pasos++;
    }
    
    digitalWrite(Hardware::Motor::DIR_X_PIN, HIGH);
    for (int i = 0; i < Hardware::Motor::HOMING_BACK_STEPS; i++) {
        digitalWrite(Hardware::Motor::STEP_X_PIN, HIGH);
        delayMicroseconds(Hardware::Motor::STEP_PULSE_US);
        digitalWrite(Hardware::Motor::STEP_X_PIN, LOW);
        delayMicroseconds(Hardware::Motor::HOMING_SPEED_DELAY);
    }
    
    Sistema::estado.reset();
    Sistema::estado.estado = EstadoBobinado::LISTO;
    Sistema::estado.movimiento_manual_activo = false;
    Serial.println(">>> HOMING COMPLETADO <<<");
}

void UIHandlers::move_motor_steps_safe(int step_pin, int dir_pin, int steps, int delay_us) {
    if (steps == 0) return;
    Sistema::estado.movimiento_manual_activo = true;
    int enable_pin = (step_pin == Hardware::Motor::STEP_X_PIN) ? 
                     Hardware::Motor::ENABLE_X_PIN : Hardware::Motor::ENABLE_Y_PIN;
    bool was_enabled = (digitalRead(enable_pin) == LOW);
    if (!was_enabled) {
        digitalWrite(enable_pin, LOW);
        delayMicroseconds(100);
    }
    digitalWrite(dir_pin, (steps > 0) ? HIGH : LOW);
    steps = abs(steps);
    for (int i = 0; i < steps; i++) {
        digitalWrite(step_pin, HIGH);
        delayMicroseconds(delay_us);
        digitalWrite(step_pin, LOW);
        delayMicroseconds(delay_us);
    }
    if (!was_enabled) digitalWrite(enable_pin, HIGH);
    Sistema::estado.movimiento_manual_activo = false;
}

// =========================================================================
// TAREA DE LOGGING Y DEBUG (CORE 0)
// =========================================================================
void logging_task(void *pvParameters) {
    Serial.println("Tarea de Logging iniciada en Core 0");
    
    for (;;) {
        unsigned long ahora = millis();
        
        // Detectar inicio de bobinado
        if (Sistema::estado.estado == EstadoBobinado::BOBINANDO && !debug_info.bobinado_iniciado) {
            Serial.println("========================================");
            Serial.println(">>> INICIANDO NUEVO BOBINADO <<<");
            Serial.print("Modo: ");
            Serial.println(Sistema::estado.modo == ModoBobinado::TRANSFORMADOR ? "TRANSFORMADOR" : "NIDO DE ABEJA");
            
            if (Sistema::estado.modo == ModoBobinado::TRANSFORMADOR) {
                Serial.print("Vueltas totales: "); 
                Serial.println(Sistema::config_transformador.vueltas_total);
                Serial.print("Velocidad objetivo: "); 
                Serial.print(Sistema::config_transformador.velocidad_rpm); 
                Serial.println(" RPM");
            } else {
                Serial.print("Vueltas totales: "); 
                Serial.println(Sistema::config_nido_abeja.num_vueltas);
                Serial.print("Desfase: "); 
                Serial.print(Sistema::config_nido_abeja.desfase_grados); 
                Serial.println("°");
                Serial.print("Factor desfase: "); 
                Serial.println(Sistema::config_nido_abeja.factor_desfase, 4);
                Serial.print("Vueltas por capa: "); 
                Serial.println(Sistema::config_nido_abeja.vueltas_por_capa);
            }
            Serial.println("========================================");
            
            debug_info.bobinado_iniciado = true;
            debug_info.ultima_vuelta_registrada = 0;
            debug_info.ultima_capa_registrada = 0;
        }
        
        // Detectar fin de bobinado
        if (Sistema::estado.estado != EstadoBobinado::BOBINANDO && debug_info.bobinado_iniciado) {
            Serial.println("\n========================================");
            Serial.println(">>> BOBINADO FINALIZADO <<<");
            Serial.print("Estado final: ");
            switch(Sistema::estado.estado) {
                case EstadoBobinado::LISTO: Serial.println("COMPLETADO"); break;
                case EstadoBobinado::PAUSADO: Serial.println("PAUSADO"); break;
                case EstadoBobinado::ERROR: Serial.println("ERROR"); break;
                default: Serial.println("DESCONOCIDO"); break;
            }
            Serial.print("Vueltas completadas: "); 
            Serial.println(Sistema::estado.vueltas_completadas);
            Serial.print("Capas completadas: "); 
            Serial.println(Sistema::estado.capas_completadas);
            Serial.print("Posición final X: "); 
            Serial.println(Sistema::estado.pasos_X_acumulados);
            Serial.println("========================================");
            
            debug_info.bobinado_iniciado = false;
        }
        
        // Logging periódico durante bobinado (cada 5 segundos)
        if (Sistema::estado.estado == EstadoBobinado::BOBINANDO && 
            (ahora - debug_info.tiempo_ultimo_log) >= 5000) {
            
            Serial.print(">>> Estado: Vuelta ");
            Serial.print(Sistema::estado.vueltas_completadas);
            Serial.print("/");
            
            uint32_t vueltas_objetivo = (Sistema::estado.modo == ModoBobinado::TRANSFORMADOR) ? 
                                       Sistema::config_transformador.vueltas_total : 
                                       Sistema::config_nido_abeja.num_vueltas;
            Serial.print(vueltas_objetivo);
            Serial.print(" | Capa: ");
            Serial.print(Sistema::estado.capas_completadas + 1);
            Serial.print(" | Vel: ");
            Serial.print(Sistema::estado.rpm_actual, 1);
            Serial.print(" RPM | Pos X: ");
            Serial.println(Sistema::estado.pasos_X_acumulados);
            
            debug_info.tiempo_ultimo_log = ahora;
        }
        
        // Detectar nueva capa (solo en nido de abeja)
        if (Sistema::estado.modo == ModoBobinado::NIDO_ABEJA && 
            Sistema::estado.capas_completadas > debug_info.ultima_capa_registrada) {
            
            Serial.println("----------------------------------------");
            Serial.print(">>> CAPA ");
            Serial.print(Sistema::estado.capas_completadas);
            Serial.print(" COMPLETADA | Grosor: ");
            
            float grosor_actual = Sistema::estado.capas_completadas * Sistema::config_nido_abeja.diametro_hilo;
            Serial.print(grosor_actual, 2);
            Serial.println(" mm");
            Serial.println("----------------------------------------");
            
            debug_info.ultima_capa_registrada = Sistema::estado.capas_completadas;
        }
        
        // Dormir para no saturar el Core 0
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// =========================================================================
// LVGL DRIVERS
// =========================================================================
uint32_t millis_cb(void) { return millis(); }

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_display_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    touchController.read();
    if (touchController.isTouched && touchController.touches > 0) {
        data->point.x = touchController.points[0].x;
        data->point.y = touchController.points[0].y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// =========================================================================
// SETUP
// =========================================================================
void setup() {
    
    Serial.begin(115200);
    Serial.println("========================================");
    Serial.println("BOBINADORA CNC v10.1 - OPTIMIZADA");
    Serial.println("Core 0: UI + Logging + Comunicaciones");
    Serial.println("Core 1: SOLO Timing Crítico de Motores");
    Serial.println("========================================");
    
    Sistema::inicializar();
    init_hardware();
    Persistence.begin();
    Persistence.loadAll();
    
    if (!gfx->begin()) {
        Serial.println("Error GFX!");
        while (true);
    }
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
    gfx->fillScreen(RGB565_BLACK);
    
    Wire.begin(Hardware::Touch::SDA_PIN, Hardware::Touch::SCL_PIN);
    touchController.begin();
    touchController.setRotation(ROTATION_NORMAL);
    
    lv_init();
    lv_tick_set_cb(millis_cb);
    UI::init_styles();
    
    uint32_t bufSize = Hardware::SCREEN_WIDTH * 40;
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    if (!disp_draw_buf) {
        disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_DEFAULT);
    }
    
    disp = lv_display_create(Hardware::SCREEN_WIDTH, Hardware::SCREEN_HEIGHT);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, disp_draw_buf, NULL, bufSize, LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);
    
    UI::Numpad::create();
    UIScreens::init_all_screens();
    
    if (UIScreens::screen_main) {
        lv_scr_load(UIScreens::screen_main);
    }
    
    UIHandlers::homing_ejes();
    
    lv_timer_create([](lv_timer_t *timer) {
        UIHandlers::update_winding_screen();
    }, 100, NULL);
    
    // =========================================================================
    // CREAR TAREAS EN CORES ESPECÍFICOS
    // =========================================================================
    
    // CORE 1: Tarea de motores (MÁXIMA PRIORIDAD - TIMING CRÍTICO)
    xTaskCreatePinnedToCore(
        motor_control_task_optimized,  // <<<< NUEVA FUNCIÓN OPTIMIZADA
        "MotorCore1",
        Hardware::Memory::TASK_STACK_SIZE,
        NULL,
        configMAX_PRIORITIES - 1,  // <<<< MÁXIMA PRIORIDAD
        NULL,
        1  // <<<< CORE 1 DEDICADO
    );
    Serial.println("✓ Tarea de Motores creada en Core 1 (Prioridad máxima)");
    
    // CORE 0: Tarea de logging y debug (PRIORIDAD BAJA)
    xTaskCreatePinnedToCore(
        logging_task,
        "LoggingCore0",
        4096,
        NULL,
        1,  // <<<< PRIORIDAD BAJA
        NULL,
        0  // <<<< CORE 0
    );
    Serial.println("✓ Tarea de Logging creada en Core 0 (Prioridad baja)");
    
    Serial.println("========================================");
    Serial.println("Sistema iniciado correctamente!");
    Serial.println("========================================");
    Sistema::estado.imprimir_estado();
}

// =========================================================================
// LOOP (CORE 0)
// =========================================================================
void loop() {
    // Este loop corre en Core 0 - Maneja LVGL y UI
    lv_timer_handler();
    delay(Hardware::LVGL_TICK_PERIOD);
}

/*
================================================================================
INSTRUCCIONES DE MIGRACIÓN DESDE v10.0 A v10.1:

1. Guardar backup de tu proyecto actual
2. Crear nuevo archivo: motor_task_optimized.h
3. Reemplazar archivo principal con este código
4. Compilar y probar con velocidades bajas primero (30-50 RPM)
5. Ir aumentando velocidad gradualmente hasta encontrar el límite estable

CAMBIOS CLAVE:
- Core 1 ahora SOLO genera pulsos (sin Serial.print, sin delays largos)
- Toda la lógica de logging se movió al Core 0
- Rampa de aceleración optimizada
- Loops críticos sin interrupciones

TESTING:
1. Probar homing - debe funcionar igual
2. Probar modo transformador a baja velocidad
3. Probar modo nido de abeja a baja velocidad
4. Aumentar velocidad progresivamente
5. Verificar que no hay pérdida de pasos monitoreando posición final

BENEFICIOS ESPERADOS:
- ✓ Eliminación de pérdida de pulsos a alta velocidad
- ✓ Movimientos más suaves y precisos
- ✓ Posibilidad de aumentar RPM máxima
- ✓ Mejor sincronización X-Y en nido de abeja
- ✓ Corrección de posición más precisa

Si encuentras problemas, revisa:
1. Que todos los #include estén presentes
2. Que config.h tenga las definiciones correctas
3. Monitor Serial para ver mensajes de debug (ahora desde Core 0)

================================================================================
*/