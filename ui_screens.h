#ifndef UI_SCREENS_H
#define UI_SCREENS_H

#include <lvgl.h>
#include "config.h"
#include "ui_components.h"

namespace UIScreens {
    // Pantallas
    extern lv_obj_t *screen_main;
    extern lv_obj_t *screen_modo_selection;
    extern lv_obj_t *screen_config;
    extern lv_obj_t *screen_config_honeycomb;
    extern lv_obj_t *screen_winding;
    extern lv_obj_t *screen_manual_control;
    
    // Campos de configuración TRANSFORMADOR
    extern lv_obj_t *ta_diametro_alambre;
    extern lv_obj_t *ta_longitud_bobinado;
    extern lv_obj_t *ta_vueltas_total;
    extern lv_obj_t *ta_velocidad_rpm;
    extern lv_obj_t *sw_detener_en_capas;
    
    // Campos de configuración NIDO DE ABEJA
    extern lv_obj_t *ta_hc_diametro_hilo;
    extern lv_obj_t *ta_hc_diametro_carrete;
    extern lv_obj_t *ta_hc_ancho_carrete;
    extern lv_obj_t *ta_hc_desfase_grados;
    extern lv_obj_t *ta_hc_num_vueltas;
    extern lv_obj_t *ta_hc_velocidad;
    
    // Labels de pantalla de bobinado
    extern lv_obj_t *label_estado;
    extern lv_obj_t *label_modo_actual;
    extern lv_obj_t *label_vueltas_actuales;
    extern lv_obj_t *label_vueltas_totales;
    extern lv_obj_t *label_progreso;
    extern lv_obj_t *bar_progreso;
    extern lv_obj_t *label_capa_info;
    extern lv_obj_t *label_velocidad_actual;
    extern lv_obj_t *label_tiempo_restante;
    
    // Funciones de creación de pantallas
    void crear_pantalla_principal();
    void crear_pantalla_seleccion_modo();
    void crear_pantalla_configuracion();
    void crear_pantalla_configuracion_honeycomb();
    void crear_pantalla_bobinado();
    void crear_pantalla_control_manual();
    
    void init_all_screens();
}

#endif // UI_SCREENS_H