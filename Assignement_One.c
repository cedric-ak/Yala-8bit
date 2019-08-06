/* 
 * File:   Assignement_One.c
 * Author: CEDRIC
 *
 * Created on 20 July 2019, 5:12 PM
 */

#include "myDefinition.h"

uint32_t *dataBuffer;
bool data_TransferComplete;
char student_Number[] = "20114xxxx";
char student_Name[]   = "YalaTech";

void ASS_1(void) {
   
        for (int bSize = 0; !PIR1bits.RCIF; bSize++) { //continue reading until last character complete
            dataBuffer[bSize] = UART_Read();
            if (PIR1bits.RCIF)
                data_TransferComplete = true;
        }
    

    if (SW2_pressed || data_TransferComplete) {
        for (int bSize = 0; !TXSTAbits.TRMT; bSize++) //continue writing as long as buffer is not empty
            UART_Write_String(dataBuffer[bSize]);
        data_TransferComplete = false;
    }
    return;
}

