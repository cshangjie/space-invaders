#include "buttons.h"

void init_s1s2(void)
{
    // Configure the pin as an input
    P3->DIR &= ~BIT5;
    P5->DIR &= ~BIT1;
}

bool S1_pressed() {
    return P5->IN & BIT1;
}

bool S2_pressed() {
    return P3->IN & BIT5;
}

TaskHandle_t task_s1_handle;
TaskHandle_t task_s2_handle;
uint8_t s1_debounce = 0;
uint8_t s2_debounce = 0;

// Debounce buttons for both pressed and released states and alert game.c
void task_s1(void *pvParameters) {
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        s1_debounce <<= 1;
        if (S1_pressed()) {
            s1_debounce |= BIT0;
        }
        if (s1_debounce == 0x1F) {
            game_actions_t action = S1_PRESSED;
            xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
        }
        if ((s1_debounce & 0x1F) == 0x010) {
            game_actions_t action = S1_RELEASED;
            xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
        }
    }
}

void task_s2(void *pvParameters) {
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        s2_debounce <<= 1;
        if (S2_pressed()) {
            s2_debounce |= BIT0;
        }
        if (s2_debounce == 0x1F) {
            game_actions_t action = S2_PRESSED;
            xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
        }
        if ((s2_debounce & 0x1F) == 0x010) {
            game_actions_t action = S2_RELEASED;
            xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
        }
    }
}
