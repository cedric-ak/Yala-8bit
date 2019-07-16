/*
 * File:   pwm_Init.c
 * Author: CEDRIC
 *
 * Created on 12 July 2019, 8:24 PM
 */


#include "myDefinition.h"
/*
 PWM_Init function is to be called in main prior any activity related to PWM be executed (PMW1 in this case)
 * Pwm_init argument "frequency" defines frequency of out of related PWM signal when pwm_init is called define and integer value of frequency in hertz
 * pwm_dutyCycle function defines frequency duty cycle, this is defined in percentage from 0 representing 0% and 100 represent 100% duty cycle
 */
void PWM_Init(uint32_t iFrequency){
  PWM_TRIS = input;
 
  PR2 = (_XTAL_FREQ / (iFrequency * 4)) -1;                //set PWM period, (refer to device data sheet page. 126 equation 11-1) 
  CCP1CONbits.P1M   = 0b00;                               //single output P1A modulated, P1B, P1C, P1D assigned as port pins
  CCP1CONbits.CCP1M = 0b1100;                             //PWM mode P1A, P1B, P1C, P1D active low
  CCP1CONbits.DC1B  = 0b00;
  CCPR1L            = 0x00;                               //duty cycle to 0% 

  PIR1bits.TMR2IF  = clear;                               //TMR2 interrupt flag clear
  T2CONbits.TOUTPS = 0b0000;                              //postscaler 0
  T2CONbits.T2CKPS = 0b00;                                //Prescaler is 1
  T2CONbits.TMR2ON = set;                                 //TMR2 enable
  PWM_TRIS = output;                                      //set PWM1 pin as output
}


void PWM_DutyCycle(uint32_t iDutyCycle) {
 /*
 pwm duty cycle is set in percentage with a range of 0 - 100
*/
    iDutyCycle = (iDutyCycle * (4*(PR2 + 1)))/100;             //duty cycle ratio equation (refer to device data sheet page. 127 )
    CCP1CONbits.DC1B = (iDutyCycle<<4);                       //assign duty to cycle to register
    CCPR1L = (iDutyCycle >> 2);    
}