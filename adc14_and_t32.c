
#include <adc14_andt_32.h>
#include "math.h"

void configure_T32() {
    // Configure Timer32_1 to generate an interrupt every 10mS
    TIMER32_1->CONTROL = 0;
    TIMER32_1->CONTROL |= TIMER32_CONTROL_MODE;
    TIMER32_1->CONTROL |= TIMER32_CONTROL_PRESCALE_1;
    TIMER32_1->LOAD = 15000;
    TIMER32_1->CONTROL |= TIMER32_CONTROL_IE;
    NVIC_EnableIRQ(T32_INT1_IRQn);
    NVIC_SetPriority(T32_INT1_IRQn, 2);
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE;

    // Configure Timer32_2 to generate an interrupt every 100mS
    TIMER32_2->CONTROL = 0;
    TIMER32_2->CONTROL |= TIMER32_CONTROL_MODE;
    TIMER32_2->CONTROL |= TIMER32_CONTROL_PRESCALE_2;
    TIMER32_2->LOAD = 9375;
    TIMER32_2->CONTROL |= TIMER32_CONTROL_IE;
    NVIC_EnableIRQ(T32_INT2_IRQn);
    NVIC_SetPriority(T32_INT2_IRQn, 2);
    TIMER32_2->CONTROL |= TIMER32_CONTROL_ENABLE;
}

void adc14_init() {
    /* X: J3.23, Y: J3.24, Z: J3.25
     *    A14       A13       A11
     *    P6.1      P4.0      P4.2
     */

    // Configure pins to be analog input
    P6->SEL0 |= BIT1;
    P4->SEL0 |= BIT0 | BIT2;
    // Configure the X direction as an analog input pin.
    P6->SEL0 |= BIT0;
    P6->SEL1 |= BIT0;
    // Configure the Y direction as an analog input pin.
    P4->SEL0 |= BIT4;
    P4->SEL1 |= BIT4;
    // Configure CTL0 to sample 16-times in pulsed sample mode.
    // Indicate that this is a sequence of samples.
    ADC14->CTL0 = 0;
    ADC14->CTL1 = 0;
    ADC14->CTL0 |= ADC14_CTL0_SHP | ADC14_CTL0_SHT02 | ADC14_CTL0_CONSEQ_1;
    // Configure CTL1 to return 12-bit values
    ADC14->CTL1 |= ADC14_CTL1_RES_2;
    // Store conversion result
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_14 | ADC14_MCTLN_WINC; //enable window comparison for shaking detection
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_13 | ADC14_MCTLN_WINC;
    ADC14->MCTL[2] |= ADC14_MCTLN_INCH_11 | ADC14_MCTLN_WINC;
    ADC14->MCTL[3] |= ADC14_MCTLN_INCH_15;
    ADC14->MCTL[4] |= ADC14_MCTLN_INCH_9 | ADC14_MCTLN_EOS;
    // Set up the HI0 Window, this deteermines the sensitivity of the shaking
    ADC14->HI0 = 3500;
    // Enable interrupts when either the HI or LO thresholds of the window
    // comparator has been exceeded.  Disable all other interrupts
    ADC14->IER1 = ADC14_IER1_HIIE;
    ADC14->IER0 = ADC14_IER0_IE4;
    // Enable ADC Interrupt
    NVIC_EnableIRQ(ADC14_IRQn);
    NVIC_SetPriority(ADC14_IRQn, 2);
    // Turn ADC ON
    ADC14->CTL0 |= ADC14_CTL0_ON;
}

