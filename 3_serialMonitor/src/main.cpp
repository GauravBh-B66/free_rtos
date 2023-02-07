/*Task A(Ear Task): 
    Listen for input from serial monitor,
    On newline character(\n) store all characters upto that point,
    Notify task B of new message.
*/

/*Task B(Mouth Task): 
    Wait for notification form task A,
    Prints message found in heap memory to serial monitor,
    free the heap memory.
*/


#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else 
  static const BaseType_t app_cpu = 1;
#endif


static TaskHandle_t hEarTask   = NULL;
static TaskHandle_t hMouthTask = NULL;

bool notification = false;

static char *message;
char endChar;
int pos = 0, lastPos, nChar, totalLen;


//This is listening task
void earTask(void* parameter){
    while(1){

      lastPos = 0;
      totalLen = 0;
      endChar = 0;
      message= (char *)malloc(50*sizeof(char));

      while (endChar != 13){
        nChar = Serial.available();
        if (nChar > 0){
          totalLen += nChar;

          for (pos=lastPos; pos < totalLen; pos++){
            *(message+pos) = Serial.read();
          }

          lastPos = pos;
          endChar = *(message+(pos-1));
        
          if (endChar == 13){
            lastPos = 0;
            notification = true;
            vTaskResume(hMouthTask);
            vTaskDelay(100/portTICK_PERIOD_MS);
            vTaskSuspend(hMouthTask);
          } 
        }
      }
    }
  }



//This is printing task
void mouthTask(void* parameter){

  while(1){
    if (!notification){
      vTaskSuspend(NULL);
    }

    vTaskSuspend(hEarTask);
    for (pos=0; pos<(totalLen-1); pos++){
      Serial.print(*(message+pos));
    }
    Serial.println();

    totalLen = 0;
    lastPos = 0;
    notification = false;
    free(message);

    vTaskResume(hEarTask);
  }
}


void setup(){

  //Initialize the serial monitor
  Serial.begin(9600);
  vTaskDelay(1000/portTICK_PERIOD_MS);

  Serial.println();
  Serial.println("..........Serial Monitor Initialized..........");
  

  //The listening task is created
  xTaskCreatePinnedToCore(
      earTask,
      "Listening Task",
      1024,
      NULL,
      1,
      &hEarTask,
      app_cpu
  );

  //The printing task is created
  xTaskCreatePinnedToCore(
      mouthTask,
      "Printing Task",
      1024,
      NULL,
      1,
      &hMouthTask,
      app_cpu
  ); 
  vTaskDelete(NULL);
}



void loop(){

}