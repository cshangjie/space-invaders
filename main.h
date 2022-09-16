/*
 * main.h
 *
 *  Created on: Oct 19, 2020
 *      Author: Joe Krachey
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "buttons.h"
#include "game.h"
#include "adc14_andt_32.h"
#include "game_images.h"
#include "interrupts.h"
#include "lcd.h"
#include "i2c.h"
#include "music.h"
#include "opt3001.h"
#include "accel.h"
#include "joystick.h"

/* LCD Semaphore */
extern SemaphoreHandle_t Sem_LCD;

#endif /* MAIN_H_ */
