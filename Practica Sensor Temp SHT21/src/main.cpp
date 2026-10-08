#include <Arduino.h> 
#include <Wire.h> 
#include "SHT2x.h" 
 
const uint8_t PIN_SDA = 21; 
const uint8_t PIN_SCL = 22; 
const uint8_t PIN_LED = 26;            // LED con resistencia de 220 ohm 
const float UMBRAL_C = 28.0;           // Umbral en grados C (todavía sin usar) 
const unsigned long PERIODO_MS = 1000; // Una lectura por segundo 
 
SHT21 sht; 
 
unsigned long tAnterior = 0; 
bool ledEncendido = false; 
 
void setup() { 
  Serial.begin(115200); 
  pinMode(PIN_LED, OUTPUT); 
 
  Wire.begin(PIN_SDA, PIN_SCL);  // Inicia el bus I2C 
  if (!sht.begin()) { 
    Serial.println("Error: SHT21 no encontrado (direccion 0x40). Revisa SDA y SCL"); 
  } else { 
    Serial.println("SHT21 listo"); 
  } 
} 
 
void loop() { 
  if (millis() - tAnterior >= PERIODO_MS) { 
    tAnterior = millis(); 
 
    if (sht.read()) {                           // Lee temperatura y humedad 
      float temperatura = sht.getTemperature(); // En grados Celsius 
      Serial.print("Temperatura: "); 
      Serial.print(temperatura, 2); 
      Serial.println(" C"); 
    } else { 
      Serial.print("Error de lectura, codigo 0x"); 
      Serial.println(sht.getError(), HEX); 
    } 
 
    // Ejemplo: el LED parpadea sin mirar la temperatura 
    ledEncendido = !ledEncendido; 
    digitalWrite(PIN_LED, ledEncendido); 
  } 
} 