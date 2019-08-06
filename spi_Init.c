/* 
 * File:   spi_Init.c
 * Author: CEDRIC
 *
 * Created on 13 July 2019, 10:17 PM
 */

#include "myDefinition.h"

void SPI_Init(void){
    
   SSPCONbits.SSPEN = set;               //Enables serial port and configures SCK, SDO, SDI, and SS as serial port pins
   SSPCONbits.SSPM  = 0b000;             //SPI Master mode, clock = FOSC/4 2Mhz clock frequency @ Fosc 8Mz
   SSPCONbits.WCOL  = clear;             //No collision 
   SSPCONbits.CKP   = clear;
   SSPSTATbits.SMP  = set;               //input data sample at the end of transmission time
   SSPSTATbits.CKE  = set;
    
}

void SPI_MasterTransmit(uint8_t iData) {
    SSPBUF = iData;                      //write data to buffer
    while (!SSPSTATbits.BF);            //wait for data transfer to complete (do not comment out if interrupt is disabled)
}


