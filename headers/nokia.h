#ifndef __nokia_h__
#define __nokia_h__
void LCD_write_byte(unsigned char dat, unsigned char command);
void LCD_init();
void LCD_clear();
void LCD_set_XY(unsigned char X, unsigned char Y);
void LCD_write_char(unsigned char c);
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
void LCD_joystick(unsigned char X,unsigned char Y,unsigned long);
void delay_ms(int miliSec);
void LCD_clear_Row(unsigned char x);
#endif

