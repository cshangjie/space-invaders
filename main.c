
/*
 *  ======== main_freertos.c ========
 */
#include "main.h"
/*
 *  ======== main ========
 */
int main(void)
 {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    queue_game_actions = xQueueCreate(20,sizeof(game_actions_t));
    Sem_LCD = xSemaphoreCreateBinary();
    xSemaphoreGive(Sem_LCD);
    xTaskCreate
    (   task_game,
        "task_game",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        &task_game_handle
    );

    xTaskCreate
    (   task_accel,
        "task_accel",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &task_accel_handle
    );

    xTaskCreate
    (   Task_Joystick_Bottom_Half,
        "Task_Joystick",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &Task_Joystick_Handle
    );

    xTaskCreate
    (   task_opt3k,
        "Task_OPT3k",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &task_opt3k_handle
    );

    xTaskCreate
    (   task_s1,
        "Task_S1",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &task_s1_handle
    );

    xTaskCreate
    (   task_s2,
        "Task_S2",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &task_s2_handle
    );

    xTaskCreate
    (   task_music_play_song,
        "Task_Music",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &task_music_play_song_handle
    );

    xTaskCreate
    (   task_game_tick,
        "Task_Game_Tick",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &task_game_tick_handle
    );

//    serial_debug_init();
    init_s1s2();
    adc14_init();
    configure_T32();
    i2c_init();
    opt3001_init();
    setup_buzzer();
    Crystalfontz128x128_Init();

    __enable_irq();
//    printf("ECE 353 Final Project\r\n");

    game_actions_t action = GAME_END;
    xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);

    vTaskStartScheduler();

    while(1) {}
    return 0;
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none test
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
