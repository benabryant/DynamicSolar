/**
 * @file stepper.cpp
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief Driver implementation for the stepper motor.
 */

// =========================== Includes ===========================
#include "stepper.h"
#include "config.h"

// ======================= Global Variables =======================
/**
 * @brief Step phase table.
 *
 * Each row corresponds to one phase (Steps 0–7) and contains the logic
 * levels to apply to [IN1, IN2, IN3, IN4] respectively.
 */
static const uint8_t STEP_SEQ[8][4] = {
    {LOW,  LOW,  LOW,  HIGH},  ///< Phase 0
    {LOW,  LOW,  HIGH, HIGH},  ///< Phase 1
    {LOW,  LOW,  HIGH, LOW },  ///< Phase 2
    {LOW,  HIGH, HIGH, LOW },  ///< Phase 3
    {LOW,  HIGH, LOW,  LOW },  ///< Phase 4
    {HIGH, HIGH, LOW,  LOW },  ///< Phase 5
    {HIGH, LOW,  LOW,  LOW },  ///< Phase 6
    {HIGH, LOW,  LOW,  HIGH},  ///< Phase 7
};

// ==================== Function Implementation ===================
void stepper(int xw) {
    for (int x = 0; x < xw; x++) {
        const uint8_t *phase = STEP_SEQ[Steps];
        digitalWrite(STEP_IN1, phase[0]);
        digitalWrite(STEP_IN2, phase[1]);
        digitalWrite(STEP_IN3, phase[2]);
        digitalWrite(STEP_IN4, phase[3]);
        SetDirection();
        SetSteps();
    }
}

void SetSteps() {
    Steps += dir;
    if (Steps > 7) Steps = 0;
    if (Steps < 0) Steps = 7;
}

void SetDirection() {
    degree += dir;
    if (degree > 4095) degree = 0;
    if (degree < 0)    degree = 4095;
}
