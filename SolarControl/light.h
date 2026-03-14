/**
 * @file light.h
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief FreeRTOS tasks for sampling and displaying ambient light.
 *
 * Light levels are expressed as a percentage scaled dynamically between the
 * minimum and maximum raw light values observed at runtime (`lightMin` /
 * `lightMax` in config.h).  This means the display always spans 0–100 %
 * relative to the current operating environment rather than a hard-coded range.
 */

// ====================== Function Prototypes =====================

/**
 * @brief Task: sample the photoresistor and push readings to lightQueue.
 *
 * - Reads the light every LIGHT_INTERVAL hardware-timer ticks.
 * - Updates the global lightMin and lightMax values.
 * - Enqueues the raw light value for Task_ProcessLight to consume.
 *
 * @param args  Unused FreeRTOS task parameter.
 * @see Task_ProcessLight
 */
void Task_ReadLight(void *args);

/**
 * @brief Task: dequeue light readings and update the LCD and buzzer.
 *
 * - Converts raw light readings to a percentage using lightMin / lightMax
 * - Displays the percentage on the LCD.
 * - Activates the buzzer when the scaled level a threshhold.
 *
 * @note This task may be suspended by Task_ProcessRemote during the
 *       automatic best-position scan (OK button).
 *
 * @param args  Unused FreeRTOS task parameter.
 * @see Task_ReadLight
 */
void Task_ProcessLight(void *args);
