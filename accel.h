/*
 * accel.h
 *
 *  Created on: Apr 30, 2022
 *      Author: Ziyang_Cai
 */

#ifndef ACCEL_H_
#define ACCEL_H_
#include "main.h"

extern volatile bool ALERT_SHAKE_UPDATE;
extern uint8_t shake_cnt;
extern TaskHandle_t task_accel_handle;

void task_accel(void *pvParameters);

#endif /* ACCEL_H_ */
