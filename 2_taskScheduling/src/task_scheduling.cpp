#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else 
  static const BaseType_t app_cpu = 1;
#endif

const char msg[] = "TaskHandle is a pointer to the Task Control Block of the program.";
int msg_len = strlen(msg);


static TaskHandle_t task_1 = NULL;      //The handles are set null so that they don't point to any memory address.
static TaskHandle_t task_2 = NULL;      //TaskHandle is a pointer to the Task Control Block(TCB) of the program.

void startTask1(void* parameter){
  
    while (1){
    for (int i= 0; i<msg_len; i++){
      Serial.print(msg[i]);
    }

  Serial.println();
  vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}


void startTask2(void* parameter){
  while (1){
    Serial.print('^');
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}



void setup(){                                                       //Setup function runs initially with the priority of 1.
  Serial.begin(300);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--------Demo-------");

  Serial.print("=====Setup and Loop task run as a task as well on core ");
  Serial.print(xPortGetCoreID());
  Serial.print (" with priority ");
  Serial.print(uxTaskPriorityGet(NULL));
  Serial.println("=====");

  xTaskCreatePinnedToCore(                                          //Since task_1 has the same priority as the setup task, the processor 
    startTask1,                                                     //processes them in the round robin fashion. When the setup function is  
    "Task 1",                                                       
    1024, 
    NULL, 
    1, 
    &task_1, 
    app_cpu
  );

  xTaskCreatePinnedToCore(                                          //When its the time for setup task to run based on round robin pattern,
    startTask2,                                                     //the setup task can add task 2 to the READY queue. Since task 2 has the 
    "Task 2",                                                       //higher priority compared to task 1 and setup task, task 2 is executed immediately. 
    1024, 
    NULL,
    2, 
    &task_2, 
    app_cpu
  );
  Serial.println();
  Serial.println("-----------------------------------------------");
}


void loop(){
  Serial.println();
  Serial.println("++++++++++++++++++++++++");
  for (int i=0; i<3; i++){
    vTaskSuspend(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }

  if(task_1 != NULL){
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}




