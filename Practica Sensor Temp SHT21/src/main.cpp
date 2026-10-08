#include <Arduino.h> 
#include <Wire.h> 
#include "SHT2x.h" 

const uint8_t PIN_SDA = 21; 
const uint8_t PIN_SCL = 22; 
const uint8_t PIN_LED = 26;                 // LED con resistencia de 220 ohm 
const float UMBRAL_high = 28.0;             // Umbral de temperatura máxima para encender/apagar el LED
const float UMBRAL_low = 15.0;              // Umbral de temperatura mínima para encender/apagar el LED
const unsigned long PERIODO_MS = 1000;      // Una lectura por segundo 
 
SHT21 sht; 
 
unsigned long tAnterior = 0; 
int ledParpadeando = 0;                     // 0 es apagado, 1 parpadeo por calor y 2 por frío. Encendido es error
bool sensorConectado = false;

float temperatura = 0.0;               
unsigned long tAnteriorParpadeo = 0;   
bool estadoLed = false;                
 
void clearDisplay() {                       // He investigado y PlatformIO no tiene una función para limpiar la pantalla del monitor serie,
  Serial.print("\e[2J\e[H");                // así que he usado un comando ANSI para limpiar la pantalla y mover el cursor a la posición inicial.
}

void setup() { 
  Serial.begin(115200); 
  pinMode(PIN_LED, OUTPUT); 
 
  Wire.begin(PIN_SDA, PIN_SCL);             // Inicia el bus I2C 
  if (!sht.begin()) { 
    Serial.println("Error: SHT21 no encontrado (direccion 0x40). Revisa SDA y SCL"); 
  } else { 
    sensorConectado = true;
    Serial.println("SHT21 listo"); 
  } 
} 
 
void loop() { 
  
  unsigned long tiempoActual = millis();    // Captura del tiempo actual al inicio de cada ciclo

  if (!sensorConectado) {                   // Si el sensor no está conectado, intentamos reconectarlo y ENCIENDIDO CONSTANTE del LED
    clearDisplay();                         // Limpia la pantalla para mostrar el error
    digitalWrite(PIN_LED, HIGH);            // LED de error encendido de forma fija
    Serial.println("Error: Sensor no conectado. Reintentando...");
    
    // Código para intentar revivir el SHT21
    Wire.end();
    Wire.begin(PIN_SDA, PIN_SCL);
    if (sht.begin()) {
      sensorConectado = true;
      Serial.println("¡Sensor recuperado con éxito!");
    }

    delay(1000);                            // Espera 1 segundo para poder leer si se ha recuperado y no saturar el puerto serie con mensajes de error
    return;                                 // Salta el resto del loop
  }

  if (sensorConectado && tiempoActual - tAnterior >= PERIODO_MS) { // Temporizador de lectura del sensor
    tAnterior = tiempoActual;               // Reseteamos el contador de tiempo de lectura del sensor
 
    if (sht.read()) {
      temperatura = sht.getTemperature();   // Lee temperatura en Celsius

      if (temperatura >= UMBRAL_high) {     // El estado del LED depende de la lectura del sensor.
        ledParpadeando = 1;                 // Parpadeo por calor
      } else if (temperatura <= UMBRAL_low) {
        ledParpadeando = 2;                 // Parpadeo por frío
      } else {
        ledParpadeando = 0;                 // Apagado
      }

      Serial.print("Temperatura: "); 
      Serial.print(temperatura, 2); 
      Serial.println(" C"); 
      Serial.print("Estado del LED: ");
      Serial.println((ledParpadeando == 0) ? "Apagado" : (ledParpadeando == 1) ? "Parpadeo por calor" : "Parpadeo por frío"); // Ahorro de IFs con ternario

    } 
    else { 
      Serial.print("Error de lectura, codigo 0x"); 
      Serial.println(sht.getError(), HEX); 
      sensorConectado = false;              // Si falla la lectura en vivo, disparamos el modo error
    }
  }
    
  
  if (ledParpadeando == 0) {                // Temporizador de parpadeo                
    digitalWrite(PIN_LED, LOW);             // Si Temp en rango, forzamos LOW
    estadoLed = false;
  } 
  else {
    unsigned long intervalo = (ledParpadeando == 1) ? 200 : 600; // Intervalos de parpadeo según el estado (200ms para calor, 600ms para frío)

    if (tiempoActual - tAnteriorParpadeo >= intervalo) {
      tAnteriorParpadeo = tiempoActual;
      estadoLed = !estadoLed;
      digitalWrite(PIN_LED, estadoLed);
    }
  }
}
