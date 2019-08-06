/* 
 * File:   _25LC160B_EEPROM.c
 * Author: CEDRIC
 *
 * Created on 13 July 2019, 10:16 PM
 */

#include "myDefinition.h"

void eeprom_Write(uint8_t iData, uint8_t iAddress){    
    /*consider interrupt on SPI*/
    CS_Chip_Select;                    //select EEPROM device
    SPI_MasterTransmit(WEN);          //Initiate write enable instruction
    CS_Chip_Desect;                    //deselect EEPROM device
    __delay_ms(2);
    
    CS_Chip_Select;
    SPI_MasterTransmit(WRITE);        //Initiate a write instruction
    SPI_MasterTransmit(iAddress>>8);  //assigned high register address MSB
    SPI_MasterTransmit(iAddress);     //assigned low register address LSB
    SPI_MasterTransmit(iData);        //send data to buffer
    CS_Chip_Desect;                   //deselect EEPROM device
    __delay_ms(5); //wait write internal cycle time
}

void eeprom_Write_String(char *sText, uint8_t iAddress){
    
//    for(uint8_t iSize = 0; sText[iSize] != '\0'; iSize++){
//        eeprom_Write_String(sText[iSize], iAddress++);
//    }
} 

uint8_t eeprom_Read(uint8_t iAddress){
    uint8_t dataRead;
    CS_Chip_Select;                         //select EEPROM device
    SPI_MasterTransmit(READ);               //initiate a read instruction
    SPI_MasterTransmit(iAddress>>8);        //assigned high register address MSB
    SPI_MasterTransmit(iAddress);           //assigned low register address LSB
    SPI_MasterTransmit(NULL);               //dummy data
    CS_Chip_Desect;                         //deselect EEPROM device
    dataRead = SSPBUF;
    return dataRead;
}

char eeprom_Read_String(uint16_t iAddress){
    char dataRead[];
    for(int AddRead = iAddress; AddRead <5; AddRead++){
        dataRead[AddRead] = eeprom_Read(AddRead);
    }
    return dataRead;
}