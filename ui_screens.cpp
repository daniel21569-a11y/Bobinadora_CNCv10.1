#include "ui_screens.h"
#include "ui_handlers.h"
#include "persistence.h"

namespace UIScreens {
    // Definición de pantallas
    lv_obj_t *screen_main = nullptr;
    lv_obj_t *screen_modo_selection = nullptr;
    lv_obj_t *screen_config = nullptr;
    lv_obj_t *screen_config_honeycomb = nullptr;
    lv_obj_t *screen_winding = nullptr;
    lv_obj_t *screen_manual_control = nullptr;
    
    // Campos de configuración TRANSFORMADOR
    lv_obj_t *ta_diametro_alambre = nullptr;
    lv_obj_t *ta_longitud_bobinado = nullptr;
    lv_obj_t *ta_vueltas_total = nullptr;
    lv_obj_t *ta_velocidad_rpm = nullptr;
    lv_obj_t *sw_detener_en_capas = nullptr;
    
    // Campos de configuración NIDO DE ABEJA
    lv_obj_t *ta_hc_diametro_hilo = nullptr;
    lv_obj_t *ta_hc_diametro_carrete = nullptr;
    lv_obj_t *ta_hc_ancho_carrete = nullptr;
    lv_obj_t *ta_hc_desfase_grados = nullptr;
    lv_obj_t *ta_hc_num_vueltas = nullptr;
    lv_obj_t *ta_hc_velocidad = nullptr;
    
    // Labels de pantalla de bobinado
    lv_obj_t *label_estado = nullptr;
    lv_obj_t *label_modo_actual = nullptr;
    lv_obj_t *label_vueltas_actuales = nullptr;
    lv_obj_t *label_vueltas_totales = nullptr;
    lv_obj_t *label_progreso = nullptr;
    lv_obj_t *bar_progreso = nullptr;
    lv_obj_t *label_capa_info = nullptr;
    lv_obj_t *label_velocidad_actual = nullptr;
    lv_obj_t *label_tiempo_restante = nullptr;
    
    void crear_pantalla_principal() {
        screen_main = lv_obj_create(NULL);
        lv_obj_add_style(screen_main, &UI::style_main_bg, 0);
        lv_obj_set_layout(screen_main, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(screen_main, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(screen_main, 5, 0);
        lv_obj_set_style_pad_row(screen_main, 3, 0);
        lv_obj_clear_flag(screen_main, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *header_cont = lv_obj_create(screen_main);
        lv_obj_set_width(header_cont, LV_PCT(100));
        lv_obj_set_height(header_cont, LV_SIZE_CONTENT);
        lv_obj_add_style(header_cont, &UI::style_card, 0);
        lv_obj_set_style_pad_ver(header_cont, 5, 0);
        lv_obj_t *header_label = lv_label_create(header_cont);
        lv_label_set_text(header_label, LV_SYMBOL_SETTINGS " BOBINADORA CNC v10.1");
        lv_obj_add_style(header_label, &UI::style_header, 0);
        lv_obj_center(header_label);

        lv_obj_t *main_cont = lv_obj_create(screen_main);
        lv_obj_set_width(main_cont, LV_PCT(100));
        lv_obj_set_flex_grow(main_cont, 1);
        lv_obj_set_layout(main_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_opa(main_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(main_cont, 0, 0);

        lv_obj_t *btn_config = lv_btn_create(main_cont);
        lv_obj_set_size(btn_config, 150, 90);
        lv_obj_add_style(btn_config, &UI::style_btn_primary, 0);
        lv_obj_set_user_data(btn_config, (void *)"SELECCION_MODO");
        lv_obj_add_event_cb(btn_config, UIHandlers::btn_navegacion_handler, LV_EVENT_CLICKED, NULL);
        lv_obj_set_flex_flow(btn_config, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(btn_config, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(btn_config, 8, 0);
        lv_obj_t *config_icon = lv_label_create(btn_config);
        lv_label_set_text(config_icon, LV_SYMBOL_SETTINGS);
        lv_obj_set_style_text_font(config_icon, &lv_font_montserrat_24, 0);
        lv_obj_t *config_label = lv_label_create(btn_config);
        lv_label_set_text(config_label, "CONFIGURAR");
        lv_obj_set_style_text_font(config_label, &lv_font_montserrat_14, 0);

        lv_obj_t *btn_bobinado = lv_btn_create(main_cont);
        lv_obj_set_size(btn_bobinado, 150, 90);
        lv_obj_add_style(btn_bobinado, &UI::style_btn_success, 0);
        lv_obj_set_user_data(btn_bobinado, (void *)"BOBINADO");
        lv_obj_add_event_cb(btn_bobinado, UIHandlers::btn_navegacion_handler, LV_EVENT_CLICKED, NULL);
        lv_obj_set_flex_flow(btn_bobinado, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(btn_bobinado, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(btn_bobinado, 8, 0);
        lv_obj_t *bobinado_icon = lv_label_create(btn_bobinado);
        lv_label_set_text(bobinado_icon, LV_SYMBOL_PLAY);
        lv_obj_set_style_text_font(bobinado_icon, &lv_font_montserrat_24, 0);
        lv_obj_t *bobinado_label = lv_label_create(btn_bobinado);
        lv_label_set_text(bobinado_label, "BOBINAR");
        lv_obj_set_style_text_font(bobinado_label, &lv_font_montserrat_14, 0);

        lv_obj_t *footer_cont = lv_obj_create(screen_main);
        lv_obj_set_width(footer_cont, LV_PCT(100));
        lv_obj_set_height(footer_cont, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(footer_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(footer_cont, 0, 0);
        lv_obj_set_layout(footer_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(footer_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(footer_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        lv_obj_t *btn_manual = lv_btn_create(footer_cont);
        lv_obj_set_size(btn_manual, 150, 45);
        lv_obj_add_style(btn_manual, &UI::style_btn_warning, 0);
        lv_obj_set_user_data(btn_manual, (void *)"MANUAL");
        lv_obj_add_event_cb(btn_manual, UIHandlers::btn_navegacion_handler, LV_EVENT_CLICKED, NULL);
        lv_obj_t *label_manual = lv_label_create(btn_manual);
        lv_label_set_text(label_manual, LV_SYMBOL_GPS " Manual");
        lv_obj_center(label_manual);

        lv_obj_t *btn_home = lv_btn_create(footer_cont);
        lv_obj_set_size(btn_home, 150, 45);
        lv_obj_add_style(btn_home, &UI::style_btn_warning, 0);
        lv_obj_set_user_data(btn_home, (void *)"HOME");
        lv_obj_add_event_cb(btn_home, UIHandlers::btn_comando_handler, LV_EVENT_CLICKED, NULL);
        lv_obj_t *label_home = lv_label_create(btn_home);
        lv_label_set_text(label_home, LV_SYMBOL_HOME " HOME");
        lv_obj_center(label_home);
    }
    
    void crear_pantalla_seleccion_modo() {
        screen_modo_selection = lv_obj_create(NULL);
        lv_obj_add_style(screen_modo_selection, &UI::style_main_bg, 0);
        lv_obj_set_layout(screen_modo_selection, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(screen_modo_selection, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(screen_modo_selection, 5, 0);
        lv_obj_set_style_pad_row(screen_modo_selection, 5, 0);
        lv_obj_clear_flag(screen_modo_selection, LV_OBJ_FLAG_SCROLLABLE);

        UI::create_header(screen_modo_selection, "  SELECCIONE MODO", 
                         UIHandlers::btn_navegacion_handler);

        lv_obj_t *modes_cont = lv_obj_create(screen_modo_selection);
        lv_obj_set_width(modes_cont, LV_PCT(100));
        lv_obj_set_flex_grow(modes_cont, 1);
        lv_obj_set_layout(modes_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(modes_cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(modes_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_opa(modes_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(modes_cont, 0, 0);

        lv_obj_t *btn_transformador = lv_btn_create(modes_cont);
        lv_obj_set_size(btn_transformador, 340, 70);
        lv_obj_add_style(btn_transformador, &UI::style_btn_primary, 0);
        lv_obj_set_user_data(btn_transformador, (void *)"MODO_TRANSFORMADOR");
        lv_obj_add_event_cb(btn_transformador, UIHandlers::btn_modo_handler, LV_EVENT_CLICKED, NULL);
        
        lv_obj_t *trans_cont = lv_obj_create(btn_transformador);
        lv_obj_set_size(trans_cont, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_opa(trans_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(trans_cont, 0, 0);
        lv_obj_set_style_pad_all(trans_cont, 0, 0);
        lv_obj_add_flag(trans_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_clear_flag(trans_cont, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_layout(trans_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(trans_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(trans_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_left(trans_cont, 15, 0);
        lv_obj_set_style_pad_gap(trans_cont, 12, 0);
        
        lv_obj_t *trans_icon = lv_label_create(trans_cont);
        lv_label_set_text(trans_icon, LV_SYMBOL_LOOP);
        lv_obj_set_style_text_font(trans_icon, &lv_font_montserrat_28, 0);
        lv_obj_add_flag(trans_icon, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_clear_flag(trans_icon, LV_OBJ_FLAG_CLICKABLE);
        
        lv_obj_t *trans_text_cont = lv_obj_create(trans_cont);
        lv_obj_set_size(trans_text_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(trans_text_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(trans_text_cont, 0, 0);
        lv_obj_set_style_pad_all(trans_text_cont, 0, 0);
        lv_obj_add_flag(trans_text_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_clear_flag(trans_text_cont, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_flex_flow(trans_text_cont, LV_FLEX_FLOW_COLUMN);
        
        lv_obj_t *trans_title = lv_label_create(trans_text_cont);
        lv_label_set_text(trans_title, "TRANSFORMADOR");
        lv_obj_set_style_text_font(trans_title, &lv_font_montserrat_16, 0);
        
        lv_obj_t *trans_desc = lv_label_create(trans_text_cont);
        lv_label_set_text(trans_desc, "Bobinado tradicional capa por capa");
        lv_obj_set_style_text_font(trans_desc, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(trans_desc, lv_color_hex(0x000000), 0);

        lv_obj_t *btn_honeycomb = lv_btn_create(modes_cont);
        lv_obj_set_size(btn_honeycomb, 340, 70);
        lv_obj_add_style(btn_honeycomb, &UI::style_btn_honeycomb, 0);
        lv_obj_set_user_data(btn_honeycomb, (void *)"MODO_HONEYCOMB");
        lv_obj_add_event_cb(btn_honeycomb, UIHandlers::btn_modo_handler, LV_EVENT_CLICKED, NULL);
        
        lv_obj_t *hc_cont = lv_obj_create(btn_honeycomb);
        lv_obj_set_size(hc_cont, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_opa(hc_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(hc_cont, 0, 0);
        lv_obj_set_style_pad_all(hc_cont, 0, 0);
        lv_obj_add_flag(hc_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_clear_flag(hc_cont, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_layout(hc_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(hc_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(hc_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_left(hc_cont, 15, 0);
        lv_obj_set_style_pad_gap(hc_cont, 12, 0);
        
        lv_obj_t *hc_icon = lv_label_create(hc_cont);
        lv_label_set_text(hc_icon, LV_SYMBOL_IMAGE);
        lv_obj_set_style_text_font(hc_icon, &lv_font_montserrat_28, 0);
        
        lv_obj_t *hc_text_cont = lv_obj_create(hc_cont);
        lv_obj_set_size(hc_text_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(hc_text_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(hc_text_cont, 0, 0);
        lv_obj_set_style_pad_all(hc_text_cont, 0, 0);
        lv_obj_add_flag(hc_text_cont, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_clear_flag(hc_text_cont, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_flex_flow(hc_text_cont, LV_FLEX_FLOW_COLUMN);
        
        lv_obj_t *hc_title = lv_label_create(hc_text_cont);
        lv_label_set_text(hc_title, "NIDO DE ABEJA");
        lv_obj_set_style_text_font(hc_title, &lv_font_montserrat_16, 0);
        
        lv_obj_t *hc_desc = lv_label_create(hc_text_cont);
        lv_label_set_text(hc_desc, "Patron entrecruzado con desfase angular");
        lv_obj_set_style_text_font(hc_desc, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(hc_desc, UI::color_bg_dark, 0);
    }
    
    void crear_pantalla_configuracion() {
        screen_config = lv_obj_create(NULL);
        lv_obj_add_style(screen_config, &UI::style_main_bg, 0);
        lv_obj_set_layout(screen_config, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(screen_config, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(screen_config, 5, 0);
        lv_obj_set_style_pad_row(screen_config, 5, 0);
        lv_obj_clear_flag(screen_config, LV_OBJ_FLAG_SCROLLABLE);

        UI::create_header(screen_config, LV_SYMBOL_EDIT " TRANSFORMADOR", 
                         UIHandlers::back_and_save_handler,
                         UIHandlers::validate_and_wind_handler,
                         "Bobinar " LV_SYMBOL_RIGHT);

        lv_obj_t *params_cont = lv_obj_create(screen_config);
        lv_obj_set_width(params_cont, LV_PCT(100));
        lv_obj_add_style(params_cont, &UI::style_card, 0);
        lv_obj_set_flex_grow(params_cont, 1);
        lv_obj_set_layout(params_cont, LV_LAYOUT_GRID);
        
        static lv_coord_t col_dsc[] = {LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(params_cont, col_dsc, row_dsc);
        lv_obj_set_style_pad_row(params_cont, 5, 0);
        
        auto create_param_row = [&](int r, const char *l, const char *v, const char *u) -> lv_obj_t * {
            lv_obj_t *lbl = lv_label_create(params_cont);
            lv_label_set_text(lbl, l);
            lv_obj_set_style_text_color(lbl, UI::color_text, 0);
            lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, r, 1);
            
            lv_obj_t *ta = lv_textarea_create(params_cont);
            lv_textarea_set_text(ta, v);
            lv_textarea_set_one_line(ta, true);
            lv_obj_add_style(ta, &UI::style_ta, 0);
            lv_obj_add_event_cb(ta, UIHandlers::ta_event_cb, LV_EVENT_ALL, NULL);
            lv_obj_set_grid_cell(ta, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, r, 1);
            
            lv_obj_t *ulbl = lv_label_create(params_cont);
            lv_label_set_text(ulbl, u);
            lv_obj_set_style_text_color(ulbl, UI::color_text_secondary, 0);
            lv_obj_set_grid_cell(ulbl, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, r, 1);
            
            lv_obj_set_user_data(ta, (void*)l);
            return ta;
        };
        
        char buf_diam[16], buf_long[16], buf_vueltas[16], buf_vel[16];
        snprintf(buf_diam, sizeof(buf_diam), "%.2f", Sistema::config_transformador.diametro_alambre_mm);
        snprintf(buf_long, sizeof(buf_long), "%.1f", Sistema::config_transformador.longitud_bobinado_mm);
        snprintf(buf_vueltas, sizeof(buf_vueltas), "%lu", Sistema::config_transformador.vueltas_total);
        snprintf(buf_vel, sizeof(buf_vel), "%.0f", Sistema::config_transformador.velocidad_rpm);
        
        ta_diametro_alambre = create_param_row(0, "Diametro hilo", buf_diam, "mm");
        ta_longitud_bobinado = create_param_row(1, "Ancho carrete", buf_long, "mm");
        ta_vueltas_total = create_param_row(2, "Vueltas totales", buf_vueltas, "");
        ta_velocidad_rpm = create_param_row(3, "Velocidad (Y)", buf_vel, "RPM");

        lv_obj_t *label_switch = lv_label_create(params_cont);
        lv_label_set_text(label_switch, "Pausar en cada capa");
        lv_obj_set_style_text_color(label_switch, UI::color_text, 0);
        lv_obj_set_grid_cell(label_switch, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);

        sw_detener_en_capas = lv_switch_create(params_cont);
        lv_obj_set_style_bg_color(sw_detener_en_capas, UI::color_primary, LV_PART_INDICATOR | LV_STATE_CHECKED);
        if (Sistema::config_transformador.detener_en_capas) {
            lv_obj_add_state(sw_detener_en_capas, LV_STATE_CHECKED);
        }
        lv_obj_set_grid_cell(sw_detener_en_capas, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    }
    
        void crear_pantalla_configuracion_honeycomb() {
        screen_config_honeycomb = lv_obj_create(NULL);
        lv_obj_add_style(screen_config_honeycomb, &UI::style_main_bg, 0);
        lv_obj_set_layout(screen_config_honeycomb, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(screen_config_honeycomb, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(screen_config_honeycomb, 5, 0);
        lv_obj_set_style_pad_row(screen_config_honeycomb, 5, 0);
        lv_obj_clear_flag(screen_config_honeycomb, LV_OBJ_FLAG_SCROLLABLE);

        UI::create_header(screen_config_honeycomb, LV_SYMBOL_IMAGE " CONFIG NIDO ABEJA", 
                         UIHandlers::back_and_save_honeycomb_handler,
                         UIHandlers::validate_and_wind_honeycomb_handler,
                         "Bobinar " LV_SYMBOL_RIGHT);

        lv_obj_t *params_cont = lv_obj_create(screen_config_honeycomb);
        lv_obj_set_width(params_cont, LV_PCT(100));
        lv_obj_add_style(params_cont, &UI::style_card, 0);
        lv_obj_set_flex_grow(params_cont, 1);
        lv_obj_set_layout(params_cont, LV_LAYOUT_GRID);
        
        static lv_coord_t col_dsc_hc[] = {LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t row_dsc_hc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), 
                                           LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(params_cont, col_dsc_hc, row_dsc_hc);
        lv_obj_set_style_pad_row(params_cont, 5, 0);
        
        auto create_param_row = [&](int r, const char *l, const char *v, const char *u) -> lv_obj_t * {
            lv_obj_t *lbl = lv_label_create(params_cont);
            lv_label_set_text(lbl, l);
            lv_obj_set_style_text_color(lbl, UI::color_text, 0);
            lv_obj_set_grid_cell(lbl, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, r, 1);
            
            lv_obj_t *ta = lv_textarea_create(params_cont);
            lv_textarea_set_text(ta, v);
            lv_textarea_set_one_line(ta, true);
            lv_obj_add_style(ta, &UI::style_ta, 0);
            lv_obj_add_event_cb(ta, UIHandlers::ta_event_cb, LV_EVENT_ALL, NULL);
            lv_obj_set_grid_cell(ta, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, r, 1);
            
            lv_obj_t *ulbl = lv_label_create(params_cont);
            lv_label_set_text(ulbl, u);
            lv_obj_set_style_text_color(ulbl, UI::color_text_secondary, 0);
            lv_obj_set_grid_cell(ulbl, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, r, 1);
            
            lv_obj_set_user_data(ta, (void*)l);
            return ta;
        };
        
        char buf_hilo[16], buf_carrete[16], buf_ancho[16], buf_grados[16], buf_vueltas[16], buf_vel[16];
        snprintf(buf_hilo, sizeof(buf_hilo), "%.2f", Sistema::config_nido_abeja.diametro_hilo);
        snprintf(buf_carrete, sizeof(buf_carrete), "%.1f", Sistema::config_nido_abeja.diametro_carrete);
        snprintf(buf_ancho, sizeof(buf_ancho), "%.1f", Sistema::config_nido_abeja.ancho_carrete);
        snprintf(buf_grados, sizeof(buf_grados), "%.1f", Sistema::config_nido_abeja.desfase_grados);
        snprintf(buf_vueltas, sizeof(buf_vueltas), "%lu", Sistema::config_nido_abeja.num_vueltas);
        snprintf(buf_vel, sizeof(buf_vel), "%.0f", Sistema::config_nido_abeja.velocidad_rpm);
        
        ta_hc_diametro_hilo = create_param_row(0, "Diametro hilo", buf_hilo, "mm");
        ta_hc_diametro_carrete = create_param_row(1, "Diam. carrete", buf_carrete, "mm");
        ta_hc_ancho_carrete = create_param_row(2, "Ancho carrete", buf_ancho, "mm");
        ta_hc_desfase_grados = create_param_row(3, "Desfase", buf_grados, "°");
        ta_hc_num_vueltas = create_param_row(4, "Num. vueltas", buf_vueltas, "");
        ta_hc_velocidad = create_param_row(5, "Velocidad", buf_vel, "RPM");
    }
    
    void crear_pantalla_bobinado() {
        screen_winding = lv_obj_create(NULL);
        lv_obj_add_style(screen_winding, &UI::style_main_bg, 0);
        lv_obj_add_event_cb(screen_winding, UIHandlers::screen_winding_load_handler, LV_EVENT_SCREEN_LOADED, NULL);
        lv_obj_set_layout(screen_winding, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(screen_winding, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(screen_winding, 5, 0);
        lv_obj_set_style_pad_row(screen_winding, 3, 0);
        lv_obj_clear_flag(screen_winding, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *header_cont = lv_obj_create(screen_winding);
        lv_obj_set_width(header_cont, LV_PCT(100));
        lv_obj_set_height(header_cont, LV_SIZE_CONTENT);
        lv_obj_add_style(header_cont, &UI::style_card, 0);
        lv_obj_set_style_pad_ver(header_cont, 6, 0);
        lv_obj_set_layout(header_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(header_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        lv_obj_t *btn_home = lv_btn_create(header_cont);
        lv_obj_set_size(btn_home, 40, 40);
        lv_obj_add_style(btn_home, &UI::style_btn_primary, 0);
        lv_obj_set_user_data(btn_home, (void *)"PRINCIPAL");
        lv_obj_add_event_cb(btn_home, UIHandlers::btn_navegacion_handler, LV_EVENT_CLICKED, NULL);
        lv_obj_t *label_home = lv_label_create(btn_home);
        lv_label_set_text(label_home, LV_SYMBOL_HOME);
        lv_obj_center(label_home);
        
        lv_obj_t *header_title = lv_label_create(header_cont);
        lv_label_set_text(header_title, "CONTROL DE BOBINADO");
        lv_obj_add_style(header_title, &UI::style_header, 0);
        lv_obj_set_flex_grow(header_title, 1);
        lv_obj_set_style_text_align(header_title, LV_TEXT_ALIGN_CENTER, 0);
        
        label_modo_actual = lv_label_create(header_cont);
        lv_label_set_text(label_modo_actual, LV_SYMBOL_LOOP " Transformador");
        lv_obj_set_style_text_font(label_modo_actual, &lv_font_montserrat_14, 0);
        
        label_estado = lv_label_create(header_cont);
        lv_label_set_text(label_estado, "LISTO");
        lv_obj_set_style_text_font(label_estado, &lv_font_montserrat_16, 0);

        lv_obj_t *info_panel = lv_obj_create(screen_winding);
        lv_obj_set_width(info_panel, LV_PCT(100));
        lv_obj_add_style(info_panel, &UI::style_card, 0);
        lv_obj_set_style_pad_ver(info_panel, 6, 0);
        lv_obj_set_style_pad_hor(info_panel, 10, 0);
        lv_obj_set_flex_grow(info_panel, 1);
        lv_obj_set_flex_flow(info_panel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(info_panel, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t *vueltas_cont = lv_obj_create(info_panel);
        lv_obj_set_size(vueltas_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(vueltas_cont, 0, 0);
        lv_obj_set_flex_flow(vueltas_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(vueltas_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
        lv_obj_set_style_bg_opa(vueltas_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(vueltas_cont, 0, 0);
        label_vueltas_actuales = lv_label_create(vueltas_cont);
        lv_label_set_text(label_vueltas_actuales, "0");
        lv_obj_add_style(label_vueltas_actuales, &UI::style_label_value, 0);
        label_vueltas_totales = lv_label_create(vueltas_cont);
        lv_label_set_text(label_vueltas_totales, "/ 0");
        lv_obj_set_style_text_font(label_vueltas_totales, &lv_font_montserrat_20, 0);
        lv_obj_set_style_pad_bottom(label_vueltas_totales, 5, 0);
        lv_obj_set_style_text_color(label_vueltas_totales, UI::color_text_secondary, 0);

        lv_obj_t *prog_cont = lv_obj_create(info_panel);
        lv_obj_set_width(prog_cont, LV_PCT(95));
        lv_obj_set_height(prog_cont, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(prog_cont, 0, 0);
        lv_obj_set_layout(prog_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(prog_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_opa(prog_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(prog_cont, 0, 0);
        bar_progreso = lv_bar_create(prog_cont);
        lv_obj_set_width(bar_progreso, LV_PCT(85));
        lv_obj_set_height(bar_progreso, 15);
        label_progreso = lv_label_create(prog_cont);
        lv_label_set_text(label_progreso, "0%");
        lv_obj_set_style_text_font(label_progreso, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(label_progreso, UI::color_text_secondary, 0);

        lv_obj_t *extra_info_cont = lv_obj_create(info_panel);
        lv_obj_set_width(extra_info_cont, LV_PCT(100));
        lv_obj_set_height(extra_info_cont, LV_SIZE_CONTENT);
        lv_obj_set_layout(extra_info_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(extra_info_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_opa(extra_info_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(extra_info_cont, 0, 0);
        label_capa_info = lv_label_create(extra_info_cont);
        lv_label_set_text(label_capa_info, "Capa: 0/0 | Vueltas: 0/0 | Grosor: 0.00 mm");
        lv_obj_add_style(label_capa_info, &UI::style_text_secondary, 0);
        
        lv_obj_t *sec_panel = lv_obj_create(info_panel);
        lv_obj_set_width(sec_panel, LV_PCT(100));
        lv_obj_set_height(sec_panel, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(sec_panel, 0, 0);
        lv_obj_set_layout(sec_panel, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(sec_panel, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_opa(sec_panel, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(sec_panel, 0, 0);
        label_velocidad_actual = lv_label_create(sec_panel);
        lv_label_set_text(label_velocidad_actual, LV_SYMBOL_CHARGE " 0 RPM");
        lv_obj_set_style_text_color(label_velocidad_actual, UI::color_text, 0);
        label_tiempo_restante = lv_label_create(sec_panel);
        lv_label_set_text(label_tiempo_restante, LV_SYMBOL_POWER " --:--");
        lv_obj_set_style_text_color(label_tiempo_restante, UI::color_text, 0);

        lv_obj_t *ctrl_panel = lv_obj_create(screen_winding);
        lv_obj_set_width(ctrl_panel, LV_PCT(100));
        lv_obj_set_height(ctrl_panel, LV_SIZE_CONTENT);
        lv_obj_add_style(ctrl_panel, &UI::style_card, 0);
        lv_obj_set_style_pad_ver(ctrl_panel, 6, 0);
        lv_obj_set_layout(ctrl_panel, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(ctrl_panel, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        struct {const char *t; const char *id; lv_style_t *s;} btns[] = {
            {LV_SYMBOL_PLAY " Iniciar", "INICIAR", &UI::style_btn_success},
            {LV_SYMBOL_NEXT " Seguir", "PROSEGUIR", &UI::style_btn_primary},
            {LV_SYMBOL_PAUSE " Pausar", "PAUSAR", &UI::style_btn_warning},
            {LV_SYMBOL_STOP " Parar", "PARAR", &UI::style_btn_danger}
        };
        for (int i=0; i<4; i++) {
            lv_obj_t *btn = lv_btn_create(ctrl_panel);
            lv_obj_set_user_data(btn, (void*)btns[i].id);
            lv_obj_set_size(btn, 100, 45);
            lv_obj_add_style(btn, btns[i].s, 0);
            lv_obj_add_event_cb(btn, UIHandlers::btn_comando_handler, LV_EVENT_CLICKED, NULL);
            lv_obj_t *l = lv_label_create(btn);
            lv_label_set_text(l, btns[i].t);
            lv_obj_center(l);
        }
    }
    
    void crear_pantalla_control_manual() {
        screen_manual_control = lv_obj_create(NULL);
        lv_obj_add_style(screen_manual_control, &UI::style_main_bg, 0);
        lv_obj_set_layout(screen_manual_control, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(screen_manual_control, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_all(screen_manual_control, 5, 0);
        lv_obj_set_style_pad_row(screen_manual_control, 5, 0);
        lv_obj_clear_flag(screen_manual_control, LV_OBJ_FLAG_SCROLLABLE);

        UI::create_header(screen_manual_control, "  " LV_SYMBOL_GPS " CONTROL MANUAL",
                         UIHandlers::btn_navegacion_handler);

        lv_obj_t *warning_card = lv_obj_create(screen_manual_control);
        lv_obj_set_width(warning_card, LV_PCT(100));
        lv_obj_set_height(warning_card, LV_SIZE_CONTENT);
        lv_obj_add_style(warning_card, &UI::style_card, 0);
        lv_obj_set_style_bg_color(warning_card, UI::color_warning, 0);
        lv_obj_t *warning = lv_label_create(warning_card);
        lv_label_set_text(warning, LV_SYMBOL_WARNING " Disponible solo en estado LISTO o PAUSADO");
        lv_obj_center(warning);
        lv_obj_set_style_text_color(warning, UI::color_bg_dark, 0);
        lv_obj_set_style_text_font(warning, &lv_font_montserrat_12, 0);

        lv_obj_t *axes_panel = lv_obj_create(screen_manual_control);
        lv_obj_set_width(axes_panel, LV_PCT(100));
        lv_obj_set_flex_grow(axes_panel, 1);
        lv_obj_add_style(axes_panel, &UI::style_card, 0);
        lv_obj_set_flex_flow(axes_panel, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(axes_panel, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        
        lv_obj_t *x_cont = lv_obj_create(axes_panel);
        lv_obj_set_width(x_cont, LV_PCT(100));
        lv_obj_set_height(x_cont, LV_SIZE_CONTENT);
        lv_obj_set_layout(x_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(x_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_opa(x_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(x_cont, 0, 0);
        lv_obj_t *label_x = lv_label_create(x_cont);
        lv_label_set_text(label_x, "EJE X:");
        lv_obj_set_style_text_color(label_x, UI::color_primary, 0);
        
        lv_obj_t *btn_x_minus = UI::create_button(x_cont, LV_SYMBOL_LEFT " -1mm", "X-", 
                                                  &UI::style_btn_warning,
                                                  UIHandlers::btn_manual_handler, 150, 40);
        lv_obj_t *btn_x_plus = UI::create_button(x_cont, "+1mm " LV_SYMBOL_RIGHT, "X+",
                                                 &UI::style_btn_warning,
                                                 UIHandlers::btn_manual_handler, 150, 40);

        lv_obj_t *y_cont = lv_obj_create(axes_panel);
        lv_obj_set_width(y_cont, LV_PCT(100));
        lv_obj_set_height(y_cont, LV_SIZE_CONTENT);
        lv_obj_set_layout(y_cont, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(y_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_opa(y_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(y_cont, 0, 0);
        lv_obj_t *label_y = lv_label_create(y_cont);
        lv_label_set_text(label_y, "EJE Y:");
        lv_obj_set_style_text_color(label_y, UI::color_success, 0);
        
        lv_obj_t *btn_y_minus = UI::create_button(y_cont, LV_SYMBOL_MINUS " -1 Vuelta", "Y-",
                                                  &UI::style_btn_warning,
                                                  UIHandlers::btn_manual_handler, 150, 40);
        lv_obj_t *btn_y_plus = UI::create_button(y_cont, LV_SYMBOL_PLUS " +1 Vuelta", "Y+",
                                                 &UI::style_btn_warning,
                                                 UIHandlers::btn_manual_handler, 150, 40);
    }
    
    void init_all_screens() {
        crear_pantalla_principal();
        crear_pantalla_seleccion_modo();
        crear_pantalla_configuracion();
        crear_pantalla_configuracion_honeycomb();
        crear_pantalla_bobinado();
        crear_pantalla_control_manual();
    }
}