/* 
 * File:   main.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 6:10 PM
 */

#include "configuartion.h"
#include "myDefinition.h"

int main(void) {    
    
    system_Initialize();
    
    while (true) {
        temp_Display();
    }
    return;
}
