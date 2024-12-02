/**************************************************************************/
/*!
  
  @authors  Natalia Kuri LAG A01236378 & Julio Vivas CEM A01749879
  @file main.cpp
  @version 1.0

  Código principal para el proyecto de Dispositivos de Medición Inteligentes con respeto al
  Servicio Social AD24: Creando Tecnologías para el Football Americano - WA1058.670

*/
/**************************************************************************/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_PN532.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN    4
#define BUZZER_PIN      5

// Configuración de WiFi
const char* ssid     = "TU_SSID";
const char* password = "TU_CONTRASEÑA";

// URLs de las APIs
const char* configURL = "http://tu_api.com/config";
const char* dataURL = "http://tu_api.com/endpoint";

// Variables de configuración
float targetDistance = 100.0; // Distancia objetivo en metros
bool useNFC = false;

// Estados del dispositivo
enum DeviceState { WAITING_CONFIG, READY, COUNTDOWN, RUNNING, COMPLETED, UPLOADING };
DeviceState deviceState = WAITING_CONFIG;

// Tiempo y mediciones
unsigned long startTime = 0;
unsigned long endTime = 0;
bool measurementStarted = false;

// Configuración del MPU6050
Adafruit_MPU6050 mpu;

// Configuración del PN532 (NFC)
Adafruit_PN532 nfc(/* Configura los pines adecuados para tu módulo */);

// Configuración del Neopixel
Adafruit_NeoPixel pixels(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Variables para cálculos
float totalDistance = 0.0;
// Agrega variables adicionales según sea necesario

void setup() {
  Serial.begin(115200);

  // Inicializar Neopixel
  pixels.begin();
  pixels.show(); // Apagar todos los píxeles

  // Configurar buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Conexión a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  pixels.setPixelColor(0, pixels.Color(255, 255, 0)); // Amarillo
  pixels.show();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.println(WiFi.localIP());

  // Inicializar MPU6050
  if (!mpu.begin()) {
    Serial.println("No se encontró el MPU6050");
    while (1) yield();
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Inicializar PN532
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("No se encontró el PN532");
    useNFC = false;
  } else {
    nfc.SAMConfig();
  }

  // Obtener configuración inicial
  getConfig();
}

void loop() {
  switch (deviceState) {
    case WAITING_CONFIG:
      getConfig();
      break;
    case READY:
      deviceState = COUNTDOWN;
      break;
    case COUNTDOWN:
      countdown();
      deviceState = RUNNING;
      break;
    case RUNNING:
      if (!measurementStarted) {
        startMeasurement();
      }
      measureData();
      if (totalDistance >= targetDistance) {
        endMeasurement();
        deviceState = COMPLETED;
      }
      break;
    case COMPLETED:
      uploadData();
      deviceState = UPLOADING;
      break;
    case UPLOADING:
      deviceState = READY;
      break;
  }
}

void getConfig() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(configURL);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println("Configuración recibida: " + payload);
      // Parsear el JSON recibido y actualizar variables de configuración
      // Suponiendo que el payload es {"distance":100,"useNFC":true}
      targetDistance = 100.0; // Actualizar con el valor recibido
      useNFC = true;
      deviceState = READY;
    } else {
      Serial.println("Error al obtener la configuración");
      pixels.setPixelColor(0, pixels.Color(255, 255, 0)); // Amarillo
      pixels.show();
      delay(5000);
    }
    http.end();
  } else {
    Serial.println("No conectado a WiFi");
    pixels.setPixelColor(0, pixels.Color(255, 255, 0)); // Amarillo
    pixels.show();
    delay(5000);
  }
}

void countdown() {
  for (int i = 3; i > 0; i--) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Rojo
    pixels.show();
    delay(500);
    pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Verde
    pixels.show();
    delay(500);
  }
  tone(BUZZER_PIN, 1000); // Inicio de medición
  delay(500);
  noTone(BUZZER_PIN);
}

void startMeasurement() {
  measurementStarted = true;
  startTime = millis();
  totalDistance = 0.0;
}

void measureData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calcular distancia y otros parámetros
  float accel = sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z);
  float deltaTime = (millis() - startTime) / 1000.0; // En segundos
  totalDistance += 0.5 * accel * deltaTime * deltaTime;
  startTime = millis();
}

void endMeasurement() {
  endTime = millis();
  pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Azul
  pixels.show();
  tone(BUZZER_PIN, 1500); // Fin de medición
  delay(500);
  noTone(BUZZER_PIN);
}

void uploadData() {
  if (WiFi.status() == WL_CONNECTED) {
    pixels.setPixelColor(0, pixels.Color(128, 0, 128)); // Morado
    pixels.show();

    unsigned long totalTime = endTime - startTime;
    String jsonData = "{";
    jsonData += "\"distance\":" + String(totalDistance) + ",";
    jsonData += "\"time\":" + String(totalTime / 1000.0) + ",";
    jsonData += "\"useNFC\":" + String(useNFC ? "true" : "false");
    jsonData += "}";

    HTTPClient http;
    http.begin(dataURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println("Datos subidos correctamente: " + response);
      if (useNFC) {
        writeToNFC(jsonData);
      }
    } else {
      Serial.println("Error al subir los datos");
    }
    http.end();
  } else {
    Serial.println("No conectado a WiFi");
  }
}

void writeToNFC(String data) {
  Serial.println("Escribiendo datos en el tag NFC...");
  // Implementar escritura en el tag NFC
}
