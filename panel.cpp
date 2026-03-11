#include "solar.h"

const int rotations = 4096;

int degree = 0;
int Steps = 0;
int dir = 1;

void Task_MovePanel(void *args);
void stepper(int xw);
void SetSteps();
void SetDirection();

extern void panelSetup(){
  pinMode(STEP_IN1, OUTPUT);
  pinMode(STEP_IN2, OUTPUT);
  pinMode(STEP_IN3, OUTPUT);
  pinMode(STEP_IN4, OUTPUT);
  xTaskCreatePinnedToCore(Task_MovePanel, "Move Panel Task", 4096, NULL, 1, NULL, 1);
}

/*
 * Takes from queue movement information, moves motor accordingly, core 1
 */
void Task_MovePanel(void *args){
  while (1) {
    if (xQueueReceive(remoteQueue, &dir, 0) == pdTRUE) {
      for (int i = 0; i < rotations / DEGR; i++){
        stepper(1);
        vTaskDelay(1);
      }
    }
  }
}

void stepper(int xw) {
  for (int x = 0; x < xw; x++) {
    switch (Steps) {
      case 0:
        digitalWrite(STEP_IN1, LOW);
        digitalWrite(STEP_IN2, LOW);
        digitalWrite(STEP_IN3, LOW);
        digitalWrite(STEP_IN4, HIGH);
        break;
      case 1:
        digitalWrite(STEP_IN1, LOW);
        digitalWrite(STEP_IN2, LOW);
        digitalWrite(STEP_IN3, HIGH);
        digitalWrite(STEP_IN4, HIGH);
        break;
      case 2:
        digitalWrite(STEP_IN1, LOW);
        digitalWrite(STEP_IN2, LOW);
        digitalWrite(STEP_IN3, HIGH);
        digitalWrite(STEP_IN4, LOW);
        break;
      case 3:
        digitalWrite(STEP_IN1, LOW);
        digitalWrite(STEP_IN2, HIGH);
        digitalWrite(STEP_IN3, HIGH);
        digitalWrite(STEP_IN4, LOW);
        break;
      case 4:
        digitalWrite(STEP_IN1, LOW);
        digitalWrite(STEP_IN2, HIGH);
        digitalWrite(STEP_IN3, LOW);
        digitalWrite(STEP_IN4, LOW);
        break;
      case 5:
        digitalWrite(STEP_IN1, HIGH);
        digitalWrite(STEP_IN2, HIGH);
        digitalWrite(STEP_IN3, LOW);
        digitalWrite(STEP_IN4, LOW);
        break;
      case 6:
        digitalWrite(STEP_IN1, HIGH);
        digitalWrite(STEP_IN2, LOW);
        digitalWrite(STEP_IN3, LOW);
        digitalWrite(STEP_IN4, LOW);
        break;
      case 7:
        digitalWrite(STEP_IN1, HIGH);
        digitalWrite(STEP_IN2, LOW);
        digitalWrite(STEP_IN3, LOW);
        digitalWrite(STEP_IN4, HIGH);
        break;
      default:
        digitalWrite(STEP_IN1, LOW);
        digitalWrite(STEP_IN2, LOW);
        digitalWrite(STEP_IN3, LOW);
        digitalWrite(STEP_IN4, LOW);
        break;
    }
    SetDirection();
    SetSteps();
  }
}

void SetSteps() {
  Steps += dir;
  if (Steps > 7) {
    Steps = 0;
  }
  if (Steps < 0) {
    Steps = 7;
  }
}

void SetDirection() {
  degree += dir;
  if (degree >= rotations) {
    degree = 0;
  }
  if (degree < 0) {
    degree = rotations - 1;
  }
}