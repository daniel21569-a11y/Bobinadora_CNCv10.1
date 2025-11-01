#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <lvgl.h>
#include "config.h"

namespace UI {
    // Colores globales
    extern lv_color_t color_primary;
    extern lv_color_t color_secondary;
    extern lv_color_t color_success;
    extern lv_color_t color_warning;
    extern lv_color_t color_danger;
    extern lv_color_t color_honeycomb;
    extern lv_color_t color_bg_dark;
    extern lv_color_t color_bg_card;
    extern lv_color_t color_text;
    extern lv_color_t color_text_secondary;
    
    // Estilos globales
    extern lv_style_t style_main_bg;
    extern lv_style_t style_card;
    extern lv_style_t style_header;
    extern lv_style_t style_btn_primary;
    extern lv_style_t style_btn_success;
    extern lv_style_t style_btn_warning;
    extern lv_style_t style_btn_danger;
    extern lv_style_t style_btn_honeycomb;
    extern lv_style_t style_ta;
    extern lv_style_t style_label_value;
    extern lv_style_t style_text_secondary;
    extern lv_style_t style_numpad_btn;
    extern lv_style_t style_numpad_display;
    extern lv_style_t style_numpad_btn_action;
    
    class Numpad {
    private:
        static lv_obj_t *overlay;
        static lv_obj_t *container;
        static lv_obj_t *current_textarea;
        static lv_obj_t *display_label;
        static lv_obj_t *title_label;
        
    public:
        static void create();
        static void show(lv_obj_t *textarea);
        static void hide();
        static void btn_handler(lv_event_t *e);
        static lv_obj_t* getCurrentTextarea() { return current_textarea; }
    };
    
    void init_styles();
    lv_obj_t* create_header(lv_obj_t* parent, const char* title, 
                            void (*back_cb)(lv_event_t*) = nullptr,
                            void (*action_cb)(lv_event_t*) = nullptr,
                            const char* action_text = nullptr);
    lv_obj_t* create_card(lv_obj_t* parent);
    lv_obj_t* create_button(lv_obj_t* parent, const char* text, 
                            const char* id, lv_style_t* style,
                            void (*click_cb)(lv_event_t*),
                            int width = 100, int height = 45);
}

#endif // UI_COMPONENTS_H