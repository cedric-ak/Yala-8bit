/* 
 * File:   _25LC160B_EEPROM.h
 * Author: CEDRIC
 *
 * Created on 15 July 2019, 12:15 AM
 */

#ifndef _25LC160B_EEPROM_H
#define	_25LC160B_EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

#define WREN    0x06        //Set the write enable latch (enable write operations)
#define WRITE   0x02        //Write data to memory array beginning at selected address
#define READ    0x03        //Read data from memory array beginning at selected address
#define WRDI    0x04        //Reset the write enable latch (disable write operations)
#define RDSR    0x05        //Read STATUS register
#define WRSR    0x01        //Write STATUS register
    
extern void EEPROM_Write(uint8_t *iData, uint8_t iAddress);
extern void EEPROM_Write_String(char *sText, uint8_t iAddress);
extern uint8_t EEPROM_Read(uint8_t iAddress);
char EEPROM_Read_String(uint16_t iAddress);


#ifdef	__cplusplus
}
#endif

#endif	/* _25LC160B_EEPROM_H */

