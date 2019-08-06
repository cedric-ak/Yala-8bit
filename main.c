/* 
 * File:   main.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 6:10 PM
 */

#include "configuartion.h"
#include "myDefinition.h"

uint8_t shift = 1;
void led_Demo(void);
char da[10];
int main(void) {    
    
    system_Initialize();
    
    shitf_Speed = eeprom_Read(0x01);
    if(eeprom_Read(0x05) == 0xff || eeprom_Read(0x05) == 0x00){   //if led array value not set yet, 
        led_Array = 0x01;
    }else{
      led_Array = eeprom_Read(0x05);                             //initialy start on 200ms shif speed                           
    }
    
    
    while (true) {
         Lcd_Set_Cursor(1,1);
         Lcd_write("WELCOME TO YALA");

        switch (Get_Application()) {
            
            case LED_DEMO:
                led_Demo();
                break;

            case PWM_DEMO:
                
                eeprom_Write(0x00, 0x02);         //load last last routine executed in eeprom  
                break;
            case ADC_DEMO:
                
                eeprom_Write(0x00, 0x03);         //load last last routine executed in eeprom 
                break;

            case USART_DEMO :
                
                eeprom_Write(0x00, 0x04);        //load last last routine executed in eeprom 
                break;
        }
    }
    return;
}

void led_Demo(void){
    __delay_ms(300);
    led_Array = led_Array << shift;
    eeprom_Write(led_Array, 0x05);
    if (led_Array == 0x80) {
        __delay_ms(300);
        led_Array = 0x01;
    }
}

void pwm_demo(void){
    if(SW1_pressed){
        __delay_ms(10);
        if(SW1_pressed){
            
//           PWM_DutyCycle(); 
        }
    }
}