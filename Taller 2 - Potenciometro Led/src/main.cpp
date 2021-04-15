#include <Arduino.h>

#define ADC_INTERVAL 100    //Se toman muestras de voltaje cada 100ms
#define LED_INTERVAL 1000   //Se calcula el promedio y se toman decisiones cada 1s
#define ADC_RESOLUTION 4096 //Resolución ADC de ESP32
#define ADC_VOLTAGE 3300    //Factor de conversión a mV
#define TARGET 2500         //Valor objetivo para controlar led

// the setup function runs once when you press reset or power the board
uint32_t led_current_t, led_previous_t, adc_current_t, adc_previous_t, samples, adc_acumulative, adc_value = 0;

char buffer[32] = " ";

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  analogReadResolution(ADC_RESOLUTION);
}

void loop()
{
  adc_current_t = millis();
  if (adc_current_t - adc_previous_t >= ADC_INTERVAL)
  {
    adc_previous_t = adc_current_t;
    adc_acumulative += analogRead(A0);
    samples += 1;
  }

  led_current_t = millis();
  if (led_current_t - led_previous_t >= LED_INTERVAL)
  {
    led_previous_t = led_current_t;
    adc_value = (ADC_VOLTAGE * adc_acumulative) / (ADC_RESOLUTION * samples);
    digitalWrite(LED_BUILTIN, (adc_value >= TARGET) ? HIGH : LOW);
    sprintf(buffer, "LED : %s\tSamples: %u\tADC: %umV\n", digitalRead(LED_BUILTIN) ? "ON" : "OFF", samples, adc_value);
    Serial.print(buffer);
    samples = 0;
    adc_acumulative = 0;
  }
}
