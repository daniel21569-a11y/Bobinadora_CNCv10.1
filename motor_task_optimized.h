// =========================================================================
// TAREA DE CONTROL DE MOTORES OPTIMIZADA - CORE 1
// VERSIÃ"N: 10.1 - SIN INTERFERENCIAS
// =========================================================================
// CAMBIOS CLAVE:
// - Core 1: SOLO genera pulsos STEP/DIR (timing crítico)
// - Core 0: Cálculos, UI, Serial.print, lógica compleja
// - Comunicación mediante estructuras volatiles
// - Sin Serial.print en Core 1
// - Sin delays largos en generación de pulsos
// =========================================================================

#ifndef MOTOR_TASK_OPTIMIZED_H
#define MOTOR_TASK_OPTIMIZED_H

#include "config.h"

// =========================================================================
// ESTRUCTURA DE ESTADO LOCAL DEL MOTOR (SOLO CORE 1)
// =========================================================================
struct MotorStateCore1 {
    // Timing de pulsos (variables críticas)
    unsigned long last_step_time_Y;
    unsigned long last_speed_update_us;
    float step_period_us_Y_current;
    
    // Acumuladores de pasos
    float X_step_accumulator_transformer;
    float X_step_accumulator_honeycomb;
    
    // Estado de nido de abeja
    unsigned long pasos_Y_en_ciclo_actual;
    long pasos_Y_por_ciclo_honeycomb;
    long pasos_X_recorrido_ida;
    long mitad_ciclo_Y;
    bool direccion_X_honeycomb;
    long posicion_X_inicio_ciclo;
    long pasos_X_objetivo_anterior;
    
    // Contadores
    unsigned long current_Y_steps;
    
    // Estado previo
    EstadoBobinado ultimo_estado;
    
    // Flags de inicialización
    bool ciclo_inicializado;
    
    void reset() {
        last_step_time_Y = micros();
        last_speed_update_us = last_step_time_Y;
        step_period_us_Y_current = 0.0f;
        
        X_step_accumulator_transformer = 0.0f;
        X_step_accumulator_honeycomb = 0.0f;
        
        pasos_Y_en_ciclo_actual = 0;
        pasos_Y_por_ciclo_honeycomb = 0;
        pasos_X_recorrido_ida = 0;
        mitad_ciclo_Y = 0;
        direccion_X_honeycomb = true;
        posicion_X_inicio_ciclo = 0;
        pasos_X_objetivo_anterior = 0;
        
        current_Y_steps = 0;
        ultimo_estado = EstadoBobinado::LISTO;
        ciclo_inicializado = false;
    }
};

// Variable local del Core 1 (no compartida)
static MotorStateCore1 motor_state_c1;

// =========================================================================
// FUNCIONES INLINE PARA MÁXIMA VELOCIDAD
// =========================================================================

// Genera un pulso de STEP en el pin especificado
inline void pulse_step_pin(int pin) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(Hardware::Motor::STEP_PULSE_US);
    digitalWrite(pin, LOW);
}

// Establece dirección del motor X
inline void set_dir_x_fast(bool forward) {
    digitalWrite(Hardware::Motor::DIR_X_PIN, forward ? HIGH : LOW);
}

// Establece dirección del motor Y
inline void set_dir_y_fast(bool forward) {
    digitalWrite(Hardware::Motor::DIR_Y_PIN, forward ? HIGH : LOW);
}

// Genera múltiples pasos en X con delay mínimo (para correcciones)
inline void step_x_multiple(long num_steps, bool forward) {
    if (num_steps == 0) return;
    
    set_dir_x_fast(forward);
    delayMicroseconds(5); // Setup time del driver
    
    for (long i = 0; i < num_steps; i++) {
        pulse_step_pin(Hardware::Motor::STEP_X_PIN);
        delayMicroseconds(Hardware::Motor::STEP_PULSE_US); // Mínimo delay entre pasos
    }
}

// =========================================================================
// FUNCIÓN DE ACTUALIZACIÓN DE VELOCIDAD CON RAMPA
// =========================================================================
inline void update_speed_ramp(unsigned long now) {
    float dt_s = (now - motor_state_c1.last_speed_update_us) / 1000000.0f;
    motor_state_c1.last_speed_update_us = now;
    
    if (Sistema::estado.estado == EstadoBobinado::BOBINANDO) {
        Sistema::estado.rpm_objetivo = (Sistema::estado.modo == ModoBobinado::TRANSFORMADOR) ? 
                                      Sistema::config_transformador.velocidad_rpm : 
                                      Sistema::config_nido_abeja.velocidad_rpm;
    } else {
        Sistema::estado.rpm_objetivo = 0.0f;
    }
    
    // Rampa de aceleración/desaceleración
    if (Sistema::estado.rpm_actual < Sistema::estado.rpm_objetivo) {
        Sistema::estado.rpm_actual = fminf(Sistema::estado.rpm_objetivo, 
                                          Sistema::estado.rpm_actual + Hardware::Motor::RPM_ACCELERATION * dt_s);
    } else if (Sistema::estado.rpm_actual > Sistema::estado.rpm_objetivo) {
        Sistema::estado.rpm_actual = fmaxf(Sistema::estado.rpm_objetivo, 
                                          Sistema::estado.rpm_actual - Hardware::Motor::RPM_ACCELERATION * dt_s);
    }
    
    // Calcular período de paso para Y
    motor_state_c1.step_period_us_Y_current = (Sistema::estado.rpm_actual > 0.01f) ? 
        1000000.0f / ((Sistema::estado.rpm_actual * Hardware::Motor::PASOS_POR_VUELTA_Y) / 60.0f) : 0.0f;
}

// =========================================================================
// BOBINADO MODO TRANSFORMADOR - OPTIMIZADO
// =========================================================================
inline void process_transformer_step() {
    long limite_pasos_X = Sistema::config_transformador.limite_pasos_X;
    
    if (limite_pasos_X > 0 && Sistema::config_transformador.step_ratio_X_per_Y > 0.0f) {
        motor_state_c1.X_step_accumulator_transformer += Sistema::config_transformador.step_ratio_X_per_Y;
        
        // Generar pasos acumulados en X
        while (motor_state_c1.X_step_accumulator_transformer >= 1.0f) {
            set_dir_x_fast(Sistema::estado.sentido_X == 1);
            pulse_step_pin(Hardware::Motor::STEP_X_PIN);
            
            Sistema::estado.pasos_X_acumulados += Sistema::estado.sentido_X;
            motor_state_c1.X_step_accumulator_transformer -= 1.0f;
            
            // Verificar límites de capa
            if (Sistema::estado.pasos_X_acumulados >= limite_pasos_X || 
                Sistema::estado.pasos_X_acumulados <= 0) {
                
                Sistema::estado.pasos_X_acumulados = (Sistema::estado.pasos_X_acumulados >= limite_pasos_X) ? 
                                                     limite_pasos_X : 0;
                Sistema::estado.capas_completadas++;
                Sistema::estado.vueltas_capa_actual = 0;
                Sistema::estado.sentido_X *= -1;
                
                // Pausa entre capas si está configurado
                if (Sistema::config_transformador.detener_en_capas && 
                    Sistema::estado.vueltas_completadas < Sistema::config_transformador.vueltas_total) {
                    Sistema::estado.estado = EstadoBobinado::PAUSADO;
                }
                break;
            }
        }
    }
    
    // Contar vuelta completa de Y
    motor_state_c1.current_Y_steps++;
    if (motor_state_c1.current_Y_steps >= (unsigned long)Hardware::Motor::PASOS_POR_VUELTA_Y) {
        motor_state_c1.current_Y_steps = 0;
        Sistema::estado.vueltas_completadas++;
        Sistema::estado.vueltas_capa_actual++;
    }
}

// =========================================================================
// BOBINADO MODO NIDO DE ABEJA - OPTIMIZADO
// =========================================================================
inline void init_honeycomb_cycle() {
    motor_state_c1.pasos_X_recorrido_ida = (long)(Sistema::config_nido_abeja.ancho_carrete * Hardware::Motor::PASOS_POR_MM_X);
    motor_state_c1.pasos_Y_por_ciclo_honeycomb = (long)(Hardware::Motor::PASOS_POR_VUELTA_Y * Sistema::config_nido_abeja.factor_desfase);
    motor_state_c1.mitad_ciclo_Y = motor_state_c1.pasos_Y_por_ciclo_honeycomb / 2;
    motor_state_c1.posicion_X_inicio_ciclo = 0;
    motor_state_c1.pasos_X_objetivo_anterior = 0;
    Sistema::estado.pasos_X_acumulados = 0;
    motor_state_c1.ciclo_inicializado = true;
    
    // Nota: Los Serial.print ahora deben hacerse desde Core 0
}

inline void process_honeycomb_step() {
    // Inicializar ciclo si es necesario
    if (!motor_state_c1.ciclo_inicializado) {
        init_honeycomb_cycle();
    }
    
    bool en_ida = (motor_state_c1.pasos_Y_en_ciclo_actual < motor_state_c1.mitad_ciclo_Y);
    long pasos_X_objetivo_acumulados;
    
    // Calcular posición objetivo de X según fase
    if (en_ida) {
        float progreso_ida = (float)motor_state_c1.pasos_Y_en_ciclo_actual / (float)motor_state_c1.mitad_ciclo_Y;
        pasos_X_objetivo_acumulados = (long)(progreso_ida * motor_state_c1.pasos_X_recorrido_ida);
        
        if (!motor_state_c1.direccion_X_honeycomb) {
            motor_state_c1.direccion_X_honeycomb = true;
            set_dir_x_fast(true);
            delayMicroseconds(5);
        }
    } else {
        long pasos_Y_en_vuelta = motor_state_c1.pasos_Y_en_ciclo_actual - motor_state_c1.mitad_ciclo_Y;
        float progreso_vuelta = (float)pasos_Y_en_vuelta / (float)motor_state_c1.mitad_ciclo_Y;
        pasos_X_objetivo_acumulados = (long)(motor_state_c1.pasos_X_recorrido_ida * (1.0f - progreso_vuelta));
        
        if (motor_state_c1.direccion_X_honeycomb) {
            motor_state_c1.direccion_X_honeycomb = false;
            set_dir_x_fast(false);
            delayMicroseconds(5);
        }
    }
    
    // Calcular diferencia de pasos necesarios
    long diferencia_pasos = pasos_X_objetivo_acumulados - motor_state_c1.pasos_X_objetivo_anterior;
    motor_state_c1.pasos_X_objetivo_anterior = pasos_X_objetivo_acumulados;
    
    // Generar pasos incrementales (máximo 10 por ciclo para evitar bloqueos)
    if (diferencia_pasos > 0) {
        long pasos_a_generar = min(diferencia_pasos, 10L);
        for (long i = 0; i < pasos_a_generar; i++) {
            pulse_step_pin(Hardware::Motor::STEP_X_PIN);
            Sistema::estado.pasos_X_acumulados++;
        }
    } else if (diferencia_pasos < 0) {
        long pasos_a_generar = min(-diferencia_pasos, 10L);
        for (long i = 0; i < pasos_a_generar; i++) {
            pulse_step_pin(Hardware::Motor::STEP_X_PIN);
            Sistema::estado.pasos_X_acumulados--;
        }
    }
    
    // Incrementar contador de pasos Y en ciclo
    motor_state_c1.pasos_Y_en_ciclo_actual++;
    
    // Verificar fin de ciclo
    if (motor_state_c1.pasos_Y_en_ciclo_actual >= motor_state_c1.pasos_Y_por_ciclo_honeycomb) {
        Sistema::estado.vueltas_completadas++;
        Sistema::estado.vueltas_capa_actual++;
        
        // Corrección final de posición X (volver a cero)
        if (Sistema::estado.pasos_X_acumulados != 0) {
            long pasos_correccion = abs(Sistema::estado.pasos_X_acumulados);
            bool direccion_correccion = (Sistema::estado.pasos_X_acumulados > 0) ? false : true;
            step_x_multiple(pasos_correccion, direccion_correccion);
            Sistema::estado.pasos_X_acumulados = 0;
        }
        
        // Resetear para próximo ciclo
        motor_state_c1.pasos_Y_en_ciclo_actual = 0;
        motor_state_c1.pasos_X_objetivo_anterior = 0;
        motor_state_c1.direccion_X_honeycomb = true;
        set_dir_x_fast(true);
        
        // Actualizar capa
        if (Sistema::estado.vueltas_capa_actual >= Sistema::config_nido_abeja.vueltas_por_capa) {
            Sistema::estado.capas_completadas++;
            Sistema::estado.vueltas_capa_actual = 0;
        }
    }
}

// =========================================================================
// TAREA PRINCIPAL DE CONTROL DE MOTORES - CORE 1
// =========================================================================
void motor_control_task_optimized(void *pvParameters) {
    
    // Inicializar estado local
    motor_state_c1.reset();
    
    // NO imprimir en Serial desde aquí - usar flags para Core 0
    
    for (;;) {
        // =====================================================================
        // VERIFICACIÓN DE SEGURIDAD (límites)
        // =====================================================================
        if (Sistema::estado.estado == EstadoBobinado::BOBINANDO) {
            bool limite_x = !digitalRead(Hardware::Motor::LIMIT_X_PIN);
            if (limite_x) {
                Sistema::estado.estado = EstadoBobinado::ERROR;
                digitalWrite(Hardware::Motor::ENABLE_X_PIN, HIGH);
                digitalWrite(Hardware::Motor::ENABLE_Y_PIN, HIGH);
                // Setear flag para que Core 0 imprima el error
                continue;
            }
        }
        
        // =====================================================================
        // RESET DE TAREA
        // =====================================================================
        if (Sistema::estado.reset_solicitado) {
            motor_state_c1.reset();
            digitalWrite(Hardware::Motor::ENABLE_X_PIN, HIGH);
            digitalWrite(Hardware::Motor::ENABLE_Y_PIN, HIGH);
            Sistema::estado.reset_solicitado = false;
            vTaskDelay(10);
            continue;
        }
        
        // =====================================================================
        // DETECCIÓN DE INICIO DE BOBINADO
        // =====================================================================
        if (Sistema::estado.estado == EstadoBobinado::BOBINANDO && 
            motor_state_c1.ultimo_estado != EstadoBobinado::BOBINANDO) {
            
            motor_state_c1.reset();
            motor_state_c1.last_step_time_Y = micros();
            motor_state_c1.last_speed_update_us = micros();
        }
        motor_state_c1.ultimo_estado = Sistema::estado.estado;
        
        // =====================================================================
        // SKIP SI HAY MOVIMIENTO MANUAL ACTIVO
        // =====================================================================
        if (Sistema::estado.movimiento_manual_activo) {
            vTaskDelay(1);
            continue;
        }
        
        // =====================================================================
        // CONTROL DE HABILITACIÓN DE MOTORES
        // =====================================================================
        bool enable_motores = (Sistema::estado.estado == EstadoBobinado::BOBINANDO) || 
                             (Sistema::estado.estado == EstadoBobinado::PAUSADO && 
                              Sistema::estado.mantener_motor_en_pausa);
        digitalWrite(Hardware::Motor::ENABLE_X_PIN, !enable_motores);
        digitalWrite(Hardware::Motor::ENABLE_Y_PIN, !enable_motores);
        
        // =====================================================================
        // ACTUALIZACIÓN DE VELOCIDAD CON RAMPA
        // =====================================================================
        unsigned long now = micros();
        update_speed_ramp(now);
        
        // =====================================================================
        // GENERACIÓN DE PULSOS (TIMING CRÍTICO)
        // =====================================================================
        if (Sistema::estado.estado == EstadoBobinado::BOBINANDO && 
            motor_state_c1.step_period_us_Y_current > 0.0f) {
            
            // Verificar si es momento de generar paso en Y
            if ((now - motor_state_c1.last_step_time_Y) >= motor_state_c1.step_period_us_Y_current) {
                
                // PASO EN Y (MANDRIL)
                set_dir_y_fast(true); // Siempre gira en una dirección
                pulse_step_pin(Hardware::Motor::STEP_Y_PIN);
                motor_state_c1.last_step_time_Y = now;
                
                // PROCESAMIENTO SEGÚN MODO
                if (Sistema::estado.modo == ModoBobinado::TRANSFORMADOR) {
                    process_transformer_step();
                } else {
                    process_honeycomb_step();
                }
                
                // VERIFICAR FINALIZACIÓN
                uint32_t vueltas_objetivo = (Sistema::estado.modo == ModoBobinado::TRANSFORMADOR) ? 
                                           Sistema::config_transformador.vueltas_total : 
                                           Sistema::config_nido_abeja.num_vueltas;
                
                if (Sistema::estado.vueltas_completadas >= vueltas_objetivo) {
                    Sistema::estado.estado = EstadoBobinado::LISTO;
                    Sistema::estado.reset_solicitado = true;
                }
            }
        } else {
            // No hay trabajo, dormir brevemente
            vTaskDelay(1);
        }
    }
}

#endif // MOTOR_TASK_OPTIMIZED_H