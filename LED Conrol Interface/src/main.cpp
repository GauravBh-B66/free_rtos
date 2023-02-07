#include <Arduino.h>

#if CONFIG_FREERT0S_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

static TaskHandle_t hInputTask  = NULL;
static TaskHandle_t hBlinkTask  = NULL;

const int LED    = 2;
int userRate     = 0;
int promptPeriod = 5000;

void inputTask(void* parameter){
    while(1){
        userRate = Serial.parseInt();
        Serial.println(userRate); 

        if(Serial.available() == 0){
            vTaskSuspend(NULL);
        }      
    }
}


void blinkTask(void* parameter){
    while(1){
        if(Serial.available() > 0 ){
           vTaskResume(hInputTask);
        }
        else{
            digitalWrite(LED,HIGH);
            vTaskDelay(userRate/portTICK_PERIOD_MS);
            digitalWrite(LED,LOW);
            vTaskDelay(userRate/portTICK_PERIOD_MS);
        }

    }
}

void setup(){
    pinMode(LED, OUTPUT);
    Serial.begin(300);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.println();
    Serial.println("===========Welcome to the LED control Interface.===========");
    Serial.println("===========Please enter the blinking rate===========");

    while (Serial.available() == 0){}

    
    xTaskCreatePinnedToCore(
        inputTask,
        "Input Task",
        1024,
        NULL,
        2,
        &hInputTask,
        app_cpu
    );


    xTaskCreatePinnedToCore(
        blinkTask,
        "LED Blink",
        2048,
        NULL,
        2,
        &hBlinkTask,
        app_cpu
    );
}

void loop(){

}