#include <avr/io.h>
#include <stdio.h>
#include "./../headers/bit.h"
#include "./../headers/timer.h"
#include "./../headers/io.h"
#include "./../headers/scheduler.h"
#include "./../headers/adc.h"
#include "./../headers/joystick.h"

//gobal variables
//stores the display message for each gear
unsigned char * speed[] = {"Gear 1, Gear 2, Gear 3, Gear 4"};
//stores the currentGear of the tranmission
unsigned char currentGear = 0x00;
//tracks if the car is on or not
unsigned char Ignition = 0x00;
//shifting boolean
unsigned char shifting = 0x00;
//grab the input
unsigned char direction = 0x00;

//JOYSTICK TICK FUNCTION
enum JoyStick_States {Joystick_Start, Wait, Input} joystick_State;

int JoystickTick(int state){
    return state;
}

//Motor TICK FUNCTION

enum Motor_States {Motor_Start, Motor_Off, Motor_On, On_Hold, Off_Hold} motor_State;

int MotorTick(int state){
    unsigned char button = ~PINA & 0x01;
    switch (state) {
        case Motor_Start: //MOTOR START STATE
            state = Motor_Off;
            break;
        case Motor_Off: //MOTOR OFF STATE
            if(button){
                state = On_Hold;
            }
            else{
                state = Motor_Off;
            }
            break;
        case On_Hold: //ON_HOLD STATE
            if(button){
                state = On_Hold;
            }
            else{
                state = Motor_On;
            }
            break;
        case Motor_On: //On STATE
            if(button){
                state = Off_Hold;
            }
            else{
                state = Motor_On;
            }
            break;
        case Off_Hold: //Off_Hold STATE
            if(button){
                state = Off_Hold;
            }
            else{
                state = Motor_Off;
            }
            break;
        default:
            state = Motor_Off;
            break;
    }
    switch (state) {
        case On_Hold:
            Ignition = 0x01;
            break;
        case Off_Hold:
            Ignition =0x00;
        default:
            break;
    }
    
    return state;
}

int main(void){
    //Joystick Register
    DDRA = 0x00; PORTA = 0xFF;
    //LCD SCREEN Register
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    //Button Register
    DDRC = 0x00; PORTC = 0xFF;
    //adc init
    adc_init();
    //lcd init
    LCD_init();
    LCD_ClearScreen();
    
    static task task1, task2;
    task *tasks[] = {&task1,&task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // Period for the tasks
    unsigned long int SMTick1_calc = 100;
    unsigned long int SMTick2_calc = 100;
    //Calculating GCD
    unsigned long int tmpGCD = 1; // Normally initialized to 1
    tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
    //set the GCD
    unsigned long int GCD = tmpGCD;
    //Recalculate GCD periods for scheduler
    unsigned long int Joystick_period = SMTick1_calc/GCD;
    unsigned long int Motor_period = SMTick2_calc/GCD;
    
    //Joystick Task
    task1.state = Joystick_Start;//Task initial state.
    task1.period = Joystick_period;//Task Period.
    task1.elapsedTime = Joystick_period;//Task current elapsed time.
    task1.TickFct = &JoystickTick;//Function pointer for the tick.
    
    //Motor Task
    task1.state = Motor_Start;//Task initial state.
    task1.period = Motor_period;//Task Period.
    task1.elapsedTime = Motor_period;//Task current elapsed time.
    task1.TickFct = &MotorTick;//Function pointer for the tick.
   
    //while loop
    unsigned short i;
    while(1){
        for ( i = 0; i < numTasks; i++ ) {
            // Task is ready to tick
            if ( tasks[i]->elapsedTime == tasks[i]->period ) {
                // Setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // Reset the elapsed time for next tick.
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }
        while(!TimerFlag);
        TimerFlag = 0;

    }
    
    return 0;
}

