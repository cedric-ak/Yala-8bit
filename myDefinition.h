/* 
 * File:   myDefinition.h
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 5:10 PM
 */



#ifndef MYDEFINITION_H
#define	MYDEFINITION_H

#ifdef	__cplusplus
extern "C" {
#endif

#define _XTAL_FREQ 8000000UL   //4Mhz clock frequency

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pin_Manager.h"
#include "lcd_Config.h"
#include "adc_Init.h"
#include "_25LC160B_EEPROM.h"
#include "uart_Init.h"  
#include "RN4020_Bluetooth.h"

    void system_Initialize(void);
    extern void Oscillator_Init(void);
    extern void Interrupt_Init(void);
    extern void temp_Display(void);
    unsigned int segment_Variable;
    extern void PWM_Init(uint32_t iFrequency);
    extern void PWM_DutyCycle(uint32_t iDutyCycle);
    extern void TMR0_Init(void);
    extern void SPI_Init(void);
    extern void SPI_MasterTransmit(uint8_t iData);

    uint8_t internal_eeprom_read(uint8_t addr);

    uint8_t segment_Array[] = {0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; //7segment look up table

    extern void ASS_1(void);
    extern void Ass_2(void);
    uint8_t shitf_Speed;
    bool increaseTime_Enabled = false;

#define set_Application(state)      g_state = state
#define Get_Application()           g_state

    typedef enum {
        LED_DEMO,
        PWM_DEMO,
        ADC_DEMO,
        USART_DEMO
    } t_Application;

    t_Application g_state = LED_DEMO;


#ifdef	__cplusplus
}
#endif

#endif	/* MYDEFINITION_H */

