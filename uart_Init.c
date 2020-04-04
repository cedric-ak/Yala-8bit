/* 
 * File:   uart_Init.c
 * Author: CEDRIC
 *
 * Created on 13 July 2019, 10:18 PM
 */

#include "myDefinition.h"

void UART_Init(uint32_t baudRate) {

    TRISCbits.TRISC6 = input;
    TRISCbits.TRISC7 = input;
    
    TXSTAbits.TXEN = set;           //UART transmit enabled
    TXSTAbits.SYNC = clear;         //asynchronous mode transmission
    TXSTAbits.TX9  = clear;         //select 8bit transmission
    RCSTAbits.SPEN = set;           //Serial port enabled
    RCSTAbits.CREN = set;           //enable UART continuous receiver
    RCSTAbits.RX9  = clear;         //8 bit reception
    RCSTAbits.ADDEN = set;
    RCSTAbits.FERR  = clear;
    RCSTAbits.OERR  = clear;
    
    
    TXSTAbits.BRGH    = set;        //select high baud rate
    BAUDCTLbits.BRG16 = set;        //16bit baud rate generator is used    
    SPBRG = ((_XTAL_FREQ/baudRate)/4)-1; //calculate baud rate
}

void UART_Write_char(uint8_t iData){
    while(!TXSTAbits.TRMT);          //wait for transmit to complete
    TXREG = iData;
}

void UART_Write_String(uint8_t cText[]) {
    for (int iSize = 0; cText[iSize] != '\0'; iSize++) {  //print strings and break at carriage return
        UART_Write_char(cText[iSize]);
    }
}

char UART_Read(void) {
    while (!PIR1bits.RCIF) { //wait for data to complete (do nothing while buffer is receiving)
//        RxData[RxHead++] = RCREG;
//        if (RxHead == sizeof (RxData)) {
//            RxHead = 0;
//        }
//
//        if (RxHead == RxTail) {
//            RxTail++;
//            if (RxTail == sizeof (RxData)) {
//                RxTail = 0;
//            }
//        }
    }
//    PIR1bits.RCIF = set;
    return RCREG;
}

 uint16_t UART_Read_String(uint8_t* pbData, uint16_t iSize) {
        uint16_t iSizeRead = 0;

        while (iSizeRead != iSize && RxTail != RxHead) {
            pbData[iSizeRead++] = RxData[RxTail++];

            if (RxTail == sizeof (RxData)) {
                RxTail = 0;
            }
        }       
        return iSizeRead;
    }

bool UART_Data_available(void) {
    return RCIF;
}