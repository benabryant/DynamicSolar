/**
 * @file light.cpp
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief Implementation of the ambient-light sampling and display tasks.
 */

// =========================== Includes ===========================
#include "light.h"
#include "config.h"
#include "soc/timer_group_reg.h"

// ==================== Function Implementation ===================

/**
 * @brief Update the running min/max calibration window with a new sample.
 *
 * Both bounds start at extreme values so the very first reading sets them.
 *
 * @param raw  Raw 12-bit value (0–4095).
 */
static void updateCalibration(int raw) {
    if (raw < lightMin)
        lightMin = raw;
    else if (raw > lightMax)
        lightMax = raw;
}

/**
 * @brief Convert a raw reading to a calibrated light percentage.
 *
 * Scales @p raw linearly between the observed lightMin and lightMax.
 * Returns 0 if min == max to avoid division by zero during start-up.
 *
 * @param raw  Raw 12-bit ADC value.
 * @return     Light level as an integer percentage [0, 100].
 */
static int adcToPercent(int raw) {
    int range = lightMax - lightMin;
    return (int)((float)(raw - lightMin) / (float)range * 100.0f);
}


void Task_ReadLight(void *args) {
    static uint32_t lastTime = 0;
    uint32_t now = 0;

    while (1) {
        now = *((volatile uint32_t *) TIMG_T0LO_REG(0));

        if ((now - lastTime) >= LIGHT_INTERVAL) {
            int raw = analogRead(PHOTO_PIN);
            updateCalibration(raw);
            xQueueSend(lightQueue, &raw, portMAX_DELAY);
            lastTime = now;
        }

        *((volatile uint32_t *) TIMG_T0UPDATE_REG(0)) = 1;
        vTaskDelay(1);
    }
}

void Task_ProcessLight(void *args) {
    int raw = 0;

    lcd.clear();
    lcd.print("Sunlight: ");
    lcd.setCursor(12, 0);

    while (1) {
        if (xQueueReceive(lightQueue, &raw, portMAX_DELAY) == pdTRUE) {
            int pct = adcToPercent(raw);

            lcd.setCursor(10, 0);
            lcd.print("    ");        // clear the field
            lcd.setCursor(10, 0);
            lcd.print(pct);
            lcd.print("%");

            // Buzzer: activate when light exceeds threshhold
            ledcWrite(BUZZER_PIN, (pct > 75) ? BUZZER_DUTY : 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
