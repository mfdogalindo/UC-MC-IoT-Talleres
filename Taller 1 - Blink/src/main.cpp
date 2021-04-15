/**
 * @file main.cpp
 * @author Manuel Fernando Galindo Semanate (manuelgalindo@unicauca.edu.co)
 * @brief Control de LED pulsante
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>

#define PERIOD 200    // Periodo del LED

/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/** Funciones */
/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

void toggleLed(){
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(PERIOD);  
}

/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  toggleLed(); 
}
