/* 
 * File:   _25LC160B_EEPROM.c
 * Author: CEDRIC
 *
 * Created on 13 July 2019, 10:16 PM
 */

#include "myDefinition.h"

void EEPROM_Write_Data(uint8_t iData, uint8_t iAddress){    
    /*consider interrupt on SPI*/
    CS_Chip_Select;                    //select EEPROM device
    SPI_MasterTransmit(WREN);          //Initiate write enable instruction
    CS_Chip_Desect;                    //deselect EEPROM device
    __delay_ms(2);
    
    CS_Chip_Select;
    __delay_ms(5);                    //wait write internal cycle time
    SPI_MasterTransmit(WRITE);        //Initiate a write instruction
    SPI_MasterTransmit(iAddress>>8);  //assigned high register address MSB
    SPI_MasterTransmit(iAddress);     //assigned low register address LSB
    SPI_MasterTransmit(iData);        //send data to buffer
    CS_Chip_Desect;                   //deselect EEPROM device
}

void EEPROM_Write_String(char *sText, uint8_t iAddress){
    
    for(uint8_t iSize = 0; sText[iSize] != '\0'; iSize++){
        EEPROM_Write_Data(sText[iSize], iAddress++);
    }
} 

uint8_t EEPROM_Read_Data(uint8_t iAddress){
    CS_Chip_Select;                         //select EEPROM device
    __delay_ms(5);                          //wait write internal cycle time before attempt reading
    SPI_MasterTransmit(READ);               //initiate a read instruction
    SPI_MasterTransmit(iAddress>>8);        //assigned high register address MSB
    SPI_MasterTransmit(iAddress);           //assigned low register address LSB
    SPI_MasterTransmit(NULL);               //dummy data
    CS_Chip_Desect;                         //deselect EEPROM device
    return SSPBUF;
}

char EEPROM_Read_String(uint16_t iAddress){
    char dataRead[];
    for(int AddRead = iAddress; AddRead <5; AddRead++){
        dataRead[AddRead] = EEPROM_Read_Data(AddRead);
    }
    return dataRead;
}