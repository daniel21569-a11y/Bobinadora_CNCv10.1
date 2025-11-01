#include "ui_handlers.h"
#include "ui_screens.h"
#include "ui_components.h"
#include "config.h"
#include "persistence.h"
#include <Arduino.h>

namespace UIHandlers {
    
    void btn_navegacion_handler(lv_event_t *e) {
        lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
        const char *id = (const char *)lv_obj_get_user_data(target);
        
        if (strcmp(id, "SELECCION_MODO") == 0)
            lv_scr_load_anim(UIScreens::screen_modo_selection, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
        else if (strcmp(id, "CONFIG") == 0)
            lv_scr_load_anim(UIScreens::screen_config, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
        else if (strcmp(id, "CONFIG_HC") == 0)
            lv_scr_load_anim(UIScreens::screen_config_honeycomb, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
        else if (strcmp(id, "BOBINADO") == 0)
            lv_scr_load_anim(UIScreens::screen_winding, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
        else if (strcmp(id, "MANUAL") == 0)
            lv_scr_load_anim(UIScreens::screen_manual_control, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
        else if (strcmp(id, "PRINCIPAL") == 0)
            lv_scr_load_anim(UIScreens::screen_main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    }
    
    void btn_modo_handler(lv_event_t *e) {
        lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
        const char *id = (const char *)lv_obj_get_user_data(target);
        
        if (strcmp(id, "MODO_TRANSFORMADOR") == 0) {
            Sistema::estado.modo = ModoBobinado::TRANSFORMADOR;
            Serial.println("Modo TRANSFORMADOR seleccionado");
            lv_scr_load_anim(UIScreens::screen_config, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
        } else if (strcmp(id, "MODO_HONEYCOMB") == 0) {
            Sistema::estado.modo = ModoBobinado::NIDO_ABEJA;
            Serial.println("Modo NIDO DE ABEJA seleccionado");
            lv_scr_load_anim(UIScreens::screen_config_honeycomb, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
        }
    }
    
    void btn_comando_handler(lv_event_t *e) {
        lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
        const char *id = (const char *)lv_obj_get_user_data(target);
        
        if (strcmp(id, "HOME") == 0) {
            if (Sistema::estado.estado == EstadoBobinado::LISTO || 
                Sistema::estado.estado == EstadoBobinado::PAUSADO ||
                Sistema::estado.estado == EstadoBobinado::ERROR) {
                Serial.println(">>> COMANDO: HOME MANUAL <<<");
                homing_ejes();
                if (UIScreens::label_estado) {
                    lv_label_set_text(UIScreens::label_estado, "HOME OK");
                }
            }
        }
        else if (strcmp(id, "INICIAR") == 0 && Sistema::estado.estado == EstadoBobinado::LISTO) {
            Sistema::estado.iniciar_bobinado();
        }
        else if (strcmp(id, "PROSEGUIR") == 0 && Sistema::estado.estado == EstadoBobinado::PAUSADO) {
            Sistema::estado.reanudar_bobinado();
        }
        else if (strcmp(id, "PAUSAR") == 0 && Sistema::estado.estado == EstadoBobinado::BOBINANDO) {
            Sistema::estado.pausar_bobinado();
        }
        else if (strcmp(id, "PARAR") == 0) {
            Sistema::estado.detener_bobinado();
            Sistema::estado.reset_solicitado = true;
        }
    }
    
    void btn_manual_handler(lv_event_t *e) {
        if (!Sistema::estado.puede_mover_manual()) return;
        
        lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
        const char *id = (const char *)lv_obj_get_user_data(target);
        const int delay_us = 500;
        const int pasos_X = Hardware::Motor::PASOS_POR_MM_X;
        const int pasos_Y = Hardware::Motor::PASOS_POR_VUELTA_Y;
        
        if (strcmp(id, "X+") == 0)
            move_motor_steps_safe(Hardware::Motor::STEP_X_PIN, Hardware::Motor::DIR_X_PIN, pasos_X, delay_us);
        else if (strcmp(id, "X-") == 0)
            move_motor_steps_safe(Hardware::Motor::STEP_X_PIN, Hardware::Motor::DIR_X_PIN, -pasos_X, delay_us);
        else if (strcmp(id, "Y+") == 0)
            move_motor_steps_safe(Hardware::Motor::STEP_Y_PIN, Hardware::Motor::DIR_Y_PIN, pasos_Y, delay_us);
        else if (strcmp(id, "Y-") == 0)
            move_motor_steps_safe(Hardware::Motor::STEP_Y_PIN, Hardware::Motor::DIR_Y_PIN, -pasos_Y, delay_us);
    }
    
    void ta_event_cb(lv_event_t *e) {
        if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
            lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
            UI::Numpad::show(target);
        }
    }
    
    void screen_winding_load_handler(lv_event_t *e) {
        if (lv_event_get_code(e) == LV_EVENT_SCREEN_LOADED) {
            Sistema::actualizar_configuracion();
        }
    }
    
    void back_and_save_handler(lv_event_t *e) {
        Sistema::config_transformador.diametro_alambre_mm = atof(lv_textarea_get_text(UIScreens::ta_diametro_alambre));
        Sistema::config_transformador.longitud_bobinado_mm = atof(lv_textarea_get_text(UIScreens::ta_longitud_bobinado));
        Sistema::config_transformador.vueltas_total = atoi(lv_textarea_get_text(UIScreens::ta_vueltas_total));
        Sistema::config_transformador.velocidad_rpm = atof(lv_textarea_get_text(UIScreens::ta_velocidad_rpm));
        Sistema::config_transformador.detener_en_capas = lv_obj_has_state(UIScreens::sw_detener_en_capas, LV_STATE_CHECKED);
        
        if (Sistema::config_transformador.validar()) {
            Sistema::config_transformador.calcular_parametros();
            Sistema::config_transformador.imprimir_debug();
            Persistence.saveTransformadorConfig();
        }
        
        lv_scr_load_anim(UIScreens::screen_main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    }
    
    void back_and_save_honeycomb_handler(lv_event_t *e) {
        Sistema::config_nido_abeja.diametro_hilo = atof(lv_textarea_get_text(UIScreens::ta_hc_diametro_hilo));
        Sistema::config_nido_abeja.diametro_carrete = atof(lv_textarea_get_text(UIScreens::ta_hc_diametro_carrete));
        Sistema::config_nido_abeja.ancho_carrete = atof(lv_textarea_get_text(UIScreens::ta_hc_ancho_carrete));
        Sistema::config_nido_abeja.desfase_grados = atof(lv_textarea_get_text(UIScreens::ta_hc_desfase_grados));
        Sistema::config_nido_abeja.num_vueltas = atoi(lv_textarea_get_text(UIScreens::ta_hc_num_vueltas));
        Sistema::config_nido_abeja.velocidad_rpm = atof(lv_textarea_get_text(UIScreens::ta_hc_velocidad));
        
        if (Sistema::config_nido_abeja.validar()) {
            Sistema::config_nido_abeja.calcular_parametros();
            Sistema::config_nido_abeja.imprimir_debug();
            Persistence.saveHoneycombConfig();
        }
        
        lv_scr_load_anim(UIScreens::screen_main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    }
    
    void validate_and_wind_handler(lv_event_t *e) {
        back_and_save_handler(e);
        lv_scr_load_anim(UIScreens::screen_winding, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    }
    
    void validate_and_wind_honeycomb_handler(lv_event_t *e) {
        back_and_save_honeycomb_handler(e);
        lv_scr_load_anim(UIScreens::screen_winding, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
    }
    
    // =========================================================================
    // FUNCIÓN DE ACTUALIZACIÓN DE UI - VERSIÓN OPTIMIZADA CON LECTURAS ATÓMICAS
    // =========================================================================
    void update_winding_screen() {
        // Verificar que estamos en la pantalla correcta
        if (lv_scr_act() != UIScreens::screen_winding) return;

        // =====================================================================
        // PASO 1: HACER COPIA LOCAL DE TODAS LAS VARIABLES VOLATILE
        // =====================================================================
        // CRÍTICO: Leer cada variable volatile UNA SOLA VEZ para evitar
        // condiciones de carrera y lecturas inconsistentes entre cores
        
        uint32_t vueltas_actual_local = Sistema::estado.vueltas_completadas;
        uint32_t vueltas_capa_local = Sistema::estado.vueltas_capa_actual;
        uint32_t capas_actual_local = Sistema::estado.capas_completadas;
        float rpm_actual_local = Sistema::estado.rpm_actual;
        EstadoBobinado estado_local = Sistema::estado.estado;
        ModoBobinado modo_local = Sistema::estado.modo;
        
        // =====================================================================
        // PASO 2: ACTUALIZAR ESTADO Y COLOR
        // =====================================================================
        const char *estado_str;
        lv_color_t estado_color;
        
        switch (estado_local) {
            case EstadoBobinado::BOBINANDO:
                estado_str = "BOBINANDO"; 
                estado_color = UI::color_success; 
                break;
            case EstadoBobinado::PAUSADO:
                estado_str = "PAUSADO"; 
                estado_color = UI::color_warning; 
                break;
            case EstadoBobinado::ERROR:
                estado_str = "ERROR"; 
                estado_color = UI::color_danger; 
                break;
            case EstadoBobinado::HOMING:
                estado_str = "HOMING"; 
                estado_color = UI::color_primary; 
                break;
            default:
                estado_str = "LISTO"; 
                estado_color = UI::color_secondary; 
                break;
        }
        
        if (UIScreens::label_estado) {
            lv_label_set_text(UIScreens::label_estado, estado_str);
            lv_obj_set_style_text_color(UIScreens::label_estado, estado_color, 0);
        }

        // =====================================================================
        // PASO 3: ACTUALIZAR MODO ACTUAL
        // =====================================================================
        if (UIScreens::label_modo_actual) {
            if (modo_local == ModoBobinado::TRANSFORMADOR) {
                lv_label_set_text(UIScreens::label_modo_actual, LV_SYMBOL_LOOP " Transformador");
                lv_obj_set_style_text_color(UIScreens::label_modo_actual, UI::color_primary, 0);
            } else {
                lv_label_set_text(UIScreens::label_modo_actual, LV_SYMBOL_IMAGE " Nido Abeja");
                lv_obj_set_style_text_color(UIScreens::label_modo_actual, UI::color_honeycomb, 0);
            }
        }

        // =====================================================================
        // PASO 4: OBTENER VALORES OBJETIVO (NO SON VOLATILE)
        // =====================================================================
        uint32_t vueltas_objetivo = (modo_local == ModoBobinado::TRANSFORMADOR) ?
                                    Sistema::config_transformador.vueltas_total :
                                    Sistema::config_nido_abeja.num_vueltas;
        
        uint32_t capas_totales = (modo_local == ModoBobinado::TRANSFORMADOR) ?
                                 Sistema::config_transformador.capas_estimadas :
                                 Sistema::config_nido_abeja.capas_estimadas;
        
        uint32_t vueltas_por_capa = (modo_local == ModoBobinado::TRANSFORMADOR) ?
                                    Sistema::config_transformador.vueltas_por_capa :
                                    Sistema::config_nido_abeja.vueltas_por_capa;
        
        // =====================================================================
        // PASO 5: ACTUALIZAR VUELTAS (USANDO VARIABLES LOCALES)
        // =====================================================================
        if (UIScreens::label_vueltas_actuales) {
            lv_label_set_text_fmt(UIScreens::label_vueltas_actuales, "%lu", vueltas_actual_local);
        }
        
        if (UIScreens::label_vueltas_totales) {
            lv_label_set_text_fmt(UIScreens::label_vueltas_totales, "/ %lu", vueltas_objetivo);
        }
        
        // =====================================================================
        // PASO 6: ACTUALIZAR BARRA DE PROGRESO
        // =====================================================================
        if (UIScreens::bar_progreso) {
            int32_t progreso = 0;
            if (vueltas_objetivo > 0) {
                progreso = (int32_t)((vueltas_actual_local * 100) / vueltas_objetivo);
                if (progreso > 100) progreso = 100; // Limitar a 100%
            }
            lv_bar_set_value(UIScreens::bar_progreso, progreso, LV_ANIM_OFF); // Sin animación para mejor rendimiento
        }
        
        if (UIScreens::label_progreso) {
            int32_t progreso = (vueltas_objetivo > 0) ? 
                              (int32_t)((vueltas_actual_local * 100) / vueltas_objetivo) : 0;
            if (progreso > 100) progreso = 100;
            lv_label_set_text_fmt(UIScreens::label_progreso, "%ld%%", progreso);
        }
        
        // =====================================================================
        // PASO 7: ACTUALIZAR INFORMACIÓN DE CAPAS SEGÚN MODO
        // =====================================================================
        if (UIScreens::label_capa_info) {
            if (modo_local == ModoBobinado::TRANSFORMADOR) {
                // Calcular número de capa actual (1-indexed)
                uint32_t capa_actual_display = (capas_totales > 0 && capas_actual_local < capas_totales) ? 
                                              capas_actual_local + 1 : capas_totales;
                
                float grosor_actual_mm = capas_actual_local * Sistema::config_transformador.diametro_alambre_mm;
                
                lv_label_set_text_fmt(UIScreens::label_capa_info, 
                                     "Capa: %lu/%lu | Vueltas: %lu/%lu | Grosor: %.2f mm",
                                     capa_actual_display, 
                                     capas_totales,
                                     vueltas_capa_local,
                                     vueltas_por_capa,
                                     grosor_actual_mm);
            } else {
                // Modo nido de abeja
                float grosor_actual = 0.0f;
                if (vueltas_por_capa > 0) {
                    float capas_actuales_float = (float)vueltas_actual_local / (float)vueltas_por_capa;
                    grosor_actual = capas_actuales_float * Sistema::config_nido_abeja.diametro_hilo;
                }
                
                lv_label_set_text_fmt(UIScreens::label_capa_info,
                                     "Grosor: %.2f mm | Desfase: %.1f° | Ancho: %.1f mm",
                                     grosor_actual,
                                     Sistema::config_nido_abeja.desfase_grados,
                                     Sistema::config_nido_abeja.ancho_carrete);
            }
        }
        
        // =====================================================================
        // PASO 8: ACTUALIZAR VELOCIDAD ACTUAL
        // =====================================================================
        if (UIScreens::label_velocidad_actual) {
            lv_label_set_text_fmt(UIScreens::label_velocidad_actual, 
                                 LV_SYMBOL_CHARGE " %.0f RPM", 
                                 rpm_actual_local);
        }

        // =====================================================================
        // PASO 9: ACTUALIZAR TIEMPO RESTANTE ESTIMADO
        // =====================================================================
        if (UIScreens::label_tiempo_restante) {
            if (estado_local == EstadoBobinado::BOBINANDO && rpm_actual_local > 1.0f) {
                if (vueltas_actual_local < vueltas_objetivo) {
                    uint32_t vueltas_restantes = vueltas_objetivo - vueltas_actual_local;
                    float minutos_float = (float)vueltas_restantes / rpm_actual_local;
                    int total_seconds = (int)(minutos_float * 60.0f);
                    
                    int horas = total_seconds / 3600;
                    int min = (total_seconds % 3600) / 60;
                    int sec = total_seconds % 60;
                    
                    if (horas > 0) {
                        lv_label_set_text_fmt(UIScreens::label_tiempo_restante, 
                                             LV_SYMBOL_POWER " %02d:%02d:%02d", 
                                             horas, min, sec);
                    } else {
                        lv_label_set_text_fmt(UIScreens::label_tiempo_restante, 
                                             LV_SYMBOL_POWER " %02d:%02d", 
                                             min, sec);
                    }
                } else {
                    lv_label_set_text(UIScreens::label_tiempo_restante, LV_SYMBOL_POWER " 00:00");
                }
            } else {
                lv_label_set_text(UIScreens::label_tiempo_restante, LV_SYMBOL_POWER " --:--");
            }
        }
    }
    
    // Implementaciones stub de funciones de hardware (el archivo principal las sobrescribirá)
    __attribute__((weak)) void homing_ejes() {
        Serial.println("homing_ejes() llamada desde UI handler");
    }
    
    __attribute__((weak)) void move_motor_steps_safe(int step_pin, int dir_pin, int steps, int delay_us) {
        Serial.println("move_motor_steps_safe() llamada desde UI handler");
    }
}