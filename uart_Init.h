/* 
 * File:   uart_Init.h
 * Author: CEDRIC
 *
 * Created on 15 July 2019, 2:57 AM
 */

#ifndef UART_INIT_H
#define	UART_INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

extern void UART_Init(uint32_t baudRate);
extern void UART_Write_char(uint16_t iData);
extern void UART_Write_String(uint8_t cText[]);
extern char UART_Read(void);
extern bool UART_Data_available(void);
extern uint16_t UART_Read_String(uint8_t* pbData, uint16_t iSize);
uint8_t RxData[20];
volatile uint16_t RxHead;
volatile uint16_t RxTail;

#ifdef	__cplusplus
}
#endif

#endif	/* UART_INIT_H */

