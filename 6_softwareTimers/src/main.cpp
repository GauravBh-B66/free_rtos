/*
  Consider the LED on the ESP-32 to be a backlight for a screen.
  The program should echo the text entered into the serial console.
  The LED should stay lit when the text is being echoed in the console.
  The LED should be off when there is inactivity for more than 5 seconds.
*/

#include <Arduino.h>
#define BUILTIN_LED 2
#define DELAY_IN_MS 2000

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static TaskHandle_t thConsole     = NULL;
static TaskHandle_t thLedControl  = NULL;
static TimerHandle_t tihTimer     = NULL;


void check(void){
  Serial.println("Check pass.");
}

// This is the function called to print in the console.
void taskConsole(void *parameters){
  char c;
  while(1){
    if (Serial.available() == 0){
    }
    else{
      c = Serial.read();
      Serial.print(c);
      digitalWrite(BUILTIN_LED, HIGH);
      xTimerReset(tihTimer, portMAX_DELAY);
    }
  }
}


//This is the callback function for the timer.
//This function is called when the timer expires.
//Control the LED through this function.
void callbackTimer( TimerHandle_t xTimer){
  //Task to perform when the timer expires.
  digitalWrite(BUILTIN_LED, LOW);
}

void setup(){
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("======This is your serial console.======");

  xTaskCreatePinnedToCore(taskConsole, "Console task", 1024, NULL, 1, NULL, app_cpu);
  tihTimer = xTimerCreate("LED Timer",DELAY_IN_MS/portTICK_PERIOD_MS, 0, NULL, callbackTimer);

  //Verify that the timer is created.
  if (NULL == tihTimer){
    Serial.println("The timer couldn't be created.");
    Serial.println("Continuing without the timer.");
  }

  vTaskDelete(NULL);
}

void loop(){
  // check();
}