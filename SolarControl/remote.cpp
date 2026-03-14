/**
 * @file remote.cpp
 * @author Hisham Bhatti, Ben Bryant
 * @date 3/18/2026
 * @brief Implementation of IR remote reception and panel movement tasks.
 */

#include "remote.h"
#include "config.h"
#include "stepper.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "soc/timer_group_reg.h"

extern IRrecv irrecv;
extern decode_results results;

// ==================== Function Implementation ===================

/**
 * @brief Perform a full sweep and return the degree position
 *        at which the lowest reading was observed.
 *
 * Suspends Task_ProcessLight for the duration so that every queued light
 * sample is available for the scan rather than being consumed by the
 * display task.
 *
 * @return  `degree` value corresponding to the minimum ADC reading.
 */
static int findBestPosition() {
    vTaskSuspend(processLightHandle);

    int minLight = 4096;
    int bestDeg = degree;
    int raw = 0;
    dir = 1;  // always sweep forward

    for (int i = 0; i < STEPS_FULL_REV; i++) {
        if (xQueueReceive(lightQueue, &raw, portMAX_DELAY) == pdTRUE) {
            if (raw < minLight) {
                minLight = raw;
                bestDeg  = degree;
            }
        }
        stepper(1);
        vTaskDelay(1);
    }

    vTaskResume(processLightHandle);
    return bestDeg;
}

/**
 * @brief Drive the panel to the target degree position.
 *
 * Continues stepping in the current dir until degree matches @p target.
 *
 * @param target  Desired `degree` value [0, 4095].
 */
static void seekToDegree(int target) {
    while (degree != target) {
        stepper(1);
        vTaskDelay(1);
    }
}

void Task_ReadRemote(void *args) {
    static uint32_t lastTime = 0;
    uint32_t now = 0;
    int prevDir = 0;  // Remembered so REPEAT codes reuse the last direction

    while (1) {
        now = *((volatile uint32_t *) TIMG_T0LO_REG(1));

        if ((now - lastTime) >= REMOTE_INTERVAL) {
            if (irrecv.decode(&results)) {
                switch (results.value) {
                    case IR_LEFT:
                        prevDir =  1;
                        break;
                    case IR_RIGHT:
                        prevDir = -1;
                        break;
                    case IR_OK:
                        prevDir =  0;
                        break;
                    case IR_REPEAT:
                        break;          // reuse prevDir unchanged
                    default:
                        irrecv.resume();
                        goto next_tick;
                }
                xQueueSend(remoteQueue, &prevDir, portMAX_DELAY);
                irrecv.resume();
            }
            next_tick:
            lastTime = now;
        }

        *((volatile uint32_t *) TIMG_T0UPDATE_REG(1)) = 1;
        vTaskDelay(1);
    }
}

void Task_ProcessRemote(void *args) {
    while (1) {
        if (xQueueReceive(remoteQueue, &dir, portMAX_DELAY) == pdTRUE) {
            if (dir == 0) {
                // Auto rotate
                int bestDeg = findBestPosition();
                seekToDegree(bestDeg);
            } else {
                // Manual rotate drive
                for (int i = 0; i < STEPS_PER_MOVE; i++) {
                    stepper(1);
                    vTaskDelay(1);
                }
            }
        }
    }
}
