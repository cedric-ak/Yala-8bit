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

extern void UART_Init(uint16_t baudRate);
extern void UART_Write_char(uint16_t iData);
extern void UART_Write_String(uint8_t *cText);
extern uint16_t UART_Read(void);



#ifdef	__cplusplus
}
#endif

#endif	/* UART_INIT_H */

