#include <avr/io.h>
#include <util/delay.h>
#include "./../headers/servo.h"

#define right 2300
#define mid  1500
#define left 1000

//init servo1 on port D5
void servo1Init(){
    TCCR3A |= 1 << WGM31 | 1 << COM3A1 | 1 << COM3A0;
    TCCR3B |= 1 << WGM32 | 1 << WGM33 | 1 << CS31;
    
    ICR3 = 19999;
    
}

void servo2Init(){
    
    
}


void turnServo1(unsigned long int degree){
    
    OCR3A = ICR3 - degree;
    _delay_ms(500);
    
    
}


/*
 int ShifterTick(int state){
 switch (state) {
 case Shifter_Start: //start state
 state = Shifter_Wait;
 break;
 case Shifter_Wait: //wait states
 if(!QueueIsEmpty(shiftList)){
 state = Shifter_Set;
 }
 else{
 state = Shifter_Wait;
 }
 break;
 case Shifter_Set: //set shift to 1
 state = Shifter_Shift;
 break;
 case Shifter_Shift: //shift motors
 state = Shifter_Wait;
 break;
 default:
 state = Shifter_Wait;
 break;
 }
 switch (state) {
 case Shifter_Set:; //shifting state
 shifting = 1;
 case Shifter_Shift: ;
 unsigned char g = QueueDequeue(shiftList);
 if(g == 1){
 currentGear = currentGear + 1;
 }
 else{
 currentGear = currentGear - 1;
 }
 break;
 case Shifter_Wait: //waiting state
 shifting = 0;
 break;
 default:
 break;
 }
 
 return state;
 }
 
 */
