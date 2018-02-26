#include <avr/io.h>
#include <avr/eeprom.h>

#define num_address 0x00


int main(void){
    //initialize the registers
    DDRB = 0xFF; PORTB = 0x00;
    
    uint8_t ByteOfData;
    
    //eeprom_write_byte((uint8_t*)num_address, 0x07);
    ByteOfData = eeprom_read_byte((uint8_t*)num_address);

    while(1){
        
        PORTB = ByteOfData;
        
        
    }

}
