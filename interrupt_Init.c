/*
 * File:   interrupt_Init.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 11:28 PM
 */

#include "myDefinition.h"

void Interrupt_Init(void) {

    INTCONbits.GIE  = set;  //enable global Interrupt
    INTCONbits.PEIE = set;  //enable peripheral interrupt

//    PIE1bits.ADIE = set;   //enable ADC (Analog to Digital converter) interrupt
//    PIR1bits.ADIF = clear; //clear ADC interrupt flag 
//    
    //PIE1bits.RCIE = set;   //EUSART receive interrupt enabled
   // PIR1bits.RCIF = clear; //clear EUSART receive interrupt
    
//    PIE2bits.EEIE = set;  //enable EEPROM interrupt
//    PIR2bits.EEIF = clear;
}

void interrupt ISR(void) {
//    if(PIE1bits.RCIE == set && PIR1bits.RCIF == set){      //if received data ready or complete
////        PIR1bits.RCIF = clear;      
//        UART_Read();
//    }
//    if (PIE1bits.ADIE == 1 && PIR1bits.ADIF == 1) {         //ADC conversion is complete clear interrupt flag
//        PIR1bits.ADIF = clear;                              //clear ADC interrupt flag at completion of conversion 
//    }
    if (INTCONbits.T0IE == set && INTCONbits.T0IF == set) {   //if TMR0 interrupt flag is raised 
        INTCONbits.T0IF = clear;                              //clear interrupt flag after instruction execute
    }

}