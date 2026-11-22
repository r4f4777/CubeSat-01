# CubeSat-01 — Roadmap del proyecto

> **Objetivo:** construir un laboratorio educativo de ingeniería espacial que combine un núcleo de software de vuelo en C++, una simulación orbital visual, una estación terrena en el PC y, finalmente, un banco de pruebas físico (*flat-sat*) con microcontrolador.
>
> **Filosofía:** aprender paso a paso. Primero entender, después implementar, probar e integrar. La visualización y el hardware deben aprovechar lo construido, no sustituirlo.

**Estado del documento:** hoja de ruta inicial · noviembre de 2026  
**Horizonte orientativo:** 6–12 meses a ritmo flexible; no son plazos comprometidos.

---

## Visión final

Una aplicación desde la que podamos:

- Ver la Tierra, una órbita y el CubeSat moviéndose en ella.
- Consultar tiempo de simulación, altitud, velocidad, posición, batería y modo operativo.
- Seleccionar un objetivo terrestre (por ejemplo, **Madrid**) y visualizar si está dentro del campo de visión de la cámara.
- Solicitar fotografías y observar las comprobaciones del software: ventana temporal, orientación, energía y estado de misión.
- Acelerar, ralentizar, pausar y reanudar la simulación (`x1`, `x10`, `x100`).
- Reiniciar el ordenador de a bordo simulado, inyectar fallos y comprobar cómo recupera su estado.
- Conectar el PC con una placa real, enviar órdenes y recibir telemetría mediante USB; posteriormente, estudiar comunicaciones inalámbricas.

> **Alcance:** prototipo educativo. No se pretende fabricar un satélite apto para vuelo ni modelar inicialmente toda la física espacial con precisión de misión real.

---

## Arquitectura objetivo

```text
┌──────────────────────────────────────────────────┐
│ Estación terrena / interfaz visual (PC)          │
│ Tierra 3D · telemetría · mandos · eventos         │
└──────────────────────┬───────────────────────────┘
                       │ API o enlace local
┌──────────────────────▼───────────────────────────┐
│ Simulador y núcleo C++                            │
│ ├─ Simulación del tiempo y de la órbita           │
│ ├─ SatelliteController                            │
│ ├─ PowerManager / SensorValidator                 │
│ ├─ CameraManager / MissionManager                 │
│ └─ Persistencia, registro y pruebas              │
└──────────────────────┬───────────────────────────┘
                       │ USB serie (fase posterior)
┌──────────────────────▼───────────────────────────┐
│ Banco físico: placa STM32 + sensores             │
│ LEDs · botones · IMU · medida eléctrica · microSD │
└──────────────────────────────────────────────────┘
```

**Regla de diseño:** separar lógica de misión, simulación del entorno, presentación y control del hardware. No toda la aplicación de escritorio se podrá compilar directamente para un microcontrolador: los servicios de archivo, reloj, periféricos y comunicación tendrán implementaciones distintas.

---

## Fase 1 — Consolidar el núcleo C++

**Estado: EN CURSO**

### Ya construido

- [x] Modos energéticos: `NORMAL`, `LOW_POWER` y `SAFE`.
- [x] Validación de lecturas de batería y detección de fallos intermitentes.
- [x] Cámara simulada y contador de fotografías.
- [x] Órdenes con objetivo y ventana temporal (`PhotoCommand`).
- [x] Almacenamiento y recuperación de una orden en `mission.dat`.
- [x] Checksum educativo que incorpora el estado de la orden.
- [x] Estados `PENDING`, `EXECUTING` y `COMPLETED` definidos.
- [x] `beginExecution()` integrado antes de solicitar una fotografía.
- [x] Primera prueba integrada: una fotografía de Madrid cuando se recupera la batería, sin repetición inmediata.

### Pendiente

- [x] Al reiniciar, recuperar la orden y **no reejecutar automáticamente** si estaba en `EXECUTING`.
- [x] Resolver de forma explícita qué hacer ante un estado incierto (verificar resultado, bloquear reintento o solicitar intervención).
- [x] Persistir el resultado de la misión, diferenciando `COMPLETED`, `EXPIRED` y posibles errores; no depender únicamente de borrar el archivo.
- [x] Hacer el guardado más resistente a interrupciones (archivo temporal, validación y sustitución controlada).
- [ ] Sustituir la suma básica por un CRC adecuado y validar datos y versiones de formato.
- [ ] Separar `main.cpp` en módulos `.h` / `.cpp`.
- [ ] Crear pruebas automatizadas y dejar `main()` como punto de entrada, sin pruebas comentadas.

**Criterio para cerrar fase:** las transiciones de misión y los reinicios se comportan de forma previsible en pruebas repetibles. *No implica garantía de ejecución exactamente una vez.*

---

## Fase 2 — Modelo de órbita y visibilidad

**Estado: PENDIENTE**

- [ ] Crear un reloj de simulación independiente del reloj real.
- [ ] Definir posición orbital, altitud y velocidad del satélite.
- [ ] Empezar con una órbita circular simplificada (~500 km de altura).
- [ ] Añadir rotación terrestre y localización de Madrid.
- [ ] Calcular si un objetivo está dentro de la zona observable por la cámara.
- [ ] Introducir orientación simplificada del satélite y campo de visión.
- [ ] Convertir la visibilidad geométrica en una condición adicional de `requestPhoto()`.
- [ ] Permitir `pause`, `resume` y multiplicadores temporales.

**Criterio para cerrar fase:** al solicitar una fotografía de Madrid, el sistema espera a tener visibilidad y verifica energía y estado antes de ejecutarla.

**Evolución opcional:** propagación orbital más realista (por ejemplo, SGP4 y elementos orbitales), siempre diferenciando el modelo simplificado del realista.

---

## Fase 3 — Simulador visual y estación terrena

**Estado: PENDIENTE**

### Pantalla de misión

- [ ] Tierra 2D/3D con trayectoria orbital.
- [ ] Marcador del CubeSat y del objetivo.
- [ ] Telemetría: altitud, velocidad, coordenadas, batería, modo y estado.
- [ ] Indicador de apuntado y ventana de fotografía.
- [ ] Panel de eventos: órdenes recibidas, fotos, errores y reinicios.

### Controles

- [ ] `Play / Pause`.
- [ ] Velocidad `x1 / x10 / x100`.
- [ ] `Request Photo`.
- [ ] `Restart OBC`.
- [ ] Inyección de lectura errónea y de batería baja.

**Tecnología candidata:** interfaz web con CesiumJS para la Tierra 3D, conectada al núcleo C++. La elección concreta se validará con una prueba de concepto pequeña; podemos empezar en 2D si resulta más sencillo.

**Criterio para cerrar fase:** ejecutar desde una interfaz una misión completa y visualizar sus estados y resultados.

---

## Fase 4 — Primer hardware embebido

**Estado: PENDIENTE**

- [ ] Configurar entorno de desarrollo para una placa STM32.
- [ ] Programar un LED de estado y un pulsador.
- [ ] Leer un sensor IMU y transmitir sus datos por USB.
- [ ] Medir tensión/corriente de un circuito sencillo.
- [ ] Probar almacenamiento persistente mediante módulo microSD u otra memoria adecuada.
- [ ] Implementar una máquina de estados equivalente a la del simulador, adaptada al hardware.

**Criterio para cerrar fase:** una placa física envía telemetría y responde a órdenes del PC.

---

## Fase 5 — Integración PC ↔ hardware (*flat-sat*)

**Estado: PENDIENTE**

- [ ] Establecer un protocolo de mensajes por USB serie.
- [ ] Enviar órdenes desde la estación terrena a la placa.
- [ ] Mostrar datos de sensores reales en el panel de telemetría.
- [ ] Simular pérdida de comunicación, mensajes incorrectos y reinicios.
- [ ] Verificar qué estados sobreviven a un reinicio del microcontrolador.
- [ ] Conectar la órbita simulada con las decisiones de una placa física.

**Criterio para cerrar fase:** desde el PC se puede enviar una orden, seguir su estado y recuperar una sesión tras un reinicio.

---

## Fase 6 — Ampliaciones avanzadas

**Estado: OPCIONAL / FUTURO**

- [ ] Watchdog, supervisión de tareas y *heartbeat*.
- [ ] Planificación de tareas periódicas y, si interesa, RTOS.
- [ ] Registro de eventos y diagnóstico de fallos.
- [ ] Protocolos con comprobación de integridad, confirmaciones y reintentos.
- [ ] Comunicaciones inalámbricas de corto alcance, previa revisión de requisitos técnicos y legales.
- [ ] Modelos más ricos de potencia, eclipse solar y generación energética.
- [ ] Mayor realismo en orientación y control de actitud (ADCS).

---

## Lista de compra para Reyes

**Prioridad: comenzar con poco, construir mucho.** Los presupuestos son orientativos, no cotizaciones verificadas ni sumas cerradas con transporte.

### Opción A — Básica (aprox. 25–40 €)

- Raspberry Pi Pico 2 (alternativa económica a STM32).
- Protoboard.
- Cables jumper compatibles.
- LEDs, resistencias y pulsadores.
- Cable USB de datos compatible.

**Para aprender:** entradas/salidas, estados y comunicación básica con el PC.

### Opción B — Recomendada (aprox. 45–65 €)

- **STM32 NUCLEO-F446RE** (o Nucleo equivalente disponible).
- Protoboard + cables jumper.
- LEDs, resistencias y pulsadores.
- Módulo IMU tipo MPU6050.
- Sensor de corriente/tensión tipo INA219.
- Cable USB **Mini-B de datos**, compatible con la NUCLEO-F446RE.

**Para aprender:** C/C++ embebido, interrupciones, sensores, potencia y telemetría.

### Opción C — Ampliada (aprox. 70–100 €)

Todo lo anterior, más:

- Lector microSD compatible con lógica de 3,3 V.
- Tarjeta microSD.
- Pantalla OLED I²C pequeña.
- Accesorios adicionales de conexión.

**Para aprender:** persistencia, visualización local, fallos de escritura y recuperación.

### Antes de comprar

- [ ] Confirmar disponibilidad y precio final con IVA y envío.
- [ ] Comprobar conexiones, niveles de 3,3 V y compatibilidad de sensores.
- [ ] Confirmar si los módulos incluyen pines soldados o requieren soldadura.
- [ ] Evitar por ahora baterías de litio sueltas, equipos de radio y componentes de alto consumo.
- [ ] Empezar comunicando por **USB**, no mediante radio.

**Recomendación:** pedir a Ana el **kit recomendado STM32 (45–65 € aprox.)**. Lo iremos ampliando cuando cada pieza nueva tenga un experimento concreto.

---

## Próximos pasos inmediatos

1. **Retomar exactamente donde estamos:** revisar qué debe hacer `SatelliteController` al recuperar `EXECUTING` después de un reinicio.
2. Añadir una prueba pequeña que demuestre que no se toma una segunda foto automáticamente.
3. Consolidar persistencia y pruebas, sin cambiar de funcionalidad cada sesión.
4. Separar clases en archivos para que el proyecto sea mantenible.
5. Crear el reloj de simulación y empezar la parte orbital.

### Norma de trabajo

En cada sesión: **un concepto → un cambio de código → una prueba → una conclusión**. Mantendremos el código de C++ como núcleo y añadiremos simulación y hardware cuando cada fase esté preparada.

---

## Registro de hitos

| Fecha | Hito | Estado |
|---|---|---|
| Noviembre 2026 | Software básico de energía, sensores, cámara y misiones | Hecho |
| Noviembre 2026 | Persistencia y checksum educativo | Hecho |
| Noviembre 2026 | Integración inicial de `beginExecution()` | Hecho |
| Próximo hito | Recuperación segura de estado `EXECUTING` | Pendiente |
| Más adelante | Primera órbita visible | Pendiente |
| Más adelante | Primera conexión con STM32 | Pendiente |

*Documento vivo: actualizar casillas, decisiones técnicas y fechas cuando realmente se completen los hitos.*
