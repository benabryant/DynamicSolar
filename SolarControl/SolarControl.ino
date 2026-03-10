#include "solar.h"

///< lightTimer
///< remoteTimer
// QueueHandle_t lightQueue;///< lightLevelQueue
QueueHandle_t remoteQueue;///< remoteQueue

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("hello");

  remoteQueue = xQueueCreate(2, sizeof(int));

  void panelSetup();
  void remoteSetup();
  void lightSetup();

  xTaskCreatePinnedToCore(Task_ReadLight, "Read Light Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_ReadRemote, "Read Remote Task", 4096, NULL, 1, NULL, 0);
  
  xTaskCreatePinnedToCore(Task_ProcessLight, "Process Light Task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(Task_MovePanel, "Move Panel Task", 4096, NULL, 1, NULL, 1);
}

void loop() {
}
