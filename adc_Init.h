/* 
 * File:   adc_Init.h
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 8:53 PM
 */

#ifndef ADC_INIT_H
#define	ADC_INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define CONVER_CLK_FOSC_2    0b00  
#define CONVER_CLK_FOSC_8    0b01
#define CONVER_CLK_FOSC_32   0b10
    
#define CHN_SELECT_AN0       0x0000
#define CHN_SELECT_AN1       0x0001
#define CHN_SELECT_AN2       0x0010
#define CHN_SELECT_AN3       0x0011
#define CHN_SELECT_AN4       0x0100
#define CHN_SELECT_AN5       0x0101
#define CHN_SELECT_AN6       0x0110
#define CHN_SELECT_AN7       0x0111
    
#define ADC_Enable           1
#define ACD_Disable          0
    
#define left_justified       0
#define right_justified      1 
    
#define external_Vref        0b11
#define Vdd_Vss_ref          0b00 

#define start_conversion         GO_DONE = 1
#define stop_conversion          GO_DONE = 0
#define conversion_inProgress    while(/*!PIR1bits.ADIF*/ADCON0bits.GO)
#define conversion_complete      PIR1bits.ADIF
    
void ADC_Init(void);
uint16_t temp_Measurement(void);

    union {
        uint8_t result;
        char reading[2];
    } adc;

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_INIT_H */

