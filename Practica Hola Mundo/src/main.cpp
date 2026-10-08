#include <Arduino.h> 

void setup() { 
  Serial.begin(115200);  // Abre el UART0 a 115200 baudios 
  delay(1000);           // Margen para que el monitor serie conecte 
  Serial.println("Arranque correcto"); 
} 

void loop() { 
  Serial.println("Hola Mundo");  // Imprime y añade salto de línea 
  delay(1000);           // Margen para que el monitor serie conecte 
}