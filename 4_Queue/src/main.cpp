/*Task A                                          Task B
  Print new messages from Q2                      Update local variable with yyy.
  Read serial ip from user.                       Blink led with yyy delay.
  Echo back to serial terminal.                   For every 100 times of blink, send "Century !! " message to Q2.
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
static TaskHandle_t hLedInterface = NULL;


static uint8_t length_queue1 = 5;
static uint8_t length_queue2 = 1;


static QueueHandle_t queue1;
static QueueHandle_t queue2;


int counter;


void userInterface (void* parameter){
  char msg[10];


  while(1){
    //Serial.println("sub-task 1 ----> print new messages from Q2");
    Serial.println("Task 1");
    if ((xQueueReceive(queue2, msg, 0))==pdTRUE){
      Serial.print(*(msg));
      for (counter = 1; counter<9; counter++){
        xQueueReceive(queue2,(msg+counter),0);
        Serial.print(*(msg+counter));
      }
      Serial.println();
    }

       
    //Serial.println("sub-task 2 ----> Read serial input from user and echo back to serial terminal");
    char endChar=0, user_message[50];
    int nChar, totalLen, pos, lastPos=0;

    while (endChar != 13){
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



    //Serial.println("sub-task 3 -----> if delay xxx, send xxx to Q1");
    char required_message[]="delay ";
    int check_length = strlen(required_message);
    int x=0;
    bool match = false;

    for (pos=0; pos <check_length; pos++){
      if (*(user_message+pos) == *(required_message + pos)){
        match = true;
      }
      
      else{
        // Serial.print(pos);
        // Serial.println("Here");
        match = false;
        pos = check_length;
      }
    }

    if (match){
      //Serial.println("Here");      
      for (pos = check_length; pos < (check_length + strlen("xxx")); pos ++ ){
        x = (10*x) +(user_message[pos] - 48);
      }
      //Serial.println(x);
      xQueueSend(queue1,&x, 0);
    }
    //vTaskDelay(1/portTICK_PERIOD_MS);
  }
}


void ledInterface(void* parameter){
  
  char message[]="Decade!!";
  int blink = 1;
  int delay_updated, delay_ = 0;
  
  //Update local variable with xxx and Blink led with xxx delay
  while (1){
    //Serial.println("Task 2");
    if (xQueueReceive(queue1,&delay_updated,0)==true){
      delay_ = delay_updated;
    }
    digitalWrite(2, HIGH);
    vTaskDelay(delay_/portTICK_PERIOD_MS);
    digitalWrite(2, LOW);
    vTaskDelay(delay_/portTICK_PERIOD_MS); 
    blink ++;
    //Serial.println(blink);

    if(blink == 10){
      xQueueSend(queue2,&message, 0);
      //Serial.println("Here");
      blink = 1;
    }    
    //vTaskDelay(10/portTICK_PERIOD_MS);
  }
}


void setup() {

  Serial.begin(9600);
  vTaskDelay(500/portTICK_PERIOD_MS);
  //Serial.println("Setup Task");

  queue1 = xQueueCreate(length_queue1, 3*sizeof(int));
  queue2 = xQueueCreate(length_queue2, 15*(sizeof(char)));

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
  //Serial.println("Task 1 created.");


  xTaskCreatePinnedToCore(
    ledInterface,
    "LED Interface",
    1024,
    NULL,
    1,
    &hLedInterface,
    app_cpu
  );
  //Serial.println("Task 2 created.");
}

void loop() {

}