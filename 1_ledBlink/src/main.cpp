#include <Arduino.h>


#if CONFIG_FREERTOS_UNICODE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif


static const int led_pin1 = 2;
static const int led_pin2 = 4;
static const int delay1  = 200;
static const int delay2  = 200;



void toggleLED1(void *parameter){
  while (1){
    digitalWrite(led_pin1, HIGH);
    vTaskDelay(delay1/portTICK_PERIOD_MS);
    digitalWrite(led_pin1, LOW);
    vTaskDelay(delay1/portTICK_PERIOD_MS);
  }
}

void toggleLED2(void *parameter){
  while (1){
    digitalWrite(led_pin2, HIGH);
    vTaskDelay(delay1/portTICK_PERIOD_MS);
    digitalWrite(led_pin2, LOW);
    vTaskDelay(delay2/portTICK_PERIOD_MS);
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(led_pin1, OUTPUT);
  pinMode(led_pin2, OUTPUT);


  xTaskCreatePinnedToCore(
    toggleLED1,
    "Task to Toggle LED1",
    1024,
    NULL,
    1,
    NULL,
    app_cpu
   );

  xTaskCreatePinnedToCore(
    toggleLED2,
    "Task to Toggle LED2",
    1024,
    NULL,
    1,
    NULL,
    app_cpu
   );


}

void loop() {
  // put your main code here, to run repeatedly:
}