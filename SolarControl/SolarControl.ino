/**
 * @file SolarControl.ino
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief Main file for solar panel controller
 *
 * Defines all shared globals declared `extern` in config.h, initialises
 * peripherals, configures the ESP32 hardware timers, and launches the four
 * FreeRTOS tasks that run the system.
 */

// =========================== Includes ===========================
#include "config.h"
#include "stepper.h"
#include "light.h"
#include "remote.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "driver/gpio.h"
#include "soc/timer_group_reg.h"

// ======================= Global Variables =======================
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

QueueHandle_t lightQueue;
QueueHandle_t remoteQueue;
TaskHandle_t processLightHandle;

int degree = 0;
int Steps = 0;
int dir = 1;

// Initialised to extremes so the first real ADC sample sets both bounds.
int lightMin = 4095;
int lightMax = 0;

IRrecv        irrecv(IR_PIN);
decode_results results;

// ==================== Function Implementation ===================

/**
 * @brief Configure and start hardware timer groups 0 and 1.
 *
 * Both timers are set to count-up mode with a divisor pre-scaler,
 * giving a tick rate of (CLK / TIMER_DIVISOR).  The same configuration
 * is applied to both groups so Task_ReadLight and Task_ReadRemote each
 * have an independent free-running counter.
 */
static void initTimers() {
    uint32_t config = 0;
    config |= ((uint32_t)TIMER_DIVISOR << 13);
    config |= TIMER_INCREMENT_MODE;
    config |= TIMER_ENABLE;

    *((volatile uint32_t *) TIMG_T0CONFIG_REG(0)) = config;
    *((volatile uint32_t *) TIMG_T0CONFIG_REG(1)) = config;

    // Latch the initial counter values
    *((volatile uint32_t *) TIMG_T0UPDATE_REG(0)) = 1;
    *((volatile uint32_t *) TIMG_T0UPDATE_REG(1)) = 1;
}


void setup() {
    Serial.begin(115200);

    // IR receiver
    irrecv.enableIRIn();

    // FreeRTOS inter-task queues
    remoteQueue = xQueueCreate(2, sizeof(int));
    lightQueue  = xQueueCreate(2, sizeof(int));

    // Stepper pins
    pinMode(STEP_IN1, OUTPUT);
    pinMode(STEP_IN2, OUTPUT);
    pinMode(STEP_IN3, OUTPUT);
    pinMode(STEP_IN4, OUTPUT);

    // I2C LCD
    Wire.begin(SDA_PIN, SCL_PIN);
    lcd.init();
    lcd.backlight();

    // Passive buzzer
    ledcAttach(BUZZER_PIN, BUZZER_FREQ, BUZZER_RES);

    // Hardware timers
    initTimers();

    // Launch FreeRTOS tasks
    // processLightHandle is stored so Task_ProcessRemote can suspend it
    // during the auto scan.
    xTaskCreatePinnedToCore(Task_ReadRemote,    "ReadRemote",    4096, NULL, 1, NULL,                 0);
    xTaskCreatePinnedToCore(Task_ProcessRemote, "ProcessRemote", 4096, NULL, 1, NULL,                 1);
    xTaskCreatePinnedToCore(Task_ReadLight,     "ReadLight",     4096, NULL, 1, NULL,                 0);
    xTaskCreatePinnedToCore(Task_ProcessLight,  "ProcessLight",  4096, NULL, 1, &processLightHandle,  1);
}

/** @brief Unused — all work is done in FreeRTOS tasks. */
void loop() {}