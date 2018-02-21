#include <avr/io.h>
#include <util/delay.h>

#define right 2300
#define mid  1500
#define left 1000

int main(void){
    DDRD = 0xFF; PORTD = 0x00;
    TCCR1A |= 1 << WGM11 | 1 << COM1A1 | 1 << COM1A0;
    TCCR1B |= 1 << WGM12 | 1 << WGM13 | 1 << CS11;
    
    ICR1 = 19999;
    
    OCR1A = ICR1 - right;
    _delay_ms(1000);
    OCR1A = ICR1 - mid;
    _delay_ms(1000);
    OCR1A = ICR1 - left;
    _delay_ms(1000);
    OCR1A = ICR1 - mid;
    _delay_ms(1000);
    while(1){
        
    }
    
    return 0;
}
