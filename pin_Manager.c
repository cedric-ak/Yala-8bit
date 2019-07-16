/* 
 * File:   pin_Manager.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 7:00 PM
 */

#include "myDefinition.h"

/*
 ANSEL and ANSELH : Registers associated with analog register, this control mode of operation of all analog associated port bits
 *                  when set to (=1) ANx port bit run as analog and when cleared with a "0" port bits run as digital. 
 *                  "THIS IS TO BE CONFIGURED FOR ALL ANx BITS prior SETTING TRIS REGISTER" 
 TRIS : Register initializes port bits; when set to (=1)causes pin to run as INPUT,
 *      and when cleared with "0" port bit is set as an output
 PORT : read and write Register, when set with a "1" specific bit is high(+5V) and when cleared with "0" port bit is low (0V)
 */

void pinManager(void) {
    
    LED_ARRAY_TRIS    = output;

    LCD_ANSEL         = digital;    
    LCD_TRIS          = output;
 
    button_SW1_TRIS   = input;
    button_SW2_TRIS   = input;
    PWM_TRIS          = output;
    
    TR1_ANSEL         = digital;
    TR2_ANSEL         = digital;
    TR3_ANSEL         = digital;
    TR4_ANSEL         = digital;
    
    transistor_1_TRIS = output;
    transistor_2_TRIS = output;
    transistor_3_TRIS = output;
    transistor_4_TRIS = output;
    
    TEMP_ANSEL        = analog;
    CS_ANSEL          = digital;
    
    Temperature_TRIS  = input;
    
    EEPROM_MOSI_TRIS  = output;
    EEPROM_MISO_TRIS  = input;
    EEPROM_CS_TRIS    = output;
    EEPROM_CLOCK_TRIS = output;       

    /*****clear MCU IO port before process begins*/
    PORTA = clear;
    PORTB = clear;
    PORTC = clear;
    PORTD = clear;
    PORTE = clear;
}

