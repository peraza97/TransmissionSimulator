#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "./../headers/timer.h"

unsigned char H = 50;
unsigned char L = 50;




//PORTB
/*
 B1 and B2 control direction
 B3 is the pwm timer, controls speed
 */

//1000 ms period
//10ms synch period

/*
 h-20
 l-80
 20% duty cycle
 */

enum Motor_States {Motor_Init,Motor_High,Motor_Low} motor_state;


int Motor_tick(int state){
    static unsigned i;
    switch(state){
        case Motor_Init: //Init
            state = Motor_High;
            i = 0;
            PORTB = 0x00;
            break;
        case Motor_High: //high state
            if(i<= H){
                state = Motor_High;
            }
            else{
                state = Motor_Low;
                i = 0;
                PORTB = 0x00;
            }
            break;
        case Motor_Low: // motor low state
            if(i<= L){
                state = Motor_Low;
            }
            else{
                state = Motor_High;
                i = 0;
                PORTB = 0x03;
            }
            break;
        default:
            break;
    }
    switch (state) {
        case Motor_High: //motor high state
            i++;
            break;
        case Motor_Low: //motot low state
            i++;
            break;
        default:
            break;
    }
    
    return state;
}


int main(){
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(10);
    TimerOn();
    motor_state = Motor_Init;

    
    PORTB |= 0x02;
    
    
    TCCR0A |= 1 << COM0A1 | 1 << WGM00 | 1 << WGM01;
    TCCR0B |= 1 << CS01;
    OCR0A = 200;
    
    while(1){
        /*
        motor_state = Motor_tick(motor_state);
        while(!TimerFlag);
        TimerFlag = 0;
         */
    }
    
    return 0;
}

