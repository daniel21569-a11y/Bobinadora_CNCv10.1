================================================================================
                    BOBINADORA CNC PROFESIONAL
                  Sistema Dual: Transformador + Nido de Abeja
================================================================================

VERSIÓN: 8.2 FINAL
FECHA: 2024
PLATAFORMA: ESP32-S3
FRAMEWORK: Arduino IDE + LVGL 9.3

================================================================================
                          DESCRIPCIÓN DEL PROYECTO
================================================================================

Sistema de control avanzado para bobinadora CNC con interfaz táctil moderna.
Permite dos modos de bobinado:

1. MODO TRANSFORMADOR
   - Bobinado tradicional capa por capa
   - Control preciso de vueltas y capas
   - Pausa automática entre capas (opcional)
   - Ideal para transformadores, inductores y bobinas estándar

2. MODO NIDO DE ABEJA
   - Patrón entrecruzado con desfase angular
   - Control de grosor dinámico
   - Desfase configurable en grados (0-360°)
   - Ideal para bobinas RF, altavoces y aplicaciones especiales

================================================================================
                          CARACTERÍSTICAS PRINCIPALES
================================================================================

INTERFAZ GRÁFICA (LVGL 9.3)
✓ Pantalla táctil capacitiva 480x272 pixels
✓ Diseño moderno y profesional sin scroll
✓ Teclado numérico personalizado para ingreso de valores
✓ Visualización en tiempo real de progreso
✓ Indicadores de velocidad, vueltas, capas y grosor
✓ Estimación de tiempo restante

CONTROL DE MOTORES
✓ Control dual de motores paso a paso
✓ Procesamiento en núcleo separado (FreeRTOS)
✓ Rampa de aceleración/desaceleración suave
✓ Sincronización precisa entre ejes X e Y
✓ Sistema de reset automático sin bloqueos

NAVEGACIÓN
✓ Pantalla principal con acceso directo
✓ Selección de modo de bobinado
✓ Configuración independiente por modo
✓ Control manual de ejes
✓ Función HOME para resetear posiciones

================================================================================
                          HARDWARE REQUERIDO
================================================================================

MICROCONTROLADOR
- ESP32-S3 (modelo JC4827W543 o compatible)
- CPU dual-core a 240MHz
- PSRAM para buffer de pantalla

PANTALLA
- LCD TFT 480x272 pixels
- Controlador táctil GT911
- Interfaz I2C para touch

MOTORES Y DRIVERS
- 2x Motores paso a paso NEMA17 (o similar)
- 2x Drivers de motor paso a paso (A4988, DRV8825, TMC2208, etc.)
- Configuración: 1/16 micropasos
- Eje X (Carro): 320 pasos/mm
- Eje Y (Mandril): 3200 pasos/vuelta

FUENTE DE ALIMENTACIÓN
- 12-24V para motores (según driver y motor)
- 5V para ESP32 y pantalla

================================================================================
                          CONFIGURACIÓN DE PINES
================================================================================

PANTALLA TÁCTIL GT911
- SDA: GPIO 8
- SCL: GPIO 4
- INT: GPIO 3
- RST: GPIO 38

MOTOR X (CARRO - MOVIMIENTO LATERAL)
- STEP: GPIO 5
- DIR:  GPIO 9
- EN:   GPIO 14

MOTOR Y (MANDRIL - ROTACIÓN)
- STEP: GPIO 6
- DIR:  GPIO 7
- EN:   GPIO 15

FINALES DE CARRERA (Opcionales)
- LIMIT X: GPIO 46
- LIMIT Y: GPIO 16

IMPORTANTE: Verificar que estos pines coincidan con tu hardware específico
antes de cargar el código.

================================================================================
                          PARÁMETROS DE CALIBRACIÓN
================================================================================

MOTOR EJE X (CARRO)
Pasos por milímetro: 320 pasos/mm
Configuración en código: pasos_por_mm_X = 320.0f

Cálculo:
- Pasos por revolución del motor: 200 (motor 1.8°)
- Micropasos: 16
- Pasos totales: 200 × 16 = 3200 pasos/rev
- Paso del tornillo/correa: Depende de tu mecánica
- Fórmula: pasos_por_mm = (pasos_totales) / (avance_por_revolución_mm)

MOTOR EJE Y (MANDRIL)
Pasos por vuelta: 3200 pasos/vuelta
Configuración en código: pasos_por_vuelta_Y = 3200

Cálculo:
- Si hay reducción 1:1: 3200 pasos = 1 vuelta del carrete
- Si hay reducción X:Y: Ajustar proporcionalmente

================================================================================
                          GUÍA DE USO
================================================================================

INICIO DEL SISTEMA
1. Conectar alimentación
2. El sistema inicia mostrando la pantalla principal
3. Verificar en monitor serial (115200 baudios) el inicio correcto

SELECCIÓN DE MODO
1. Presionar botón "CONFIGURAR" en pantalla principal
2. Elegir modo:
   - TRANSFORMADOR: Bobinado tradicional
   - NIDO DE ABEJA: Patrón entrecruzado

CONFIGURACIÓN - MODO TRANSFORMADOR
1. Diámetro hilo (mm): Grosor del alambre a bobinar
2. Ancho carrete (mm): Longitud de bobinado disponible
3. Vueltas totales: Número total de vueltas deseadas
4. Velocidad (RPM): Velocidad de rotación del mandril
5. Pausar en cada capa: ON/OFF

CONFIGURACIÓN - MODO NIDO DE ABEJA
1. Diámetro hilo (mm): Grosor del alambre (para cálculo de grosor)
2. Diámetro carrete (mm): Diámetro del carrete/mandril
3. Ancho carrete (mm): Ancho útil de bobinado
4. Desfase (grados): Ángulo de desfase (0-360°)
   - 0° = Sin desfase (bobinado recto)
   - 36° = Desfase mínimo
   - 72° = Nido de abeja estándar (recomendado)
   - 90° = Patrón abierto
   - 120° = Muy abierto
5. Num. vueltas: Total de vueltas a realizar
6. Velocidad (RPM): Velocidad de rotación

INICIO DEL BOBINADO
1. Configurar parámetros
2. Presionar "Bobinar" o volver y presionar "BOBINAR"
3. El sistema hace HOME automáticamente
4. Presionar "Iniciar" para comenzar

CONTROLES DURANTE EL BOBINADO
- Iniciar: Comienza el bobinado desde cero
- Pausar: Detiene temporalmente (motores energizados)
- Seguir: Continúa desde pausa
- Parar: Detiene completamente y resetea

CONTROL MANUAL
1. Acceder desde pantalla principal
2. Solo disponible en estado LISTO o PAUSADO
3. Permite mover cada eje independientemente:
   - Eje X: ±1mm por clic
   - Eje Y: ±1 vuelta por clic

FUNCIÓN HOME
- Resetea todas las posiciones a cero
- Limpia contadores de vueltas y capas
- Preparar sistema para nuevo bobinado
- IMPORTANTE: Colocar manualmente el cabezal en posición inicial

================================================================================
                          FÓRMULAS Y CÁLCULOS
================================================================================

MODO TRANSFORMADOR
------------------
Avance por vuelta = Diámetro del hilo
Vueltas por capa = Ancho del carrete / Diámetro del hilo
Capas totales = Vueltas totales / Vueltas por capa
Grosor = Capas totales × Diámetro del hilo

Ejemplo:
- Hilo: 0.5mm
- Ancho: 50mm
- Vueltas totales: 1000
→ Vueltas/capa: 50/0.5 = 100
→ Capas: 1000/100 = 10 capas
→ Grosor: 10 × 0.5 = 5mm

MODO NIDO DE ABEJA
------------------
Factor desfase = 1.0 + (Desfase en grados / 360)
Pasos Y por ciclo = Pasos por vuelta × Factor desfase
Vueltas por capa = 360° / Desfase en grados
Grosor = (Vueltas completadas / Vueltas por capa) × Diámetro hilo

Ejemplo:
- Desfase: 72°
- Factor: 1.0 + (72/360) = 1.2
- Hilo: 0.2mm
- Vueltas totales: 500
→ Vueltas/capa: 360/72 = 5 vueltas
→ Capas estimadas: 500/5 = 100 capas
→ Grosor final: 100 × 0.2 = 20mm

SINCRONIZACIÓN NIDO DE ABEJA
En cada vuelta de Y (mandril):
- X hace un recorrido IDA+VUELTA completo
- Primera mitad de vuelta Y: X avanza (IDA)
- Segunda mitad de vuelta Y: X retrocede (VUELTA)
- Al final del ciclo, X vuelve a posición inicial
- Y ha girado Factor × 360° (ej: 1.2 × 360° = 432°)
- Este desfase angular crea el patrón entrecruzado

================================================================================
                          SOLUCIÓN DE PROBLEMAS
================================================================================

PROBLEMA: Pantalla no responde al tacto
SOLUCIÓN: 
- Verificar conexiones I2C (SDA, SCL)
- Verificar pin INT y RST del GT911
- Comprobar en monitor serial si detecta toques

PROBLEMA: Motores no se mueven
SOLUCIÓN:
- Verificar pines STEP, DIR, EN
- Comprobar que drivers están alimentados
- Verificar que micropasos están configurados a 1/16
- Presionar 'e' para habilitar motores
- Probar función TEST desde menú

PROBLEMA: Movimiento errático
SOLUCIÓN:
- Verificar alimentación estable de motores
- Reducir velocidad (aumentar microsegundos entre pasos)
- Verificar conexiones de cables de motores
- Comprobar que no hay interferencias electromagnéticas

PROBLEMA: Deriva en posición X (nido de abeja)
SOLUCIÓN:
- El sistema tiene corrección automática al final de cada ciclo
- Si persiste, verificar paso del tornillo/correa
- Ajustar valor pasos_por_mm_X según calibración real

PROBLEMA: Sistema se bloquea al cambiar configuración
SOLUCIÓN:
- El sistema incluye reset automático
- Esperar 1-2 segundos después de terminar un bobinado
- Si persiste, presionar HOME antes de configurar

PROBLEMA: Tiempo restante incorrecto
SOLUCIÓN:
- Es una estimación basada en velocidad actual
- Durante aceleración puede ser impreciso
- Se estabiliza cuando alcanza velocidad objetivo

================================================================================
                          NOTAS TÉCNICAS
================================================================================

ARQUITECTURA DEL SISTEMA
- Core 0: Interfaz gráfica LVGL (loop principal)
- Core 1: Control de motores (tarea FreeRTOS)
- Comunicación entre cores mediante variables volatile

SINCRONIZACIÓN
- Actualización UI cada 100ms (timer LVGL)
- Generación de pasos en tiempo real (microsegundos)
- Sistema de acumuladores para precisión sub-paso

SEGURIDAD
- Límites de software para proteger mecánica
- Rampa de aceleración para evitar pérdida de pasos
- Sistema de pausa con motores energizados
- Reset automático entre bobinados

OPTIMIZACIONES
- Buffer de pantalla en PSRAM
- Render parcial (solo áreas modificadas)
- Cálculos flotantes optimizados
- Event bubbling para táctil responsivo

LIMITACIONES
- Velocidad máxima depende de motores y drivers
- Precisión limitada por resolución de micropasos
- Memoria disponible para futuras expansiones

================================================================================
                          VALORES RECOMENDADOS
================================================================================

VELOCIDADES SEGURAS
- Pruebas iniciales: 30-50 RPM
- Bobinado fino: 80-120 RPM
- Bobinado grueso: 150-300 RPM
- Máxima (depende hardware): 500+ RPM

CONFIGURACIONES TÍPICAS

Transformador de potencia:
- Hilo: 0.8mm
- Ancho: 40mm
- Vueltas: 500
- Velocidad: 100 RPM

Inductor RF:
- Hilo: 0.3mm
- Ancho: 15mm
- Vueltas: 200
- Velocidad: 80 RPM

Bobina de altavoz (Nido de Abeja):
- Hilo: 0.1mm
- Ancho: 8mm
- Desfase: 72°
- Vueltas: 1000
- Velocidad: 60 RPM

================================================================================
                          MANTENIMIENTO
================================================================================

RECOMENDACIONES
- Lubricar mecánica periódicamente
- Verificar tensión de correas/tornillos
- Limpiar guías y ejes
- Revisar conexiones eléctricas
- Mantener drivers ventilados

CALIBRACIÓN PERIÓDICA
- Verificar paso real del eje X cada 6 meses
- Comprobar alineación de ejes
- Revisar tensión de hilo durante bobinado

ACTUALIZACIONES
- Código fuente comentado para fácil modificación
- Parámetros centralizados al inicio del código
- Estructura modular para añadir funciones

================================================================================
                          CONTACTO Y SOPORTE
================================================================================

MONITOR SERIAL
- Baudios: 115200
- Mensajes de depuración detallados
- Información de estado en tiempo real

LOGS DEL SISTEMA
El sistema imprime en serial:
- Inicio de sistema
- Parámetros calculados
- Eventos de bobinado
- Errores y correcciones

================================================================================
                          LICENCIA Y GARANTÍA
================================================================================

Este software se proporciona "TAL CUAL" sin garantías de ningún tipo.
El usuario es responsable de:
- Verificar configuración antes de operar
- Seguridad de la instalación mecánica
- Protecciones eléctricas adecuadas
- Uso apropiado del equipo

Recomendado: Implementar paro de emergencia físico en el hardware.

================================================================================
                          FIN DE LA DOCUMENTACIÓN
================================================================================
