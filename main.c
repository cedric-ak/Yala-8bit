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
        
        switch(Get_Application()){
            case ASSIGNMENT_1:
                
                set_Application(ASSIGNMENT_2);
                break;
                
            case ASSIGNMENT_2:
                set_Application(ASSIGNMENT_3);
                break;
                
            case ASSIGNMENT_3:
                set_Application(ASSIGNMENT_5);
                break;
            case ASSIGNMENT_5:
                set_Application(ASSIGNMENT_6);
                break;
                
            case ASSIGNMENT_6:
                set_Application(ASSIGNMENT_7);
                break;
                
            case ASSIGNMENT_7:
                set_Application(ASSIGNMENT_1);
                break;
                
        }
    }
    return;
}

