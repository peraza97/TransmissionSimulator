#include <avr/io.h>
#include <util/delay.h>
#include "./../headers/motor.h"

void motorInit(){
    TCCR0A |= 1 << COM0A1 | 1 << WGM00 | 1 << WGM01;
    TCCR0B |= 1 << CS01;
}

void motorChangeGear(unsigned char x){
    OCR0A = x;
}

/*
void motorGear1(){
        OCR0A = 100;
}
void motorGear2(){
        OCR0A = 128;
}
void motorGear3(){
        OCR0A = 200;
}
void motorGear4(){
        OCR0A = 255;
}
 */
