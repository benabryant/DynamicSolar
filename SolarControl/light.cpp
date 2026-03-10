#include "solar.h"

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);  ///< LCD representation

void Task_ReadLight(void *args);
void Task_ProcessLight(void *args);

extern void lightSetup(){
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  ledcAttach(BUZZER_PIN, FREQ, 12);

  xTaskCreatePinnedToCore(Task_ReadLight, "Read Light Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_ProcessLight, "Process Light Task", 4096, NULL, 1, NULL, 1);
}

/*
 * @brief add to queue lightLevelQueue, core 0
 */
void Task_ReadLight(void *args){
  // Read the value from the photoresistor and add it to the queue
  while (1) {
    int light = analogRead(PHOTO_PIN);
    xQueueSend(lightQueue, &light, portMAX_DELAY);
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
      Serial.println(light);

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
    vTaskDelay(1);
  }
}