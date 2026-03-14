/**
 * @file stepper.h
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief Interface for the stepper motor driver.
 *
 * Implements a step sequence across four GPIO pins.
 * All functions rely on the shared globals `Steps`, `dir`, and `degree`
 * declared in config.h.
 */

// ====================== Function Prototypes =====================
/**
 * @brief Advance the motor by @p xw steps in the current direction.
 *
 * Applies the next entry in the 8-phase lookup table to the four
 * pins, then calls SetDirection() and SetSteps() to update shared state.
 *
 * @param xw  Number of steps to drive.  Pass 1 for fine-grained control.
 */
void stepper(int xw);

/**
 * @brief Increment or decrement the step index and wrap it to [0, 7].
 *
 * Called internally by stepper() after each phase change.
 * @see stepper
 */
void SetSteps();

/**
 * @brief Increment or decrement the degree counter and wrap it to [0, 4095].
 *
 * Tracks the panel's absolute angular position so the auto-rotate can
 * return to a target angle.  Called internally by stepper().
 * @see stepper
 */
void SetDirection();
