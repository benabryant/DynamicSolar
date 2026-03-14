/**
 * @file remote.h
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief FreeRTOS tasks for IR remote reception and panel movement.
 *
 * Three remote buttons are handled:
 *  - Left: Rotate counterclockwise
 *  - Right: Rotate clockwise
 *  - OK: Automaitacally rotate to best light 

// ====================== Function Prototypes =====================
/**
 * @brief Task: decode IR signals and enqueue movement commands.
 *
 * - Polls the IR receiver every REMOTE_INTERVAL hardware-timer ticks.
 * - Translates LEFT -> +1, RIGHT -> -1, OK -> 0, REPEAT -> previous value.
 * - Pushes the direction value onto remoteQueue.
 *
 * @param args  Unused FreeRTOS task parameter.
 * @see Task_ProcessRemote
 */
void Task_ReadRemote(void *args);

/**
 * @brief Task: execute movement commands received from ::remoteQueue.
 *
 *
 * Normal move (dir != 0): Drives the stepper STEPS_PER_MOVE steps
 * in the commanded direction.
 *
 * Auto-seek (dir == 0, OK button):
 *  1. Suspends Task_ProcessLight so light readings are not consumed.
 *  2. Sweeps the panel through one full revolution (STEPS_FULL_REV steps),
 *     reading a raw light value at each position.
 *  3. Records the position (`degree`) where the minimum ADC value was seen.
 *  4. Rotates the panel back to that optimal position.
 *  5. Resumes Task_ProcessLight.
 *
 * @param args  Unused FreeRTOS task parameter.
 * @see Task_ReadRemote
 */
void Task_ProcessRemote(void *args);
