#include "solar.h"

#define READ_INTERVAL 100000

#define TIMER_INCREMENT_MODE (1<<30)
#define TIMER_ENABLE (1<<31)
#define TIMER_DIVIDER_VALUE 80

IRrecv irrecv(IR_PIN);  // Sets the IR_PIN to be used to receive inputs

void Task_ReadRemote(void *args);

extern void remoteSetup(){
  irrecv.enableIRIn(); // Start the receiver
  xTaskCreatePinnedToCore(Task_ReadRemote, "Read Remote Task", 4096, NULL, 1, NULL, 0);

  uint32_t timer_config = (TIMER_DIVIDER_VALUE << 13);

  timer_config |= TIMER_INCREMENT_MODE;
  timer_config |= TIMER_ENABLE;

  // TODO: Write config to timer register
  *((volatile uint32_t *)TIMG_T0CONFIG_REG(0)) = timer_config;

  // TODO: Trigger a timer update to load settings
  *((volatile uint32_t *)TIMG_T0UPDATE_REG(0)) = 1;
}

/*
 * checking for remote values, writes some data to queue implement debouncing, core 0
 */
void Task_ReadRemote(void *args){
  int prev = 0;
  decode_results results;
  static uint32_t last_toggle_time = 0;
  while (1) {
    uint32_t current_time = 0;
    current_time = *((volatile uint32_t *)TIMG_T0LO_REG(0));

    if ((current_time - last_toggle_time) >= READ_INTERVAL) {
      if (irrecv.decode(&results)) {
        if (results.value == LEFT || results.value == RIGHT || results.value == REPEAT) {
          if (results.value == LEFT) { 
            prev = 1;
          }
          if (results.value == RIGHT) {
            prev = -1;
          }
          Serial.println(prev);
          xQueueSend(remoteQueue, &prev, portMAX_DELAY);
        }
        irrecv.resume(); // Receive the next value
        last_toggle_time = current_time;
      }
    }
    *((volatile uint32_t *)TIMG_T0UPDATE_REG(0)) = 1;
  }
}