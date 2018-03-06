#include <avr/io.h>
#include "./../src/nokia.c"

/*
 void LCD_write_byte(unsigned char dat, unsigned char command);
 void LCD_init();
 void LCD_clear();
 void LCD_set_XY(unsigned char X, unsigned char Y);
 void LCD_write_char(unsigned char c);
 void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
 void LCD_joystick(unsigned char X,unsigned char Y,unsigned long);
 void delay_ms(int miliSec);
*/

int main(void){
    //initialize the registers
    DDRB = 0xFF; PORTB = 0x00;
    //LCD SCREEN Register
    DDRC = 0xFF; PORTC = 0x00;
    LCD_init();

    LCD_clear();
    while(1){
        LCD_write_english_string(0,0,"Hello");
        LCD_write_english_string(0,1,"Omar");
         LCD_write_english_string(0,2,"P");
    }
    
}
