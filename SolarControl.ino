#include "solar.h"

///< remoteTimer
QueueHandle_t lightQueue;///< lightLevelQueue
QueueHandle_t remoteQueue;///< remoteQueue

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  remoteQueue = xQueueCreate(2, sizeof(int));
  lightQueue = xQueueCreate(2, sizeof(int));

  void panelSetup();
  void remoteSetup();
  void lightSetup();
}

void loop() {}
