#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <Preferences.h>
#include "config.h"

class PersistenceManager {
private:
    Preferences preferences;
    
public:
    // =========================================================================
    // INICIALIZACIÓN
    // =========================================================================
    void begin() {
        Serial.println("Inicializando sistema de persistencia...");
    }
    
    // =========================================================================
    // GUARDAR CONFIGURACIÓN TRANSFORMADOR
    // =========================================================================
    bool saveTransformadorConfig() {
        if (!preferences.begin("transformer", false)) {
            Serial.println("ERROR: No se pudo abrir namespace transformer");
            return false;
        }
        
        preferences.putFloat("diam_alambre", Sistema::config_transformador.diametro_alambre_mm);
        preferences.putFloat("long_bobinado", Sistema::config_transformador.longitud_bobinado_mm);
        preferences.putUInt("vueltas_total", Sistema::config_transformador.vueltas_total);
        preferences.putFloat("velocidad_rpm", Sistema::config_transformador.velocidad_rpm);
        preferences.putBool("detener_capas", Sistema::config_transformador.detener_en_capas);
        
        preferences.end();
        Serial.println("✓ Config Transformador guardada");
        return true;
    }
    
    // =========================================================================
    // CARGAR CONFIGURACIÓN TRANSFORMADOR
    // =========================================================================
    bool loadTransformadorConfig() {
        if (!preferences.begin("transformer", true)) {
            Serial.println("No hay configuración guardada de Transformador");
            return false;
        }
        
        Sistema::config_transformador.diametro_alambre_mm = preferences.getFloat("diam_alambre", 0.5f);
        Sistema::config_transformador.longitud_bobinado_mm = preferences.getFloat("long_bobinado", 50.0f);
        Sistema::config_transformador.vueltas_total = preferences.getUInt("vueltas_total", 1000);
        Sistema::config_transformador.velocidad_rpm = preferences.getFloat("velocidad_rpm", 100.0f);
        Sistema::config_transformador.detener_en_capas = preferences.getBool("detener_capas", false);
        
        preferences.end();
        
        // Recalcular parámetros
        Sistema::config_transformador.calcular_parametros();
        
        Serial.println("✓ Config Transformador cargada:");
        Sistema::config_transformador.imprimir_debug();
        return true;
    }
    
    // =========================================================================
    // GUARDAR CONFIGURACIÓN NIDO DE ABEJA
    // =========================================================================
    bool saveHoneycombConfig() {
        if (!preferences.begin("honeycomb", false)) {
            Serial.println("ERROR: No se pudo abrir namespace honeycomb");
            return false;
        }
        
        preferences.putFloat("diam_hilo", Sistema::config_nido_abeja.diametro_hilo);
        preferences.putFloat("diam_carrete", Sistema::config_nido_abeja.diametro_carrete);
        preferences.putFloat("ancho_carrete", Sistema::config_nido_abeja.ancho_carrete);
        preferences.putFloat("desfase_grados", Sistema::config_nido_abeja.desfase_grados);
        preferences.putUInt("num_vueltas", Sistema::config_nido_abeja.num_vueltas);
        preferences.putFloat("velocidad_rpm", Sistema::config_nido_abeja.velocidad_rpm);
        
        preferences.end();
        Serial.println("✓ Config Nido de Abeja guardada");
        return true;
    }
    
    // =========================================================================
    // CARGAR CONFIGURACIÓN NIDO DE ABEJA
    // =========================================================================
    bool loadHoneycombConfig() {
        if (!preferences.begin("honeycomb", true)) {
            Serial.println("No hay configuración guardada de Nido de Abeja");
            return false;
        }
        
        Sistema::config_nido_abeja.diametro_hilo = preferences.getFloat("diam_hilo", 0.2f);
        Sistema::config_nido_abeja.diametro_carrete = preferences.getFloat("diam_carrete", 20.0f);
        Sistema::config_nido_abeja.ancho_carrete = preferences.getFloat("ancho_carrete", 10.0f);
        Sistema::config_nido_abeja.desfase_grados = preferences.getFloat("desfase_grados", 72.0f);
        Sistema::config_nido_abeja.num_vueltas = preferences.getUInt("num_vueltas", 500);
        Sistema::config_nido_abeja.velocidad_rpm = preferences.getFloat("velocidad_rpm", 100.0f);
        
        preferences.end();
        
        // Recalcular parámetros
        Sistema::config_nido_abeja.calcular_parametros();
        
        Serial.println("✓ Config Nido de Abeja cargada:");
        Sistema::config_nido_abeja.imprimir_debug();
        return true;
    }
    
    // =========================================================================
    // GUARDAR ÚLTIMO MODO USADO
    // =========================================================================
    void saveLastMode(ModoBobinado modo) {
        preferences.begin("system", false);
        preferences.putUChar("last_mode", static_cast<uint8_t>(modo));
        preferences.end();
        Serial.print("✓ Modo guardado: ");
        Serial.println(modo == ModoBobinado::TRANSFORMADOR ? "TRANSFORMADOR" : "NIDO DE ABEJA");
    }
    
    // =========================================================================
    // CARGAR ÚLTIMO MODO USADO
    // =========================================================================
    ModoBobinado loadLastMode() {
        preferences.begin("system", true);
        uint8_t modo = preferences.getUChar("last_mode", 0);
        preferences.end();
        return static_cast<ModoBobinado>(modo);
    }
    
    // =========================================================================
    // CARGAR TODA LA CONFIGURACIÓN
    // =========================================================================
    void loadAll() {
        Serial.println("========================================");
        Serial.println("Cargando configuración guardada...");
        
        // Cargar último modo
        Sistema::estado.modo = loadLastMode();
        
        // Cargar configuraciones
        loadTransformadorConfig();
        loadHoneycombConfig();
        
        Serial.println("========================================");
    }
    
    // =========================================================================
    // GUARDAR TODA LA CONFIGURACIÓN
    // =========================================================================
    void saveAll() {
        saveLastMode(Sistema::estado.modo);
        saveTransformadorConfig();
        saveHoneycombConfig();
        Serial.println("✓ Toda la configuración guardada");
    }
    
    // =========================================================================
    // BORRAR TODA LA CONFIGURACIÓN
    // =========================================================================
    void clearAll() {
        preferences.begin("transformer", false);
        preferences.clear();
        preferences.end();
        
        preferences.begin("honeycomb", false);
        preferences.clear();
        preferences.end();
        
        preferences.begin("system", false);
        preferences.clear();
        preferences.end();
        
        Serial.println("✓ Toda la configuración borrada");
    }
};

// Instancia global
static PersistenceManager Persistence;

#endif // PERSISTENCE_H