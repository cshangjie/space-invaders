#ifndef __INTERRUPTSH__
#define __INTERRUPTSH__
#include "main.h"

extern volatile bool ALERT_DARK;
extern volatile bool ALERT_ACCEL_UPDATE;
extern volatile uint32_t ACCEL_X;
extern volatile uint32_t ACCEL_Y;
extern volatile uint32_t ACCEL_Z;
extern volatile uint32_t JOYSTICK_X_DIR;
extern volatile uint32_t JOYSTICK_Y_DIR;

//void T32_INT1_IRQHandler(void);
void ADC14_IRQHandler(void);
#endif
