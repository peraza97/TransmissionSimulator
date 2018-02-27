#include <avr/io.h>
#include <avr/eeprom.h>
#include "./../src/nokia.c"

#define num_address 0x00


int main(void){
    //initialize the registers
    DDRB = 0xFF; PORTB = 0x00;
    //LCD SCREEN Register
    DDRC = 0xFF; PORTC = 0x00;
    
    uint8_t ByteOfData;
    LCD_init();
    
    //eeprom_write_byte((uint8_t*)num_address, 0x07);
    ByteOfData = eeprom_read_byte((uint8_t*)num_address);

    LCD_clear();
    while(1){
        if(ByteOfData == 7){
            LCD_write_english_string(0,3, "  7");
        }
        else{
            LCD_write_english_string(0,3, "  None");
        }
    }

}
