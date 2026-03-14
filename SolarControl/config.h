/**
 * @file config.h
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief Central configuration: pin assignments, timing constants, and
 *        declarations for all globals shared across translation units.
 */

// =========================== Includes ===========================
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// ============================ Macros ============================
#define IR_PIN    36                  ///< GPIO pin connected to the IR receiver data line.
#define IR_LEFT   0xFF10EF            ///< Left code from remote.
#define IR_RIGHT  0xFF5AA5            ///< Right code from remote.
#define IR_OK     0xFF38C7            ///< OK code from remote.
#define IR_REPEAT 0xFFFFFFFFFFFFFFFF  ///< Repeat code sent when a button is held down.

#define PHOTO_PIN 1 ///< GPIO pin connected to the photoresistor.

#define LCD_ADDRESS 0x27  ///< I2C address of the LCD
#define SDA_PIN     8     ///< I2C SDA pin.
#define SCL_PIN     9     ///< I2C SDA pin.

#define BUZZER_PIN  2   ///<  GPIO pin for the buzzer.
#define BUZZER_FREQ 200 ///< Frequency for the buzzer in Hz.
#define BUZZER_RES  12  ///< Timer resolution in bits.
#define BUZZER_DUTY 2000///< Duty cycle applied when the buzzer is active.

#define STEP_IN1        11    ///< Stepper input 1
#define STEP_IN2        12    ///< Stepper input 2
#define STEP_IN3        13    ///< Stepper input 3
#define STEP_IN4        14    ///< Stepper input 4
#define STEPS_PER_MOVE  128   ///< Number of steps driven per remote button press.
#define STEPS_FULL_REV  4096  ///< Total number of steps in one full revolution (used for auto-seek).

#define REMOTE_INTERVAL 50 ///< Minimum timer-tick gap between processed remote codes (debounce).
#define LIGHT_INTERVAL  10 ///< Minimum timer-tick gap between photoresistor samples.

#define TIMER_INCREMENT_MODE  (1 << 30) ///< Count-up mode
#define TIMER_ENABLE          (1 << 31) ///< Enable the timer
#define TIMER_DIVISOR         128       ///< Clock pre-scaler

// ======================= Global Variables =======================
extern QueueHandle_t lightQueue;   ///< Raw readings from Task_ReadLight
extern QueueHandle_t remoteQueue;  ///< Direction commands from Task_ReadRemote

/** Handle for Task_ProcessLight — needed so Task_ProcessRemote can suspend it
 *  during the automatic best-position scan. 
 */
extern TaskHandle_t processLightHandle;

extern LiquidCrystal_I2C lcd; ///< Shared lcd instance

extern int degree;  ///< Current angular position in steps [0, 4095]
extern int Steps;   ///< Current step index within the 8-step sequence [0, 7]
extern int dir;     ///< Current rotation direction: +1 = forward, -1 = reverse

extern int lightMin; //< Running minimum raw light value observed since boot.
extern int lightMax; ///< Running maximum raw light value observed since boot.
