/* 
 * File:   SYST_Init.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 7:19 PM
 */

#include "myDefinition.h"

void system_Initialize(void) {
    Oscillator_Init();            //use device internal oscillator
    Interrupt_Init();
    pinManager();
    Lcd_Init();
    ADC_Init();
    TMR0_Init();
    PWM_Init(10000);   //10Khz set frequency
    PWM_DutyCycle(80); //80% duty cycle
    SPI_Init();
    UART_Init(9600); //Baudrate
}

void Oscillator_Init(void){
    OSCCON  = 0x77;      //Enable internal oscillator 8Mhz
    OSCTUNE = 0x00;      //Oscillator is running at the factory-calibrated frequency 
}

