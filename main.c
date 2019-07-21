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
                ASS_1();
                break;
                
            case ASSIGNMENT_2:
                Ass_2();
                break;
                
            case ASSIGNMENT_3:
                
                break;
            case ASSIGNMENT_5:
                
                break;
                
            case ASSIGNMENT_6:
                
                break;
                
            case ASSIGNMENT_7:
                
                break;
                
        }
    }
    return;
}

