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
        2.  The writing to the buffer should be stopped when there is no free location.
        3.  This can be done with semaphores.
        3.  The writing should continue when an empty slot is available.
  */

  #include <Arduino.h>
  #define BUFFER_SIZE 5

  #if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
  #else 
    static const BaseType_t app_cpu = 1;
  #endif


  static TaskHandle_t hConsumer   = NULL;
  // static TaskHandle_t hProducer   = NULL;


  const uint8_t nProducers = 5;
  const uint8_t nConsumers = 3;
  const uint8_t nWrite     = 3;


  //This semaphore makes sure that the tasks read the parameters passed from setup() successfully.
  static SemaphoreHandle_t sem_tasks;

  //This is a semaphore to control write access to the buffer.
  static SemaphoreHandle_t sem_write_buffer;

  //This is a semaphore to control read access to the buffer.
  static SemaphoreHandle_t sem_read_buffer;

  //This mutex ensures that the operations involving circular buffer is protected.
  static SemaphoreHandle_t mut_buffer_atomic;


  //The circular buffer has two indexes:
  //Head: The position from where the data is read.
  //Tail: The position where the data is added.
  uint8_t circular_buffer[BUFFER_SIZE] = {0};
  uint8_t write_index = 0;
  uint8_t read_index = 0;


  //This is the function called by the producer tasks.
  void producer_task (void *parameters){

    uint8_t counter;
    //cast the void pointer into integer pointer and dereference it.
    int task_number = *((int *)parameters);
    xSemaphoreGive(sem_tasks);

    while (true){
  /*  
      First, the mutex should be taken so that the process of writing to the buffer is atomic.
      When the task is allowed to write to the buffer, it should wait for semaphore to make sure
        that memory locations are not overwritten.
      
  */
      for (counter = 0; counter < nWrite; counter++){
        xSemaphoreTake(sem_write_buffer, portMAX_DELAY);
        if((xSemaphoreTake(mut_buffer_atomic,portMAX_DELAY)) == pdPASS){
          circular_buffer[write_index] = task_number;   
          write_index = (write_index+1)%BUFFER_SIZE;
          xSemaphoreGive(mut_buffer_atomic);
        }
        xSemaphoreGive(sem_write_buffer);
        xSemaphoreGive(sem_read_buffer);
      }
      vTaskDelete(NULL);
    }
    
  }


  //This is the function called by the consumer tasks.
  void consumer_task (void *parameters){
    while (true){
      xSemaphoreTake(sem_read_buffer, portMAX_DELAY);
      xSemaphoreTake(mut_buffer_atomic, portMAX_DELAY);

      Serial.println(circular_buffer[read_index]);
      read_index = (read_index+1)%BUFFER_SIZE;

      xSemaphoreGive(mut_buffer_atomic);
    }
  }

  void check(void){
    Serial.println("Check pass.");
  }

  void setup() {

    int counter;

    Serial.begin(115200);;
    vTaskDelay(1000/portTICK_PERIOD_MS);
    Serial.println("Welcome to the circular buffer.");


    //Creation of the semaphores and mutex locks.
    sem_write_buffer = xSemaphoreCreateCounting(nProducers, nProducers);
    sem_read_buffer = xSemaphoreCreateCounting(nConsumers, 0);
    sem_tasks  = xSemaphoreCreateCounting(nProducers, 0);
    mut_buffer_atomic = xSemaphoreCreateMutex();



    //Errors in case the semaphore and mutex couldn't be created.
    if(sem_write_buffer == NULL)   {Serial.println("Semaphore creation failed: returned NULL.");}
    if(mut_buffer_atomic  == NULL) {Serial.println("Mutex creation failed: returned NULL.");}



    char producer_task_name [20];
    char consumer_task_name [20];


    //Creation of producer tasks.
    for (counter=0; counter < nProducers; counter++){
      sprintf(producer_task_name, "Producer task %d", counter);

      xTaskCreatePinnedToCore(
        producer_task, producer_task_name, 1024, (void *)&counter, 1, NULL, app_cpu
      );
    }


    //Verification that all the producers are created.
    for (counter=0; counter < nProducers; counter ++){
      xSemaphoreTake(sem_tasks, portMAX_DELAY);
    }
    Serial.println("All producers tasks created successfully.");



    //Creation of consumer tasks.
    for (counter=0; counter < nConsumers; counter++){
      sprintf(consumer_task_name, "Consumer task %d", counter);

      xTaskCreatePinnedToCore(
        consumer_task, consumer_task_name, 1024, NULL, 1, NULL, app_cpu
      );
    }

    // for (counter = 0; counter < (nProducers*nConsumers); counter++){
    //   xSemaphoreTake(sem_read_buffer, portMAX_DELAY);
    // }
  }

  void loop() {

  }