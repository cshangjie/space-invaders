/*
 * task_led.c
 *
 *  Created on: Oct 19, 2020
 *      Author: Joe Krachey
 */

#include <main.h>


 TaskHandle_t Task_Joystick_Handle;
 TaskHandle_t Task_Joystick_Timer_Handle;

/******************************************************************************
* Bottom Half Task.  Examines the ADC data from the joystick on the MKII
******************************************************************************/
void Task_Joystick_Bottom_Half(void *pvParameters)
{
    game_actions_t dir;
    game_actions_t prev_dir = JOYSTICK_DIR_CENTER;
    uint32_t ulEventsToProcess;


    while(1)
    {
        /*
         * Wait until we get a task notification from the ADC14 ISR
         */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /*
         * Set the dir variable to one of the following values based
         * on the values of JOYSTICK_X_DIR and JOYSTIC_Y_DIR
         */
        if(JOYSTICK_X_DIR < VOLT_0P85)
        {
           dir = JOYSTICK_DIR_LEFT;
        }
        else if(JOYSTICK_X_DIR > VOLT_2P50)
        {
            dir = JOYSTICK_DIR_RIGHT;
        }
        else if(JOYSTICK_Y_DIR < VOLT_0P85)
        {
            dir = JOYSTICK_DIR_DOWN;
        }
        else if(JOYSTICK_Y_DIR > VOLT_2P50)
        {
            dir = JOYSTICK_DIR_UP;
        }
        else
        {
            dir = JOYSTICK_DIR_CENTER;
        }

        /*
         * Send dir to Queue_Console if the the current direction
         * of the joystick does not match the previous direction of the joystick
         */
        if (dir != prev_dir) {
            xQueueSendToBack(queue_game_actions, &dir, portMAX_DELAY);
        }
    }
}



