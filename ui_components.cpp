#include "ui_components.h"

namespace UI {
    // Definición de colores
    lv_color_t color_primary;
    lv_color_t color_secondary;
    lv_color_t color_success;
    lv_color_t color_warning;
    lv_color_t color_danger;
    lv_color_t color_honeycomb;
    lv_color_t color_bg_dark;
    lv_color_t color_bg_card;
    lv_color_t color_text;
    lv_color_t color_text_secondary;
    
    // Definición de estilos
    lv_style_t style_main_bg;
    lv_style_t style_card;
    lv_style_t style_header;
    lv_style_t style_btn_primary;
    lv_style_t style_btn_success;
    lv_style_t style_btn_warning;
    lv_style_t style_btn_danger;
    lv_style_t style_btn_honeycomb;
    lv_style_t style_ta;
    lv_style_t style_label_value;
    lv_style_t style_text_secondary;
    lv_style_t style_numpad_btn;
    lv_style_t style_numpad_display;
    lv_style_t style_numpad_btn_action;
    
    // Variables estáticas del Numpad
    lv_obj_t* Numpad::overlay = nullptr;
    lv_obj_t* Numpad::container = nullptr;
    lv_obj_t* Numpad::current_textarea = nullptr;
    lv_obj_t* Numpad::display_label = nullptr;
    lv_obj_t* Numpad::title_label = nullptr;
    
    void init_styles() {
        // Colores
        color_primary = lv_color_hex(0x2196F3);
        color_secondary = lv_color_hex(0x90CAF9);
        color_success = lv_color_hex(0x4CAF50);
        color_warning = lv_color_hex(0xFFA726);
        color_danger = lv_color_hex(0xEF5350);
        color_honeycomb = lv_color_hex(0xFFD700);
        color_bg_dark = lv_color_hex(0x000000);
        color_bg_card = lv_color_hex(0x1A1A1A);
        color_text = lv_color_hex(0xFFFFFF);
        color_text_secondary = lv_color_hex(0xB0B0B0);

        // Estilo de fondo principal
        lv_style_init(&style_main_bg);
        lv_style_set_bg_color(&style_main_bg, color_bg_dark);

        // Estilo de tarjeta
        lv_style_init(&style_card);
        lv_style_set_bg_color(&style_card, color_bg_card);
        lv_style_set_radius(&style_card, 12);
        lv_style_set_pad_all(&style_card, 10);
        lv_style_set_border_width(&style_card, 1);
        lv_style_set_border_color(&style_card, lv_color_hex(0x333333));

        // Estilo header
        lv_style_init(&style_header);
        lv_style_set_text_font(&style_header, &lv_font_montserrat_20);
        lv_style_set_text_color(&style_header, color_text);

        // Estilos de botones
        lv_style_init(&style_btn_primary);
        lv_style_set_bg_color(&style_btn_primary, color_primary);
        lv_style_set_radius(&style_btn_primary, 10);
        lv_style_set_text_color(&style_btn_primary, color_text);
        lv_style_set_text_font(&style_btn_primary, &lv_font_montserrat_16);

        lv_style_init(&style_btn_success);
        lv_style_set_bg_color(&style_btn_success, color_success);
        lv_style_set_radius(&style_btn_success, 10);
        lv_style_set_text_color(&style_btn_success, color_text);
        lv_style_set_text_font(&style_btn_success, &lv_font_montserrat_16);

        lv_style_init(&style_btn_warning);
        lv_style_set_bg_color(&style_btn_warning, color_warning);
        lv_style_set_radius(&style_btn_warning, 10);
        lv_style_set_text_color(&style_btn_warning, color_bg_dark);
        lv_style_set_text_font(&style_btn_warning, &lv_font_montserrat_16);

        lv_style_init(&style_btn_danger);
        lv_style_set_bg_color(&style_btn_danger, color_danger);
        lv_style_set_radius(&style_btn_danger, 10);
        lv_style_set_text_color(&style_btn_danger, color_text);
        lv_style_set_text_font(&style_btn_danger, &lv_font_montserrat_16);

        lv_style_init(&style_btn_honeycomb);
        lv_style_set_bg_color(&style_btn_honeycomb, color_honeycomb);
        lv_style_set_radius(&style_btn_honeycomb, 10);
        lv_style_set_text_color(&style_btn_honeycomb, color_bg_dark);
        lv_style_set_text_font(&style_btn_honeycomb, &lv_font_montserrat_16);

        // Estilo textarea
        lv_style_init(&style_ta);
        lv_style_set_bg_color(&style_ta, lv_color_hex(0x2A2A2A));
        lv_style_set_border_color(&style_ta, color_primary);
        lv_style_set_border_width(&style_ta, 2);
        lv_style_set_radius(&style_ta, 5);
        lv_style_set_pad_all(&style_ta, 5);
        lv_style_set_text_color(&style_ta, color_text);
        lv_style_set_text_font(&style_ta, &lv_font_montserrat_14);

        // Otros estilos
        lv_style_init(&style_label_value);
        lv_style_set_text_font(&style_label_value, &lv_font_montserrat_36);
        lv_style_set_text_color(&style_label_value, color_primary);

        lv_style_init(&style_text_secondary);
        lv_style_set_text_color(&style_text_secondary, color_text_secondary);
        lv_style_set_text_font(&style_text_secondary, &lv_font_montserrat_14);
        
        // Estilos numpad
        lv_style_init(&style_numpad_display);
        lv_style_set_bg_color(&style_numpad_display, lv_color_hex(0x1E1E1E));
        lv_style_set_border_color(&style_numpad_display, color_primary);
        lv_style_set_border_width(&style_numpad_display, 2);
        lv_style_set_radius(&style_numpad_display, 8);
        lv_style_set_pad_all(&style_numpad_display, 6);

        lv_style_init(&style_numpad_btn);
        lv_style_set_bg_color(&style_numpad_btn, lv_color_hex(0x2D2D2D));
        lv_style_set_radius(&style_numpad_btn, 10);
        lv_style_set_text_color(&style_numpad_btn, color_text);
        lv_style_set_text_font(&style_numpad_btn, &lv_font_montserrat_28);
        
        lv_style_init(&style_numpad_btn_action);
        lv_style_set_bg_color(&style_numpad_btn_action, lv_color_hex(0x3D3D3D));
        lv_style_set_radius(&style_numpad_btn_action, 10);
        lv_style_set_text_color(&style_numpad_btn_action, color_text);
        lv_style_set_text_font(&style_numpad_btn_action, &lv_font_montserrat_20);
    }
    
    // Implementación del Numpad
    void Numpad::create() {
        overlay = lv_obj_create(lv_layer_top());
        lv_obj_set_size(overlay, Hardware::SCREEN_WIDTH, Hardware::SCREEN_HEIGHT);
        lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(overlay, LV_OPA_70, 0);
        lv_obj_set_style_border_width(overlay, 0, 0);
        lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);

        container = lv_obj_create(lv_layer_top());
        lv_obj_set_size(container, 320, 260);
        lv_obj_center(container);
        lv_obj_add_style(container, &style_card, 0);
        lv_obj_set_layout(container, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_gap(container, 5, 0);
        lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);

        title_label = lv_label_create(container);
        lv_label_set_text(title_label, "Editar Parámetro");
        lv_obj_add_style(title_label, &style_text_secondary, 0);
        lv_obj_set_width(title_label, LV_PCT(100));
        lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t *display_cont = lv_obj_create(container);
        lv_obj_set_width(display_cont, LV_PCT(100));
        lv_obj_set_height(display_cont, LV_SIZE_CONTENT);
        lv_obj_add_style(display_cont, &style_numpad_display, 0);
        display_label = lv_label_create(display_cont);
        lv_label_set_text(display_label, "0");
        lv_obj_set_style_text_font(display_label, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(display_label, color_primary, 0);
        lv_obj_center(display_label);

        lv_obj_t *btn_panel = lv_obj_create(container);
        lv_obj_set_width(btn_panel, LV_PCT(100));
        lv_obj_set_flex_grow(btn_panel, 1);
        lv_obj_set_style_bg_opa(btn_panel, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(btn_panel, 0, 0);
        lv_obj_set_layout(btn_panel, LV_LAYOUT_GRID);
        
        static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(btn_panel, col_dsc, row_dsc);
        lv_obj_set_style_pad_column(btn_panel, 5, 0);
        lv_obj_set_style_pad_row(btn_panel, 5, 0);

        struct {
            const char *label;
            const char *id;
            int col;
            int row;
            bool is_action;
            lv_color_t color;
        } buttons[] = {
            {"7", "7", 0, 0, false}, {"8", "8", 1, 0, false}, {"9", "9", 2, 0, false}, 
            {LV_SYMBOL_BACKSPACE, "DEL", 3, 0, true, color_warning},
            {"4", "4", 0, 1, false}, {"5", "5", 1, 1, false}, {"6", "6", 2, 1, false}, 
            {LV_SYMBOL_TRASH, "CLR", 3, 1, true, color_danger},
            {"1", "1", 0, 2, false}, {"2", "2", 1, 2, false}, {"3", "3", 2, 2, false}, 
            {LV_SYMBOL_OK, "OK", 3, 2, true, color_success},
            {"0", "0", 0, 3, false}, {"00", "00", 1, 3, false}, {".", ".", 2, 3, false}, 
            {LV_SYMBOL_CLOSE, "ESC", 3, 3, true, lv_color_hex(0x555555)},
        };

        for (int i = 0; i < 16; i++) {
            lv_obj_t *btn = lv_btn_create(btn_panel);
            lv_obj_set_user_data(btn, (void *)buttons[i].id);
            if (buttons[i].is_action) {
                lv_obj_add_style(btn, &style_numpad_btn_action, 0);
                lv_obj_set_style_bg_color(btn, buttons[i].color, 0);
            } else {
                lv_obj_add_style(btn, &style_numpad_btn, 0);
            }
            lv_obj_add_event_cb(btn, btn_handler, LV_EVENT_CLICKED, NULL);
            lv_obj_t *label = lv_label_create(btn);
            lv_label_set_text(label, buttons[i].label);
            lv_obj_center(label);
            lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, buttons[i].col, 1, 
                                LV_GRID_ALIGN_STRETCH, buttons[i].row, 1);
        }
    }
    
    void Numpad::show(lv_obj_t *ta) {
        if (container && ta) {
            current_textarea = ta;
            const char *current_value = lv_textarea_get_text(ta);
            lv_label_set_text(display_label, strlen(current_value) == 0 ? "0" : current_value);
            
            const char* title_text = (const char*)lv_obj_get_user_data(ta);
            if (title_text) {
                char buffer[64];
                snprintf(buffer, sizeof(buffer), "Editar %s", title_text);
                lv_label_set_text(title_label, buffer);
            }
            
            lv_obj_clear_flag(overlay, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    void Numpad::hide() {
        if (container) {
            lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
            current_textarea = nullptr;
        }
    }
    
    void Numpad::btn_handler(lv_event_t *e) {
        lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
        const char *id = (const char *)lv_obj_get_user_data(target);
        if (!current_textarea || !display_label) return;
        
        const char *current = lv_label_get_text(display_label);
        char new_text[32];
        
        if (strcmp(id, "OK") == 0) {
            lv_textarea_set_text(current_textarea, current);
            hide();
        } else if (strcmp(id, "ESC") == 0) {
            hide();
        } else if (strcmp(id, "CLR") == 0) {
            lv_label_set_text(display_label, "0");
        } else if (strcmp(id, "DEL") == 0) {
            size_t len = strlen(current);
            if (len > 0) {
                strncpy(new_text, current, len - 1);
                new_text[len - 1] = '\0';
                lv_label_set_text(display_label, strlen(new_text) == 0 ? "0" : new_text);
            }
        } else {
            if (strcmp(current, "0") == 0 && strcmp(id, ".") != 0) {
                lv_label_set_text(display_label, id);
            } else {
                if (strcmp(id, ".") == 0 && strchr(current, '.') != NULL) return;
                if (strlen(current) < 15) {
                    snprintf(new_text, sizeof(new_text), "%s%s", current, id);
                    lv_label_set_text(display_label, new_text);
                }
            }
        }
    }
    
    // Funciones helper
    lv_obj_t* create_header(lv_obj_t* parent, const char* title, 
                            void (*back_cb)(lv_event_t*),
                            void (*action_cb)(lv_event_t*),
                            const char* action_text) {
        lv_obj_t *header = lv_obj_create(parent);
        lv_obj_set_width(header, LV_PCT(100));
        lv_obj_set_height(header, LV_SIZE_CONTENT);
        lv_obj_add_style(header, &style_card, 0);
        lv_obj_set_style_pad_all(header, 8, 0);
        lv_obj_set_layout(header, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        if (back_cb) {
            lv_obj_t *btn_back = lv_btn_create(header);
            lv_obj_set_size(btn_back, 45, 45);
            lv_obj_add_style(btn_back, &style_btn_primary, 0);
            lv_obj_add_event_cb(btn_back, back_cb, LV_EVENT_CLICKED, NULL);
            lv_obj_t *label_back = lv_label_create(btn_back);
            lv_label_set_text(label_back, LV_SYMBOL_LEFT);
            lv_obj_center(label_back);
        }
        
        lv_obj_t *header_label = lv_label_create(header);
        lv_label_set_text(header_label, title);
        lv_obj_add_style(header_label, &style_header, 0);
        lv_obj_set_flex_grow(header_label, 1);
        lv_obj_set_style_text_align(header_label, LV_TEXT_ALIGN_CENTER, 0);
        
        if (action_cb && action_text) {
            lv_obj_t *btn_action = lv_btn_create(header);
            lv_obj_set_size(btn_action, 110, 45);
            lv_obj_add_style(btn_action, &style_btn_success, 0);
            lv_obj_add_event_cb(btn_action, action_cb, LV_EVENT_CLICKED, NULL);
            lv_obj_t *label_action = lv_label_create(btn_action);
            lv_label_set_text(label_action, action_text);
            lv_obj_center(label_action);
        }
        
        return header;
    }
    
    lv_obj_t* create_card(lv_obj_t* parent) {
        lv_obj_t *card = lv_obj_create(parent);
        lv_obj_set_width(card, LV_PCT(100));
        lv_obj_add_style(card, &style_card, 0);
        return card;
    }
    
    lv_obj_t* create_button(lv_obj_t* parent, const char* text, 
                            const char* id, lv_style_t* style,
                            void (*click_cb)(lv_event_t*),
                            int width, int height) {
        lv_obj_t *btn = lv_btn_create(parent);
        lv_obj_set_size(btn, width, height);
        lv_obj_add_style(btn, style, 0);
        lv_obj_set_user_data(btn, (void*)id);
        lv_obj_add_event_cb(btn, click_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, text);
        lv_obj_center(label);
        return btn;
    }
}