#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <math.h>

// =========================================================================
// CONFIGURACIÓN DE HARDWARE
// =========================================================================
namespace Hardware {
    // Configuración de Pantalla
    constexpr uint16_t SCREEN_WIDTH = 480;
    constexpr uint16_t SCREEN_HEIGHT = 272;
    constexpr uint8_t LVGL_TICK_PERIOD = 5;
    
    // Configuración Touch Controller GT911
    namespace Touch {
        constexpr uint8_t SDA_PIN = 8;
        constexpr uint8_t SCL_PIN = 4;
        constexpr uint8_t INT_PIN = 3;
        constexpr uint8_t RST_PIN = 38;
    }
    
    // Configuración de Motores
    namespace Motor {
        // Eje X (Carro)
        constexpr uint8_t STEP_X_PIN = 5;
        constexpr uint8_t DIR_X_PIN = 9;
        constexpr uint8_t ENABLE_X_PIN = 14;
        constexpr uint8_t LIMIT_X_PIN = 46;
        // NUEVO: Configuración de finales de carrera
        constexpr bool LIMIT_X_INVERTED = false;  // false = NA (normalmente abierto)
        constexpr bool LIMIT_Y_INVERTED = false;  
        constexpr int HOMING_SPEED_DELAY = 1000;   // Velocidad de homing (microsegundos)
        constexpr int HOMING_BACK_STEPS = 100;     // Pasos para retroceder después de tocar límite
        
        // Eje Y (Mandril)
        constexpr uint8_t STEP_Y_PIN = 6;
        constexpr uint8_t DIR_Y_PIN = 7;
        constexpr uint8_t ENABLE_Y_PIN = 15;
        constexpr uint8_t LIMIT_Y_PIN = 16;
        
        // Parámetros de Control
        constexpr uint8_t STEP_PULSE_US = 5;
        constexpr float PASOS_POR_MM_X = 320.0f;
        constexpr uint16_t PASOS_POR_VUELTA_Y = 3200;
        constexpr float RPM_ACCELERATION = 250.0f;
        constexpr float MIN_RPM = 1.0f;
        constexpr float MAX_RPM = 500.0f;
    }
    
    // Configuración de Memoria
    namespace Memory {
        constexpr uint32_t LVGL_BUFFER_SIZE = 480 * 40;  // 40 líneas
        constexpr uint32_t TASK_STACK_SIZE = 10000;
        constexpr uint8_t MOTOR_TASK_PRIORITY = 1;
        constexpr uint8_t MOTOR_TASK_CORE = 1;
    }
}

// =========================================================================
// ENUMERACIONES DE ESTADO Y MODO
// =========================================================================
enum class EstadoBobinado : uint8_t {
    LISTO = 0,
    BOBINANDO = 1,
    PAUSADO = 2,
    ERROR = 3,
    HOMING = 4,
    MANUAL = 5
};

enum class ModoBobinado : uint8_t {
    TRANSFORMADOR = 0,
    NIDO_ABEJA = 1
};

enum class TipoMovimiento : uint8_t {
    NINGUNO = 0,
    MANUAL_X_POS = 1,
    MANUAL_X_NEG = 2,
    MANUAL_Y_POS = 3,
    MANUAL_Y_NEG = 4,
    HOMING_X = 5,
    HOMING_Y = 6
};

// =========================================================================
// ESTRUCTURA DE CONFIGURACIÓN PARA MODO TRANSFORMADOR
// =========================================================================
struct ConfigTransformador {
    // Parámetros de entrada
    float diametro_alambre_mm = 0.5f;
    float longitud_bobinado_mm = 50.0f;
    uint32_t vueltas_total = 1000;
    float velocidad_rpm = 100.0f;
    bool detener_en_capas = false;
    
    // Parámetros calculados (no modificar directamente)
    uint32_t vueltas_por_capa = 0;
    uint32_t capas_estimadas = 0;
    float grosor_bobinado_mm = 0.0f;
    float step_ratio_X_per_Y = 0.0f;
    int32_t limite_pasos_X = 0;
    
    // Métodos
    void calcular_parametros() {
        // Calcular vueltas por capa
        if (diametro_alambre_mm > 0.0f) {
            vueltas_por_capa = static_cast<uint32_t>(floor(longitud_bobinado_mm / diametro_alambre_mm));
        } else {
            vueltas_por_capa = 0;
        }
        
        // Calcular ratio de pasos X por cada paso Y
        if (Hardware::Motor::PASOS_POR_VUELTA_Y > 0) {
            float pasos_X_por_vuelta = diametro_alambre_mm * Hardware::Motor::PASOS_POR_MM_X;
            step_ratio_X_per_Y = pasos_X_por_vuelta / static_cast<float>(Hardware::Motor::PASOS_POR_VUELTA_Y);
        } else {
            step_ratio_X_per_Y = 0.0f;
        }
        
        // Calcular límite de pasos en X
        limite_pasos_X = static_cast<int32_t>(roundf(longitud_bobinado_mm * Hardware::Motor::PASOS_POR_MM_X));
        
        // Calcular capas estimadas
        if (vueltas_por_capa > 0) {
            capas_estimadas = static_cast<uint32_t>(ceil(static_cast<float>(vueltas_total) / vueltas_por_capa));
            if (capas_estimadas == 0 && vueltas_total > 0) {
                capas_estimadas = 1;
            }
        } else {
            capas_estimadas = 0;
        }
        
        // Calcular grosor final estimado
        grosor_bobinado_mm = static_cast<float>(capas_estimadas) * diametro_alambre_mm;
    }
    
    bool validar() const {
        if (diametro_alambre_mm <= 0.0f || diametro_alambre_mm > 10.0f) {
            Serial.println("ERROR: Diámetro de alambre inválido");
            return false;
        }
        if (longitud_bobinado_mm <= 0.0f || longitud_bobinado_mm > 500.0f) {
            Serial.println("ERROR: Longitud de bobinado inválida");
            return false;
        }
        if (vueltas_total == 0 || vueltas_total > 100000) {
            Serial.println("ERROR: Número de vueltas inválido");
            return false;
        }
        if (velocidad_rpm < Hardware::Motor::MIN_RPM || velocidad_rpm > Hardware::Motor::MAX_RPM) {
            Serial.println("ERROR: Velocidad RPM fuera de rango");
            return false;
        }
        return true;
    }
    
    void imprimir_debug() const {
        Serial.println("=== CONFIG TRANSFORMADOR ===");
        Serial.print("Diámetro alambre: "); Serial.print(diametro_alambre_mm); Serial.println(" mm");
        Serial.print("Longitud bobinado: "); Serial.print(longitud_bobinado_mm); Serial.println(" mm");
        Serial.print("Vueltas totales: "); Serial.println(vueltas_total);
        Serial.print("Velocidad: "); Serial.print(velocidad_rpm); Serial.println(" RPM");
        Serial.print("Vueltas/capa: "); Serial.println(vueltas_por_capa);
        Serial.print("Capas estimadas: "); Serial.println(capas_estimadas);
        Serial.print("Grosor final: "); Serial.print(grosor_bobinado_mm); Serial.println(" mm");
        Serial.print("Step ratio X/Y: "); Serial.println(step_ratio_X_per_Y, 6);
        Serial.println("============================");
    }
};

// =========================================================================
// ESTRUCTURA DE CONFIGURACIÓN PARA MODO NIDO DE ABEJA
// =========================================================================
struct ConfigNidoAbeja {
    // Parámetros de entrada
    float diametro_hilo = 0.2f;
    float diametro_carrete = 20.0f;
    float ancho_carrete = 10.0f;
    float desfase_grados = 72.0f;
    uint32_t num_vueltas = 500;
    float velocidad_rpm = 100.0f;
    
    // Parámetros calculados (no modificar directamente)
    float factor_desfase = 1.2f;
    uint32_t vueltas_por_capa = 0;
    uint32_t capas_estimadas = 0;
    float grosor_estimado = 0.0f;
    float step_ratio_X_per_Y = 0.0f;
    int32_t pasos_X_recorrido_completo = 0;
    int32_t pasos_Y_por_ciclo = 0;
    
    // Métodos
    void calcular_parametros() {
        // Convertir grados a factor de desfase
        factor_desfase = 1.0f + (desfase_grados / 360.0f);
        
        // Validar factor mínimo
        if (factor_desfase < 0.1f) {
            factor_desfase = 0.1f;
            desfase_grados = (factor_desfase - 1.0f) * 360.0f;
        }
        
        // Calcular pasos totales del recorrido X (ida + vuelta)
        pasos_X_recorrido_completo = static_cast<int32_t>(ancho_carrete * Hardware::Motor::PASOS_POR_MM_X * 2);
        
        // Calcular pasos de Y para un ciclo completo
        pasos_Y_por_ciclo = static_cast<int32_t>(Hardware::Motor::PASOS_POR_VUELTA_Y * factor_desfase);
        
        // Calcular ratio X/Y
        if (pasos_Y_por_ciclo > 0) {
            step_ratio_X_per_Y = static_cast<float>(pasos_X_recorrido_completo) / pasos_Y_por_ciclo;
        } else {
            step_ratio_X_per_Y = 0.0f;
        }
        
        // Calcular vueltas por capa
        if (fabs(desfase_grados) > 0.1f) {
            vueltas_por_capa = static_cast<uint32_t>(360.0f / fabs(desfase_grados));
        } else {
            vueltas_por_capa = num_vueltas;
        }
        
        // Calcular capas estimadas
        if (vueltas_por_capa > 0) {
            capas_estimadas = static_cast<uint32_t>(ceil(static_cast<float>(num_vueltas) / vueltas_por_capa));
        } else {
            capas_estimadas = 1;
        }
        
        if (capas_estimadas < 1) capas_estimadas = 1;
        
        // Calcular grosor estimado
        grosor_estimado = static_cast<float>(capas_estimadas) * diametro_hilo;
    }
    
    bool validar() const {
        if (diametro_hilo <= 0.0f || diametro_hilo > 5.0f) {
            Serial.println("ERROR: Diámetro de hilo inválido");
            return false;
        }
        if (diametro_carrete <= 0.0f || diametro_carrete > 200.0f) {
            Serial.println("ERROR: Diámetro de carrete inválido");
            return false;
        }
        if (ancho_carrete <= 0.0f || ancho_carrete > 300.0f) {
            Serial.println("ERROR: Ancho de carrete inválido");
            return false;
        }
        if (desfase_grados < -360.0f || desfase_grados > 360.0f) {
            Serial.println("ERROR: Desfase en grados fuera de rango");
            return false;
        }
        if (num_vueltas == 0 || num_vueltas > 100000) {
            Serial.println("ERROR: Número de vueltas inválido");
            return false;
        }
        if (velocidad_rpm < Hardware::Motor::MIN_RPM || velocidad_rpm > Hardware::Motor::MAX_RPM) {
            Serial.println("ERROR: Velocidad RPM fuera de rango");
            return false;
        }
        return true;
    }
    
    void imprimir_debug() const {
        Serial.println("=== CONFIG NIDO DE ABEJA ===");
        Serial.print("Diámetro hilo: "); Serial.print(diametro_hilo); Serial.println(" mm");
        Serial.print("Diámetro carrete: "); Serial.print(diametro_carrete); Serial.println(" mm");
        Serial.print("Ancho carrete: "); Serial.print(ancho_carrete); Serial.println(" mm");
        Serial.print("Desfase: "); Serial.print(desfase_grados); Serial.println(" grados");
        Serial.print("Factor desfase: "); Serial.println(factor_desfase, 4);
        Serial.print("Vueltas totales: "); Serial.println(num_vueltas);
        Serial.print("Vueltas/capa: "); Serial.println(vueltas_por_capa);
        Serial.print("Capas estimadas: "); Serial.println(capas_estimadas);
        Serial.print("Grosor estimado: "); Serial.print(grosor_estimado); Serial.println(" mm");
        Serial.print("Step ratio X/Y: "); Serial.println(step_ratio_X_per_Y, 6);
        Serial.println("============================");
    }
};

// =========================================================================
// ESTRUCTURA DE ESTADO DEL SISTEMA
// =========================================================================
struct EstadoSistema {
    // Estado principal
    EstadoBobinado estado = EstadoBobinado::LISTO;
    ModoBobinado modo = ModoBobinado::TRANSFORMADOR;
    
    // Contadores de progreso
    volatile uint32_t vueltas_completadas = 0;
    volatile uint32_t vueltas_capa_actual = 0;
    volatile uint32_t capas_completadas = 0;
    
    // Control de movimiento del carro (Eje X)
    volatile int8_t sentido_X = 1;
    volatile int32_t pasos_X_acumulados = 0;
    
    // Control de velocidad
    volatile float rpm_actual = 0.0f;
    volatile float rpm_objetivo = 0.0f;
    
    // Flags de control
    volatile bool reset_solicitado = false;
    volatile bool movimiento_manual_activo = false;
    volatile bool mantener_motor_en_pausa = true;
    volatile bool bobinado_completado = false;
    
    // Tipo de movimiento manual actual
    TipoMovimiento movimiento_manual = TipoMovimiento::NINGUNO;
    
    // Métodos
    void reset() {
        estado = EstadoBobinado::LISTO;
        vueltas_completadas = 0;
        vueltas_capa_actual = 0;
        capas_completadas = 0;
        sentido_X = 1;
        pasos_X_acumulados = 0;
        rpm_actual = 0.0f;
        rpm_objetivo = 0.0f;
        reset_solicitado = false;
        movimiento_manual_activo = false;
        bobinado_completado = false;
        movimiento_manual = TipoMovimiento::NINGUNO;
        
        Serial.println("Sistema reseteado");
    }
    
    void iniciar_bobinado() {
        if (estado == EstadoBobinado::LISTO) {
            estado = EstadoBobinado::BOBINANDO;
            bobinado_completado = false;
            Serial.println("Bobinado iniciado");
        }
    }
    
    void pausar_bobinado() {
        if (estado == EstadoBobinado::BOBINANDO) {
            estado = EstadoBobinado::PAUSADO;
            Serial.println("Bobinado pausado");
        }
    }
    
    void reanudar_bobinado() {
        if (estado == EstadoBobinado::PAUSADO) {
            estado = EstadoBobinado::BOBINANDO;
            Serial.println("Bobinado reanudado");
        }
    }
    
    void detener_bobinado() {
        estado = EstadoBobinado::LISTO;
        rpm_objetivo = 0.0f;
        Serial.println("Bobinado detenido");
    }
    
    bool puede_mover_manual() const {
        return (estado == EstadoBobinado::LISTO || estado == EstadoBobinado::PAUSADO);
    }
    
    float obtener_progreso_porcentaje() const {
        uint32_t vueltas_objetivo = 0;
        
        if (modo == ModoBobinado::TRANSFORMADOR) {
            // Se obtendría de config_transformador.vueltas_total
            return 0.0f; // Placeholder
        } else {
            // Se obtendría de config_nido_abeja.num_vueltas
            return 0.0f; // Placeholder
        }
    }
    
    void imprimir_estado() const {
        Serial.println("=== ESTADO DEL SISTEMA ===");
        Serial.print("Estado: ");
        switch(estado) {
            case EstadoBobinado::LISTO: Serial.println("LISTO"); break;
            case EstadoBobinado::BOBINANDO: Serial.println("BOBINANDO"); break;
            case EstadoBobinado::PAUSADO: Serial.println("PAUSADO"); break;
            case EstadoBobinado::ERROR: Serial.println("ERROR"); break;
            case EstadoBobinado::HOMING: Serial.println("HOMING"); break;
            case EstadoBobinado::MANUAL: Serial.println("MANUAL"); break;
        }
        Serial.print("Modo: ");
        Serial.println(modo == ModoBobinado::TRANSFORMADOR ? "TRANSFORMADOR" : "NIDO DE ABEJA");
        Serial.print("Vueltas: "); Serial.print(vueltas_completadas);
        Serial.print(" | Capa actual: "); Serial.print(capas_completadas + 1);
        Serial.print(" | RPM: "); Serial.println(rpm_actual);
        Serial.println("==========================");
    }
};

// =========================================================================
// SINGLETON PARA ACCESO GLOBAL (OPCIONAL)
// =========================================================================
class Sistema {
private:
    Sistema() {}
    
public:
    static ConfigTransformador config_transformador;
    static ConfigNidoAbeja config_nido_abeja;
    static EstadoSistema estado;
    
    static void inicializar() {
        config_transformador.calcular_parametros();
        config_nido_abeja.calcular_parametros();
        estado.reset();
        
        Serial.println("Sistema inicializado");
    }
    
    static void actualizar_configuracion() {
        if (estado.modo == ModoBobinado::TRANSFORMADOR) {
            if (config_transformador.validar()) {
                config_transformador.calcular_parametros();
                config_transformador.imprimir_debug();
            }
        } else {
            if (config_nido_abeja.validar()) {
                config_nido_abeja.calcular_parametros();
                config_nido_abeja.imprimir_debug();
            }
        }
    }
};

// Definición de los miembros estáticos (colocar en el .cpp principal)
// ConfigTransformador Sistema::config_transformador;
// ConfigNidoAbeja Sistema::config_nido_abeja;
// EstadoSistema Sistema::estado;

#endif // CONFIG_H