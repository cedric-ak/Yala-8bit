/* 
 * File:   adc_Init.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 8:52 PM
 */

#include "myDefinition.h"
uint16_t rawData;
float temperature;
char sTemperature[10];

void ADC_Init(void){
   ADCON0bits.ADCS    =  CONVER_CLK_FOSC_32;     //conversion clock Fosc/8  ===> 4Mz/8 (sec))
   ADCON0bits.CHS     =  CHN_SELECT_AN0;        //ADC channel select AN0
   ADCON0bits.GO_DONE =  stop_conversion;       //do not start conversion yet
   ADCON0bits.ADON    =  ADC_Enable;            //ADC module is enabled
   
   ADCON1bits.ADFM    =  right_justified;       //right justifier ADC result format
   ADCON1bits.VCFG0   =  Vdd_Vss_ref;           //VDD and VSS reference voltage
   ADCON1bits.VCFG1   =  Vdd_Vss_ref;   
}

uint16_t temp_Measurement(void) {
    __delay_ms(5);                                          //wait for acquisition time
    start_conversion;                                       //start conversion    
    conversion_inProgress;                                  //do nothing wait for conversion to complete 
    adc.reading[0] = ADRESL;                                //assigned lower bit of result to first variable
    adc.reading[1] = ADRESH;                                //assigned Higher bit of result to second variable
   /* for (int sample = 0; sample < 10; sample++) {           //capture 11 sample reading before proceeding for precision measurement
        rawData += adc.result;                              //collect temperature raw data into one variable                
        if (sample == 9) {  
            rawData = rawData / 11;                         //calculate mean value of collected samples
            temperature = ((rawData * 5000.0 / 1023.0) - 500) /10; //for all positive temperature
        }
    }*/
    temperature = ((adc.result * 5000.0 / 1024.0) - 500) /10;
    sprintf(sTemperature, "%.2f", temperature); //convert float to string for lcd display
    Lcd_Set_Cursor(1, sizeof("Temperature:"));
    Lcd_write(sTemperature);
    return temperature;                                     //return temperature value in degree celsius
}

void temp_Display(void) {
    char *TEMP = "Temperature:";
    Lcd_Set_Cursor(1,1);
    Lcd_write(TEMP);
    sprintf(sTemperature, "%4d", temp_Measurement()); //convert float to string for lcd display
    Lcd_Set_Cursor(1, sizeof("Temperature:"));
    Lcd_write(sTemperature);
    
}