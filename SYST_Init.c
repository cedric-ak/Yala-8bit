/* 
 * File:   SYST_Init.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 7:19 PM
 */

#include "myDefinition.h"

void system_Initialize(void) {
    Oscillator_Init();
    Interrupt_Init();
    pinManager();
    Lcd_Init();
    ADC_Init();
    TMR0_Init();
    PWM_Init(10000);   //10Khz set frequency
    PWM_DutyCycle(70); //70% duty cycle
    SPI_Init();
    UART_Init(9600);
}

void Oscillator_Init(void){
    OSCCON  = 0x00;      //disable internal oscillator (run on HS external oscillator)
    OSCTUNE = 0x00;      //Oscillator is running at the factory-calibrated frequency 
}

