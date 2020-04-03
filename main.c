/* 
 * File:   main.c
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 6:10 PM
 */

#include "configuartion.h"
#include "myDefinition.h"

char buffer[10];
//char *studentNember[10] = "2011xxxxx";
//char studentNmae[10]   = "YourName";

int main(void) {

    system_Initialize();

    while (true) {
#ifdef ASSIGNEMENT_ONE
        for (int dutyCicle = 0; dutyCicle < 100; dutyCicle++) {
            PWM_DutyCycle(dutyCicle);
            __delay_ms(50);
        }

#endif

#ifdef ASSIGNEMENT_TWO
                
#endif

#ifdef ASSIGNEMENT_THREE
#endif

#ifdef ASSIGNEMENT_FOUR
#endif

#ifdef ASSIGNEMENT_FIVE
#endif

    }
    return;
}
