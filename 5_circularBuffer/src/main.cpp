/*
Five producer tasks.
      Each producer writes its task number to the circular buffer three times.
      When the buffer is not empty, it needes to be protected from being overwritten.
  
Two consumer tasks.
      The consumer task should read from the circular buffer and print the number that it has read.
      When the buffer is empty, the reading should be stopped.
*/

/*Coding notes
      1.  The reading and writing to the circular buffer should be protected with mutex.
*/

#include <Arduino.h>
#define BUFFER_SIZE 5

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else 
  static const BaseType_t app_cpu = 1;
#endif


static TaskHandle_t hConsumer   = NULL;
static TaskHandle_t hProducer   = NULL;


const uint8_t nproducers = 5;
const uint8_t nconsumers = 3;

uint8_t counter;



static SemaphoreHandle_t sem_buffer;
//This mutex ensures that the operations involving circular buffer is protected.
static SemaphoreHandle_t mut_buffer_atomic;



uint8_t circular_buffer[BUFFER_SIZE];



//This is the function called by the prdoucer tasks.
void producer_task (void *parameters){

  //cast the void pointer into integer pointer and dereference it.
  int task_number = *(int *)parameters;
  xSemaphoreGive(sem_buffer);
  
}


//This is the function called by the comsumer tasks.
void consumer_task (void *parameters){

}



void setup() {

  Serial.begin(115200);;
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println("Welcome to the circular buffer.");

  //Creation of the semaphores and mutex locks.
  sem_buffer = xSemaphoreCreateCounting(nproducers, 0);
  mut_buffer_atomic = xSemaphoreCreateMutex();




  //Errors in case the semaphore and mutex couldn't be created.
  if(sem_buffer == NULL)         {Serial.println("Semaphore creation failed: returned NULL.");}
  if(mut_buffer_atomic  == NULL) {Serial.println("Mutex creation failed: returned NULL.");}



  char producer_task_name [20];
  char consumer_task_name [20];


  //Creation of producer tasks.
  for (counter=0; counter < nproducers; counter++){
    sprintf(producer_task_name, "Producer task %d", counter);

    xTaskCreatePinnedToCore(
      producer_task, producer_task_name, 1024, (void *)&counter, 1, NULL, app_cpu
    );
  }



  //Creation of consumer tasks.
  for (counter=0; counter < nconsumers; counter++){
    sprintf(consumer_task_name, "Consumer task %d", counter);

    xTaskCreatePinnedToCore(
      consumer_task, consumer_task_name, 1024, (void *)&counter, 1, NULL, app_cpu
    );
  }






}

void loop() {

}