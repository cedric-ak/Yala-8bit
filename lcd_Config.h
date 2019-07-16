/* 
 * File:   lcd_Config.h
 * Author: CEDRIC
 *
 * Created on 11 July 2019, 7:52 PM
 */

#ifndef LCD_CONFIG_H
#define	LCD_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    /************LCD pin MAP************************/
#define RS PORTBbits.RB5
#define EN PORTBbits.RB4
#define D4 PORTBbits.RB3
#define D5 PORTBbits.RB2
#define D6 PORTBbits.RB1
#define D7 PORTBbits.RB0
    /***********************************************/

    void Lcd_Port(char a);
    void Lcd_Cmd(char a);
    Lcd_Clear();
    void Lcd_Set_Cursor(char raw, char column);
    void Lcd_Init();
    void Lcd_Write_Char(char a);
    void Lcd_write(char *string);
    void Lcd_Shift_Right();
    void Lcd_Shift_Left();

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_CONFIG_H */

