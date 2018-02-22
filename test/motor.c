#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define right 2300
#define mid  1500
#define left 1000


int main(void){
    DDRB = 0xFF; PORTB = 0x00;
    TCCR3A |= 1 << WGM31 | 1 << COM3A1 | 1 << COM3A0 | 1 << COM3B1 | 1 << COM3B0;
    TCCR3B |= 1 << WGM32 | 1 << WGM33 | 1 << CS31;
    ICR3 = 19999;
    
    OCR3A = ICR3 - mid;
    _delay_ms(1000);
    OCR3A = ICR3 - right;
    _delay_ms(1000);

    
    
    OCR3B = ICR3 - left;
    _delay_ms(1000);
    OCR3B = ICR3 - mid;
    _delay_ms(1000);
    
}


/*
 TCCR3A |= 1 << WGM31 | 1 << COM3A1 | 1 << COM3A0;
 TCCR3B |= 1 << WGM32 | 1 << WGM33 | 1 << CS31;
 
 ICR3 = 19999;
 OCR3A = ICR3 - right;
 _delay_ms(1000);
 OCR3A = ICR3 - mid;
 _delay_ms(1000);
 OCR3A = ICR3 - left;
 _delay_ms(1000);
 OCR3A = ICR3 - mid;
 _delay_ms(1000);
 */
