/* 
 * File:   Internal_EEPROM.c
 * Author: CEDRIC
 *
 * Created on 16 July 2019, 11:55 PM
 */

#include "myDefinition.h"

uint8_t internal_eeprom_read(uint8_t addr){
  uint8_t *dataRead;
  EEADR = addr;                     //point to the address to read from
  EECON1bits.EEPGD = clear;         //read from program memoty EEPROM
  EECON1bits.RD    = set;           //read sequence enabled
  dataRead = &EEDATA;               //retrieve data from memory location 
  return dataRead;                  //return EEPROM data read
}

void internal_eeprom_write(uint8_t *addr, uint8_t value){
    
}