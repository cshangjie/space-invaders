/*
 * accel.c
 *
 *  Created on: Apr 30, 2022
 *      Author: Ziyang_Cai
 */
#include "accel.h"

uint8_t shake_cnt = 0;
volatile bool ALERT_SHAKE_UPDATE = false;
TaskHandle_t task_accel_handle;

// Debounce the accelerometer readings to detect shaking of the device
void task_accel(void *pvParameters) {
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        shake_cnt++;
        if (shake_cnt >= 5) {
            ALERT_SHAKE_UPDATE = true;
            shake_cnt = 0;
            game_actions_t action = SHAKE;
            xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
        }
    }
}
