/*
 * task_print1.h
 *
 *  Created on: Oct 19, 2020
 *      Author: Joe Krachey
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "main.h"

#define VOLT_0P85  1056      // 0.85 /(3.3/4096)
#define VOLT_2P50  3103      // 2.50 /(3.3/4096)

extern TaskHandle_t Task_Joystick_Handle;
extern TaskHandle_t Task_Joystick_Timer_Handle;


/******************************************************************************
* Examines the ADC data from the joystick on the MKII
******************************************************************************/
void Task_Joystick_Bottom_Half(void *pvParameters);


#endif /* JOYSTICK_H_ */
