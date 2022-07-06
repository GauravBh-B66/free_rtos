#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>


#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else 
  static const BaseType_t app_cpu = 1;
#endif


static TaskHandle_t hUserInterface = NULL;
static TaskHandle_t hLedInterface = NULL;


static uint8_t length_queue1 = 5;
static uint8_t length_queue2 = 5;


static QueueHandle_t queue1;
static QueueHandle_t queue2;



void userInterface (void* parameter){

}


void ledInterface(void* parameter){

}


void setup() {

  queue1 = xQueueCreate(length_queue1, sizeof(int));
  queue2 = xQueueCreate(length_queue2, sizeof(int));

  xTaskCreatePinnedToCore(
    userInterface,
    "User Interface",
    1024,
    NULL,
    1,
    &hUserInterface,
    app_cpu
  );


  xTaskCreatePinnedToCore(
    ledInterface,
    "LED Interface",
    1024,
    NULL,
    1,
    &hLedInterface,
    app_cpu
  );



}

void loop() {

}