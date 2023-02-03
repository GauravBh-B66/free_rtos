/*Task A                                          Task B
  Print new messages from Q2                      Update local variable with yyy.
  Read serial ip from user.                       Blink led with yyy delay.
  Echo back to serial terminal.                   For every 10 times of blink, send "Great Run" message to Q2.
  If "delay yyy", send yyy to Q1.              

*/
//        Serial.println("Here");




#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else 
  static const BaseType_t app_cpu = 1;
#endif


static TaskHandle_t hUserInterface = NULL;
static TaskHandle_t hLedInterface  = NULL;


static uint8_t length_delay_queue   = 5;
static uint8_t length_message_queue = 1; 


static QueueHandle_t delay_queue;
static QueueHandle_t message_queue;

int counter;


void userInterface (void* parameter){
  char msg[20];

  while(1){
      
    //Serial.println("sub-task 2 ----> Read serial input from user and echo back to serial terminal");
    char endChar=0, user_message[50];
    int nChar, totalLen, pos, lastPos=0;

    while (endChar != 13){

      if ((xQueueReceive(message_queue, msg, 0))==true){
        xQueueReceive(message_queue, msg, 0);
        Serial.print(*(msg));
        for (counter = 1; counter<strlen("Great Run!!"); counter++){
          xQueueReceive(message_queue,(msg+counter),0);
          Serial.print(*(msg+counter));
        }
        Serial.println();
      }

      nChar = Serial.available();
      if (nChar > 0){
        totalLen += nChar;
        for (pos=lastPos; pos < totalLen; pos++){
          *(user_message+pos) = Serial.read();
          Serial.print(*(user_message+pos));
        }

        lastPos = pos;
        endChar = *(user_message+(pos-1));
      }
      if (endChar==13){
        Serial.println();
        lastPos=0;
        totalLen=0;
      }
    }

    char required_message[]="delay ";
    int check_length = strlen(required_message);
    int x;
    bool match = false;

    for (pos=0; pos <check_length; pos++){
      if (*(user_message+pos) == *(required_message + pos)){
        match = true;
      }
      
      else{
        match = false;
        pos = check_length;
      }
    }

    if (match){  
      x = 0;    
      for (pos = check_length; pos < (check_length + strlen("xxx")); pos ++ ){
        x = (10*x) +(user_message[pos] - 48);
      }
      xQueueSend(delay_queue,&x, 0);
    }
  }
}


void ledInterface(void* parameter){
  
  int delay_updated, delay_ = 500;
  int blink;
  
  //Update local variable with xxx and Blink led with xxx delay
  while (1){
    if (xQueueReceive(delay_queue, &delay_updated, 0)==pdTRUE){
      delay_ = delay_updated;
      Serial.println(delay_);
      blink = 0;
    }
    digitalWrite(2, HIGH);
    vTaskDelay(delay_/portTICK_PERIOD_MS);
    digitalWrite(2, LOW);
    vTaskDelay(delay_/portTICK_PERIOD_MS); 
    blink ++;

    if(blink == 10){
      char message[] = "Great Run!!";  
      if ((xQueueSend(message_queue, message, 0)) != pdTRUE){
        Serial.println("Message Queue Full..");
      };
      blink = 0;
    }    
  }
}


void setup() {

  Serial.begin(9600);
  vTaskDelay(500/portTICK_PERIOD_MS);

  delay_queue = xQueueCreate(length_delay_queue, 3*sizeof(int));
  message_queue = xQueueCreate(length_message_queue, 25*(sizeof(char)));

  pinMode(2, OUTPUT);

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