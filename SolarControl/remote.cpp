#include "solar.h"

IRrecv irrecv(IR_PIN);  // Sets the IR_PIN to be used to receive inputs

extern void remoteSetup(){
  irrecv.enableIRIn(); // Start the receiver

}

/*
 * checking for remote values, writes some data to queue implement debouncing, core 0
 */
extern void Task_ReadRemote(void *args){
  int prev = 0;
  decode_results results;
  while (1) {
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
      // Serial.println(resultToHexidecimal(&results)); // Print code
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}