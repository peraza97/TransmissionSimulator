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

//84 columns
//48 rows

//5 rows max
//14 characters per line

//to write word on the same line
/*
LCD_write_english_string(0,0,"H");
LCD_write_english_string(6,0,"e");
LCD_write_english_string(12,0,"l");
LCD_write_english_string(18,0,"l");
LCD_write_english_string(24,0,"0 ");
 */


int main(void){
    //initialize the registers
    DDRB = 0xFF; PORTB = 0x00;
    //LCD SCREEN Register
    DDRC = 0xFF; PORTC = 0x00;
    LCD_init();
    while(1){
        //LCD_clear_Row(0);
    }
    
}
