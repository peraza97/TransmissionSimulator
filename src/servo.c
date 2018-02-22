#include <avr/io.h>
#include <util/delay.h>
#include "./../headers/servo.h"

#define right 2300
#define mid  1500
#define left 1000

//init servo1 on port D5
void servosInit(){
    TCCR3A |= 1 << WGM31 | 1 << COM3A1 | 1 << COM3A0 | 1 << COM3B1 | 1 << COM3B0;
    TCCR3B |= 1 << WGM32 | 1 << WGM33 | 1 << CS31;
    
    ICR3 = 19999;
    
}


void turnServo1(unsigned long int degree){
    
    OCR3A = ICR3 - degree;
    _delay_ms(500);
}
void turnServo2(unsigned long int degree){
    
    OCR3B = ICR3 - degree;
    _delay_ms(500);
}

