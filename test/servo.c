#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define left1 2500
#define mid1  1500
#define right1 600

#define left2 2200
#define mid2  1300
#define right2 500


int main(void){
    //initialize the registers
    DDRB = 0xFF; PORTB = 0x00;
    TCCR3A |= 1 << WGM31 | 1 << COM3A1 | 1 << COM3A0 | 1 << COM3B1 | 1 << COM3B0;
    TCCR3B |= 1 << WGM32 | 1 << WGM33 | 1 << CS31;
    ICR3 = 19999;
    
    while(1){
        
    OCR3A = ICR3 - right1;
    _delay_ms(3000);
    OCR3A = ICR3 - left1;
    _delay_ms(3000);
        
    OCR3B = ICR3 - left2;
    _delay_ms(3000);
    OCR3B = ICR3 - right2;
    _delay_ms(3000);
    }
    
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
