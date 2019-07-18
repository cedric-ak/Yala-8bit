/* 
 * File:   spi_Init.c
 * Author: CEDRIC
 *
 * Created on 13 July 2019, 10:17 PM
 */

#include "myDefinition.h"

void SPI_Init(void){
   SSPCONbits.SSPEN = set;               //Enables serial port and configures SCK, SDO, SDI, and SS as serial port pins
   SSPCONbits.SSPM  = 0b000;             //SPI Master mode, clock = FOSC/4    1Mhz clock frequency @ Fosc 4Mz
   SSPCONbits.WCOL  = clear;             //No collision 
    
   PIE1bits.SSPIE = set;                 //Enable MSSP interrupt
   PIR1bits.SSPIF = clear;               //clear MSSP interrupt flag
}

void SPI_MasterTransmit(uint8_t iData) {
    SSPBUF = iData;                      //write data to buffer
    while(!PIR1bits.SSPIF);              //wait for transmission or reception on buffer to complete
//    while (!SSPSTATbits.BF);           //wait for data transfer to complete (do not comment out if interrupt if disabled)
}


