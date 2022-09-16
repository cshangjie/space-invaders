/*
 * interrupts.c
 *
 *  Created on: Aug 20, 2020
 *      Author: Joe Krachey
 */
#include "interrupts.h"


void T32_INT1_IRQHandler(void)
{
    // 10ms
    // Start the ADC conversion
    ADC14->CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;

    BaseType_t xHigherPriorityTaskWoken;
    // notify s1, s2 tasks to check for debounce and pressed state
    vTaskNotifyGiveFromISR(task_s1_handle, &xHigherPriorityTaskWoken);
    vTaskNotifyGiveFromISR(task_s2_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    // Clear the timer interrupt
    TIMER32_1->INTCLR = BIT0;
}
void T32_INT2_IRQHandler(void)
{
    // 100ms

    BaseType_t xHigherPriorityTaskWoken;
    // Trigger light sensor conversion
    vTaskNotifyGiveFromISR(task_opt3k_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    // clear the timer interrupt
    TIMER32_2->INTCLR = BIT0;
}

volatile bool ALERT_ACCEL_UPDATE = false;
volatile uint32_t ACCEL_X;
volatile uint32_t ACCEL_Y;
volatile uint32_t ACCEL_Z;
volatile uint32_t JOYSTICK_X_DIR = 0;
volatile uint32_t JOYSTICK_Y_DIR = 0;

void ADC14_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken;
    // set the global variable that informs the application that the Window Camparator
    // had detected a change
    ALERT_ACCEL_UPDATE = true;
    // Read the value and clear the interrupt
    ACCEL_X = ADC14->MEM[0];
    ACCEL_Y = ADC14->MEM[1];
    ACCEL_Z = ADC14->MEM[2];
    JOYSTICK_X_DIR = ADC14->MEM[3];
    JOYSTICK_Y_DIR = ADC14->MEM[4];
    // Determine if the HIIFG interrupt is active, this means the user is shaking the device
    if (ADC14->IFGR1 & ADC14_IFGR1_HIIFG)
    {
        // Clear interrupt flag
        ADC14->CLRIFGR1 |= ADC14_CLRIFGR1_CLRHIIFG;
        vTaskNotifyGiveFromISR(task_accel_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    /* ADD CODE
     * Send a task notification to Task_Joystick_Bottom_Half
     */
    vTaskNotifyGiveFromISR(Task_Joystick_Handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


