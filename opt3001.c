/*
 * opt3001.c
 *
 *  Created on: Mar 14, 2021
 *      Author: younghyunkim
 */

#include "opt3001.h"
#include "math.h"

/******************************************************************************
 * Initialize the opt3001 ambient light sensor on the MKII.  This function assumes
 * that the I2C interface has already been configured to operate at 100KHz.
 ******************************************************************************/

void opt3001_init(void)
{
    // Initialize OPT3001
    // Conversion time: 100 ms
    // Mode of conversion: Continuous conversion
    uint16_t data = 0xc610;
    i2c_write_16(OPT3001_SLAVE_ADDRESS, CONFIG_REG, data);
}


/******************************************************************************
 * Returns the current ambient light in lux
 *  ******************************************************************************/
float opt3001_read_lux(void)
{
    // Read the Result register of OPT3001 and convert into Lux, then return.
    uint16_t data = i2c_read_16(OPT3001_SLAVE_ADDRESS, RESULT_REG);
    uint16_t exp = data >> 12;
    uint16_t res = data & ((1 << 12) - 1);

    float lux = 0.01 * pow(2, exp) * res;
    return lux; // Need to modify
}

TaskHandle_t task_opt3k_handle;

// upon receiving notification, check if room has changed from light to dark or dark to light
void task_opt3k(void *pvParameters) {
    uint8_t light_debounce = 0;
    uint8_t dark_debounce = 0;
    game_actions_t prev_action = IS_LIGHT;
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // Light sensor stuff
        float lux = opt3001_read_lux();
        light_debounce <<= 1;
        dark_debounce <<= 1;
        if (lux < 30) {
            dark_debounce |= BIT0;
        }
        if (lux > 30) {
            light_debounce |= BIT0;
        }
        game_actions_t action;
        if (dark_debounce == 0x07) {
            action = IS_DARK;
        }
        if (light_debounce == 0x07) {
            action = IS_LIGHT;
        }
        if (prev_action != action) {
            xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
            dark_debounce = 0;
            prev_action = action;
        }

    }
}

