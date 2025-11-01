#ifndef UI_HANDLERS_H
#define UI_HANDLERS_H

#include <lvgl.h>

namespace UIHandlers {
    // Handlers de navegación
    void btn_navegacion_handler(lv_event_t *e);
    void btn_modo_handler(lv_event_t *e);
    
    // Handlers de comandos
    void btn_comando_handler(lv_event_t *e);
    void btn_manual_handler(lv_event_t *e);
    
    // Handlers de configuración
    void back_and_save_handler(lv_event_t *e);
    void back_and_save_honeycomb_handler(lv_event_t *e);
    void validate_and_wind_handler(lv_event_t *e);
    void validate_and_wind_honeycomb_handler(lv_event_t *e);
    
    // Handlers de eventos
    void ta_event_cb(lv_event_t *e);
    void screen_winding_load_handler(lv_event_t *e);
    
    // Actualización de UI
    void update_winding_screen();
    
    // Función auxiliar para homing (declaración)
    void homing_ejes();
    void move_motor_steps_safe(int step_pin, int dir_pin, int steps, int delay_us);
}

#endif // UI_HANDLERS_H