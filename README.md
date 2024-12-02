# Proyecto: Dispositivo de Medición de Rendimiento Deportivo

Este proyecto consiste en desarrollar un dispositivo compacto que integra múltiples sensores y módulos para medir y registrar datos de rendimiento deportivo. Utilizando un **ESP32**, se recopilan datos de movimiento, ubicación y se permite la interacción a través de **NFC** para registrar resultados. Además, se complementa con una aplicación (web o móvil) con almacenamiento en una base de datos para mantener un registro de los resultados.

## Contenido

- [Componentes](#componentes)
  - [Hardware](#hardware)
  - [Software](#software)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Instalación y Configuración](#instalación-y-configuración)
- [Uso](#uso)
- [Conexiones](#conexiones)
- [Autores](#autores)
- [Licencia](#licencia)

## Componentes

### Hardware

- **ESP32 WROOM 32D**: Microcontrolador con Bluetooth BLE y Wi-Fi integrado.
- **MPU6050 / MPU9250**: Acelerómetro y giroscopio para medir el movimiento.
- **PN532**: Sensor de proximidad NFC para interacción y registro de resultados.
- **KY-009**: LED RGB Neopixel para indicaciones visuales.
- **KY-006**: Buzzer pasivo para alertas sonoras.

### Software

- **Framework**: Arduino con la extensión de PlatformIO.
- **Entorno de Desarrollo**: Visual Studio Code.
- **Librerías Utilizadas**:
  - [Adafruit MPU6050](https://github.com/adafruit/Adafruit_MPU6050)
  - [Adafruit PN532](https://github.com/adafruit/Adafruit-PN532)
  - [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
  - [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)
  - [MPU9250](https://github.com/bolderflight/MPU9250)

## Estructura del Proyecto

El proyecto tiene la siguiente estructura:

```
Proyecto-SS-AgoDic2024/
├── .gitignore
├── include/
├── lib/
├── src/
│   └── main.cpp
├── test/
├── platformio.ini
└── README.md
```

## Instalación y Configuración

1. **Clonar el repositorio**:

   ```bash
   git clone https://github.com/Dino-Julius/Proyecto-SS-AgoDic2024.git
   ```

2. **Instalar PlatformIO**: Asegúrate de tener instalado [PlatformIO](https://platformio.org/) en tu Visual Studio Code.

3. **Instalar las dependencias**: Al abrir el proyecto en PlatformIO, las librerías necesarias se instalarán automáticamente según el archivo platformio.ini.

4. **Configurar credenciales Wi-Fi**:

   En el archivo main.cpp, configura tu SSID y contraseña de Wi-Fi:

   ```cpp
   const char* ssid     = "TU_SSID";
   const char* password = "TU_CONTRASEÑA";
   ```

5. **Configurar URLs de las APIs**:

   También en  main.cpp, configura las URLs para las APIs utilizadas:

   ```cpp
   const char* configURL = "http://tu_api.com/config";
   const char* dataURL = "http://tu_api.com/endpoint";
   ```

## Uso

1. **Compilar y cargar el código**: Utiliza PlatformIO para compilar y cargar el código al ESP32.

2. **Calibración e inicialización**:

   - El dispositivo intentará conectarse a la red Wi-Fi configurada.
   - Inicializará los sensores (MPU6050, PN532) y módulos (Neopixel, buzzer).

3. **Funcionamiento**:

   - **Modo Espera**: El dispositivo espera la configuración desde la API.
   - **Listo**: Una vez recibida la configuración, está listo para iniciar la medición.
   - **Cuenta Regresiva**: Comienza una cuenta regresiva con indicaciones visuales y sonoras.
   - **Medición**: Registra datos de movimiento y calcula la distancia recorrida.
   - **Completado**: Indica el fin de la medición y muestra los resultados.
   - **Subida de Datos**: Envía los datos recopilados a la API para almacenarlos.

4. **Interacción NFC**:

   - Si está habilitado, el dispositivo interactúa con etiquetas NFC para registrar resultados o configuraciones.

## Conexiones

| Componente              | Pin del ESP32        | Notas                                                                  |
|-------------------------|----------------------|------------------------------------------------------------------------|
| **MPU6050** (I2C)       | SDA - GPIO 21        | Bus I2C (compartido).                                                  |
|                         | SCL - GPIO 22        | Bus I2C (compartido).                                                  |
|                         | VCC - 3.3V           | Algunos módulos MPU6050 pueden requerir 5V; verifica las especificaciones. |
|                         | GND - GND            | Conectar a tierra común.                                               |
| **PN532** (I2C)         | SDA - GPIO 21        | Bus I2C (compartido). Asegúrate de configurar el PN532 en modo I2C.    |
|                         | SCL - GPIO 22        | Bus I2C (compartido).                                                  |
|                         | VCC - 3.3V           | Verifica si el módulo soporta 3.3V o requiere 5V.                      |
|                         | GND - GND            | Conectar a tierra común.                                               |
| **Neopixel** (KY-009)   | DIN - GPIO 4         | Control del LED RGB.                                                   |
|                         | VCC - 5V             | Alimentación de 5V para el Neopixel.                                   |
|                         | GND - GND            | Conectar a tierra común.                                               |
| **Buzzer Pasivo** (KY-006)| Señal - GPIO 5      | Control del buzzer mediante PWM o tono.                                |
|                         | VCC - 5V (si aplica) | Algunos buzzers usan solo señal y GND.                                 |
|                         | GND - GND            | Conectar a tierra común.                                               |

**Notas detalladas:**

- **Bus I2C (MPU6050 y PN532):**
  - Ambos dispositivos compartirán los mismos pines SDA y SCL (GPIO 21 y GPIO 22).
  - Es posible que necesites resistencias pull-up de 4.7kΩ o 10kΩ en las líneas SDA y SCL si no están incorporadas en los módulos.
  - Asegura que las direcciones I2C de los dispositivos no colisionen.

- **Alimentación:**
  - **MPU6050:** Funcionan típicamente con **3.3V**, pero algunos módulos están diseñados para **5V**. Verifica el voltaje de operación específico de tu módulo.
  - **PN532:** Puede operar a **3.3V o 5V** dependiendo del fabricante. Es recomendable usar **3.3V** para compatibilidad con el ESP32.
  - **Neopixel (KY-009):** Requiere **5V** para VCC. El pin de datos (DIN) puede recibir señal de 3.3V, pero para mayor fiabilidad, puede ser necesario un adaptador de nivel lógico.
  - **Buzzer Pasivo (KY-006):** Generalmente funciona con **5V**, pero puede operar con **3.3V**. Conecta la señal al GPIO y comprueba el nivel de sonido.

- **Neopixel (KY-009):**
  - **Condensador:** Se recomienda colocar un condensador electrolítico de **1000 µF, 6.3V o mayor** entre VCC y GND cerca del Neopixel.
  - **Resistencia en la línea de datos:** Añade una resistencia de **300-500Ω** en serie entre el GPIO del ESP32 y el pin DIN del Neopixel para limitar la corriente.

- **Buzzer Pasivo (KY-006):**
  - Al ser pasivo, necesitarás generar una señal PWM para producir sonido.

**Conexiones resumidas:**

- **ESP32 GPIO 21 (SDA) ↔ SDA del MPU6050 y PN532**
- **ESP32 GPIO 22 (SCL) ↔ SCL del MPU6050 y PN532**
- **ESP32 GPIO 4 (NEOPIXEL_PIN) ↔ DIN del Neopixel (KY-009)**
- **ESP32 GPIO 5 (BUZZER_PIN) ↔ Señal del Buzzer Pasivo (KY-006)**
- **3.3V del ESP32 ↔ VCC del MPU6050 y PN532**
- **5V del ESP32 o fuente externa ↔ VCC del Neopixel y Buzzer**
- **GND del ESP32 ↔ GND de todos los componentes**

**Diagrama simplificado:**

```
ESP32            Componentes
------           ------------
GPIO 21 -------- SDA (MPU6050 y PN532)
GPIO 22 -------- SCL (MPU6050 y PN532)
GPIO 4  -------- DIN (Neopixel KY-009) [con resistencia de 300-500Ω en serie]
GPIO 5  -------- Señal (Buzzer Pasivo KY-006)
3.3V    -------- VCC (MPU6050 y PN532)
5V      -------- VCC (Neopixel y Buzzer)
GND     -------- GND (Todos los componentes)
```

**Consideraciones adicionales:**

- **Niveles lógicos:**
  - El ESP32 trabaja con niveles lógicos de 3.3V. Al conectar dispositivos que operan a 5V, ten cuidado de no aplicar 5V directamente a las entradas del ESP32.
  - Para el Neopixel, si experimentas problemas con la señal, utiliza un convertidor de nivel lógico de 3.3V a 5V en la línea de datos.

- **Alimentación:**
  - Si alimentas el Neopixel y el Buzzer con 5V, asegúrate de que la fuente de alimentación puede proporcionar suficiente corriente. Los Neopixels pueden consumir hasta 60mA por LED a máximo brillo.
  - Considera usar una fuente de alimentación externa si la corriente total excede lo que puede suministrar el regulador del ESP32.

- **Bus I2C:**
  - Verifica las direcciones I2C de los dispositivos. El MPU6050 suele tener la dirección **0x68** o **0x69**, y el PN532 puede tener **0x24** o **0x48** dependiendo de la configuración.
  - Si hay conflicto de direcciones, es posible que necesites cambiar la dirección de uno de los dispositivos si es posible.

- **Configuración del PN532:**
  - Asegúrate de configurar los switches o jumpers del PN532 para operar en modo I2C. Algunos módulos tienen configuraciones para SPI, HSU (UART) o I2C.

- **Resistencias Pull-up en I2C:**
  - Si ninguno de los módulos tiene resistencias pull-up integradas, conecta resistencias de **4.7kΩ** desde SDA a 3.3V y desde SCL a 3.3V.

**Ejemplo de conexión del Neopixel con resistencia y condensador:**

```
[ESP32 GPIO 4] -- [470Ω] -- DIN (Neopixel)
                                   |
                                 GND
                                 VCC -- [Condensador 1000µF] -- GND
```

Recuerda siempre verificar las especificaciones técnicas de los módulos que estás utilizando, ya que pueden variar según el fabricante. Si tienes esquemas o modelos específicos de los componentes, puede ser útil consultarlos para confirmar las conexiones.

Si necesitas más ayuda o tienes alguna duda sobre las conexiones, no dudes en preguntar.

## Autores

- **Natalia Kuri LAG A01236378**
- **Julio Vivas CEM A01749879**

Proyecto desarrollado como parte del Servicio Social AD24: Creando Tecnologías para el Football Americano - WA1058.670.

## Licencia

Este proyecto está bajo la Licencia MIT. Consulte el archivo LICENSE para más detalles.