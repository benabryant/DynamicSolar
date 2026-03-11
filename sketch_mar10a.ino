#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "freertos/FreeRTOS.h"     // FreeRTOS kernel API
#include "freertos/task.h"         // Task creation/management
#include "freertos/queue.h"        // Queue for inter-task communication

#define IR_PIN 36
#define LEFT 0xFF10EF
#define RIGHT 0xFF5AA5
#define REPEAT 0xFFFFFFFFFFFFFFFF

#define PHOTO_PIN 1
#define LCD_ADDRESS 0x27

#define FREQ 200
#define BUZZER_PIN 2
#define SDA_PIN 8
#define SCL_PIN 9

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2); ///< LCD representation

#define STEP_IN1 11
#define STEP_IN2 12
#define STEP_IN3 13
#define STEP_IN4 14

IRrecv irrecv(IR_PIN); // Sets the IR_PIN to be used to receive inputs

decode_results results;

int degree = 0;
int Steps = 0;
int dir = 1;

QueueHandle_t lightQueue;
///< lightLevelQueue
///< remoteQueue
QueueHandle_t remoteQueue;
///< lightTimer
///< remoteTimer

/*
 * @brief add to queue lightLevelQueue, core 0
 */
void Task_ReadLight(void *args){
  while (1) {
    int light = analogRead(PHOTO_PIN);
    xQueueSend(lightQueue, &light, portMAX_DELAY);
    Serial.println(light);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/*
 * Display light level to lcd
 */
void Task_ProcessLight(void *args){
  int light = 0;
  while (1) {
    if (xQueueReceive(lightQueue, &light, 0) == pdTRUE) {
      // Displays it on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(light);

      // Triggers the buzzer if below a threshold
      if (light < 1500) {
        ledcWrite(BUZZER_PIN, 2000);
      } else {
        ledcWrite(BUZZER_PIN, 0);
      }
    }
    vTaskDelay(100);
  }
}

// Honestly, I don't think we need specific helper functions. Code is already pretty compact.

/*
 * plays buzzer
 */
void buzzer(){}

/*
 * reads from queue, calls buzzer and display, core 1
 */
void processLight(void *args){}

/*
 * checking for remote values, writes some data to queue implement debouncing, core 0
 */
void Task_ReadRemote(void *args){
  int prev = 0;
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

/*
 * Takes from queue movement information, moves motor accordingly, core 1
 */
void Task_MovePanel(void *args){
  while (1) {
    if (xQueueReceive(remoteQueue, &dir, 0) == pdTRUE) {
      for (int i = 0; i < 128; i++){
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
  if (degree > 4095) {
    degree = 0;
  }
  if (degree < 0) {
    degree = 4095;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver

  remoteQueue = xQueueCreate(2, sizeof(int));
  lightQueue = xQueueCreate(2, sizeof(int));

  pinMode(STEP_IN1, OUTPUT);
  pinMode(STEP_IN2, OUTPUT);
  pinMode(STEP_IN3, OUTPUT);
  pinMode(STEP_IN4, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  ledcAttach(BUZZER_PIN, FREQ, 12);

  xTaskCreatePinnedToCore(Task_ReadRemote, "Read Remote Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_MovePanel, "Move Panel Task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(Task_ReadLight, "Read Light Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_ProcessLight, "Process Light Task", 4096, NULL, 1, NULL, 1);
}

void loop() {}