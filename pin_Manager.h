/* 
 * File:   pin_Manager.h
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 6:23 PM
 */

#ifndef PIN_MANAGER_H
#define	PIN_MANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define set     1
#define clear   0
#define analog  1
#define digital 0
#define input   1
#define output  0
        
#define LED_ARRAY_TRIS      TRISD
#define LCD_TRIS            TRISB
    
#define LCD_ANSEL           ANSELH
    
#define button_SW1_TRIS     TRISBbits.TRISB6
#define button_SW2_TRIS     TRISBbits.TRISB7
    
#define transistor_1_TRIS   TRISAbits.TRISA1
#define transistor_2_TRIS   TRISAbits.TRISA2
#define transistor_3_TRIS   TRISAbits.TRISA3
#define transistor_4_TRIS   TRISAbits.TRISA4 

#define EEPROM_MOSI_TRIS    TRISCbits.TRISC5
#define EEPROM_MISO_TRIS    TRISCbits.TRISC4
#define EEPROM_CS_TRIS      TRISAbits.TRISA5
#define EEPROM_CLOCK_TRIS   TRISCbits.TRISC3
    
#define Temperature_TRIS    TRISAbits.TRISA0
    
#define PWM_TRIS            TRISCbits.TRISC2
    
#define TR1_ANSEL           ANSELbits.ANS1     
#define TR2_ANSEL           ANSELbits.ANS2
#define TR3_ANSEL           ANSELbits.ANS3
#define TR4_ANSEL           ANSELbits.ANS4
    
#define CS_ANSEL            ANSELbits.ANS5
    
#define TEMP_ANSEL          ANSELbits.ANS0

#define led_Array           PORTD
#define led_Array_SetHigh   PORTD =  0xff
#define led_Array_setLow    PORTD =  0x00
#define led_Array_Toggle    PORTD ^= 0xff
    
#define SW1_pressed         PORTBbits.RB6 == 1                  
#define SW2_pressed         PORTBbits.RB7 == 1

#define CS_Chip_Select      PORTAbits.RA5 = 0
#define CS_Chip_Desect      PORTAbits.RA5 = 1
    
#define TR1_setHigh         PORTAbits.RA1 = 1
#define TR2_setHigh         PORTAbits.RA2 = 1
#define TR3_setHigh         PORTAbits.RA3 = 1
#define TR4_setHigh         PORTAbits.RA4 = 1
    
#define TR1_setLow          PORTAbits.RA1 = 0
#define TR2_setLow          PORTAbits.RA2 = 0
#define TR3_setLow          PORTAbits.RA3 = 0
#define TR4_setLow          PORTAbits.RA4 = 0

#define TR1_Toggle          PORTAbits.RA1 ^= 1
#define TR2_Toggle          PORTAbits.RA2 ^= 1
#define TR3_Toggle          PORTAbits.RA3 ^= 1
#define TR4_Toggle          PORTAbits.RA4 ^= 1

void pinManager(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PIN_MANAGER_H */

