/* 
 * File:   timer_Init.c
 * Author: CEDRIC
 *
 * Created on 14 July 2019, 7:40 PM
 */
#include "myDefinition.h"

/*
 TMR0 with enabled interrupt at 10ms.
 * FOSC = 8Mhz =  _XTAL_FERQ
 * Prescaler 1:64
 * TMR0 register at 100 after calculation
 * Desired interrupt time = (256 - TMR0)*1TICK
 * TICK = TCY*Prescaler
 * TCY = 4/FOSC
 */

void TMR0_Init(void) {
    INTCONbits.T0IE = set;                        //TMR0 interrupt enable
    INTCONbits.T0IF = clear;                      //TMR0 interrupt flag cleared
    
    OPTION_REGbits.T0CS = clear;                  //internal instruction cycle clock
    OPTION_REGbits.PSA  = clear;                  //Prescaler assigned to TMR0 module
    OPTION_REGbits.PS   = 0b101;                  //Prescaler 1:64
    TMR0 = 57;                                    //calculated value for 10ms interrupt
}

void TMR1_Init(void){
    
}