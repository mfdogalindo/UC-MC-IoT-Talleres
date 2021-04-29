/**
 * @file main.cpp
 * @author Manuel Fernando Galindo Semanate (manuelgalindo@unicauca.edu.co)
 * @brief Controla LED según valor de voltaje medido en conversor ADC
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"
#define ADC_INTERVAL 100    //Se toman muestras de voltaje cada 100ms
#define LED_INTERVAL 1000   //Se calcula el promedio y se toman decisiones cada 1s
#define TX_INTERVAL 30000    //Periodo para transmitir datos
#define ADC_RESOLUTION 4096 //Resolución ADC de ESP32
#define ADC_VOLTAGE 3300    //Factor de conversión a mV
#define TARGET 2500         //Valor objetivo para controlar led


unsigned long channelID = 123;
const char * readAPIKey = "ABCD";
const char * writeAPIKey = "ABCD";
WiFiClient client;

//Definición de variables
uint32_t led_current_t, led_previous_t, adc_current_t, adc_previous_t, tx0_current_t, tx0_previous_t, samples, adc_acumulative = 0;
int channel_tx=1;
uint32_t v_result;
boolean tx_now = false;
char buffer[64] = " ";

/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
/** Funciones */
/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

/**
 * @brief Obtiene una muestra del conversor ADC
 * 
 * @param adcaccum Variable donde se acumula la medición
 * @param adcsamples Variable donde se registran las muestras tomadas
 */
void getSample(uint32_t *adcaccum, uint32_t *adcsamples)
{
  adc_current_t = millis();
  if (adc_current_t - adc_previous_t >= ADC_INTERVAL)
  {
    adc_previous_t = adc_current_t;
    *adcaccum += analogRead(A0);
    *adcsamples += 1;
  }
}

/**
 * @brief Imprime estado de LED y medición de voltaje 
 * 
 * @param adcsamples Muestras tomadas por ADC
 * @param result Voltaje medido (mV)
 */
void printResults(uint32_t adcsamples, uint32_t voltage)
{
  digitalWrite(LED_BUILTIN, (voltage >= TARGET) ? HIGH : LOW);
  sprintf(buffer, "LED : %s\tSamples: %u\tADC: %umV\n", digitalRead(LED_BUILTIN) ? "ON" : "OFF", adcsamples, voltage);
  Serial.print(buffer);
}

/**
 * @brief Calcula promedio de ADC y lo transforma a mV, controla LED según parámetro
 * 
 * @param adcaccum Variable con valor acumulado en las muestras de ADC
 * @param adcsamples Variable con muestras tomadas por ADC al momento de calcular
 * @return uint32_t Valor de voltaje (mV) calculado del conversor ADC
 */
uint32_t controlLed(uint32_t *adcaccum, uint32_t *adcsamples)
{
  led_current_t = millis();
  if (led_current_t - led_previous_t >= LED_INTERVAL)
  {
    led_previous_t = led_current_t;
    v_result = (ADC_VOLTAGE * *adcaccum) / (ADC_RESOLUTION * *adcsamples);
    printResults(*adcsamples, v_result);
    *adcsamples = 0;
    *adcaccum = 0;
  }
  return v_result;
}

/**
 * Envía los datos a thinkspeak
 * @param voltage Voltaje medido
 */
void sendData(uint32_t voltage)
{
  tx0_current_t = millis();
  if (tx0_current_t - tx0_previous_t >= TX_INTERVAL)
  {
    channel_tx++;
    tx0_previous_t = tx0_current_t;
    int tempCh = 1 + channel_tx%2;
    int valueTx = (tempCh == 1) ? digitalRead(LED_BUILTIN) : voltage;
    int result = ThingSpeak.writeField(channelID, tempCh, valueTx, writeAPIKey);
    sprintf(buffer, "TX: %d\t Status - [%d]: %d\n", valueTx, tempCh, result);
    Serial.print(buffer);
  }
}

/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  analogReadResolution(ADC_RESOLUTION);
  Serial.println("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  ThingSpeak.begin(client);
}

void loop()
{
  getSample(&adc_acumulative, &samples);
  controlLed(&adc_acumulative, &samples);
  sendData(v_result);
}
