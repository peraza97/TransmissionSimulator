#include <avr/io.h>
#include <stdio.h>
#include "./../headers/bit.h"
#include "./../headers/timer.h"
#include "./../headers/io.h"
#include "./../headers/scheduler.h"
#include "./../headers/adc.h"
#include "./../headers/joystick.h"
#include "./../headers/queue.h"

//gobal variables
//stores the display message for each gear
unsigned char * speed[] = {"Gear 1", "Gear 2", "Gear 3","Gear 4"};
//stores the currentGear of the tranmission
unsigned char currentGear = 0x00;
//tracks if the car is on or not
unsigned char Ignition = 0x00;
//am i currently shifting
unsigned char shifting = 0x00;

Queue shiftList;

//Motor TICK FUNCTION
enum Motor_States {Motor_Start, Off, On, On_Hold, Off_Hold} motor_State;

int MotorTick(int state){
    unsigned char button = ~PINA & 0x02;
    switch (state) {
        case Motor_Start: //MOTOR START STATE
            state = Off;
            LCD_DisplayString(1,"Motor Off");
            break;
        case Off: //MOTOR OFF STATE
            if(button){
                state = On_Hold;
                LCD_DisplayString(1,speed[currentGear]);
            }
            else{
                state = Off;
            }
            break;
        case On_Hold: //ON_HOLD STATE
            if(button){
                state = On_Hold;
            }
            else{
                state = On;
            }
            break;
        case On: //On STATE
            if(button){
                LCD_DisplayString(1,"Motor Off");
                state = Off_Hold;
            }
            else{
                state = On;
            }
            break;
        case Off_Hold: //Off_Hold STATE
            if(button){
                state = Off_Hold;
            }
            else{
                state = Off;
            }
            break;
        default:
            state = Off;
            break;
    }
    switch (state) {
        case On_Hold:
            Ignition = 0x01;
            break;
        case Off_Hold:
            Ignition = 0x00;
            break;
        default:
            break;
    }
    
    return state;
}

//JOYSTICK TICK FUNCTION
enum JoyStick_States {Joystick_Start, Wait, Shift} joystick_State;

int JoystickTick(int state){
    unsigned char stick = getJoystick();
    switch (state) {
        case Joystick_Start: //start state
            state = Wait;
            break;
        case Wait: //wait state
            if(Ignition && stick && !shifting){
                state = Shift; //go to input amd push it onto the queue
                QueueEnqueue(shiftList,stick);
            }
            else{
                state = Wait;
            }
            break;
        case Shift: // Input state
            if(Ignition && stick){
                state = Shift;
            }
            else{
                state = Wait;
            }
            break;
        default:
            state = Wait;
            break;
    }
    return state;
}

//SHIFTER TICK FUNCTION
enum Shifter_States {Shifter_Start, Shifter_Wait, Shifter_Shift} shifter_State;

int ShifterTick(int state){
    switch (state) {
        case Shifter_Start: //start state
            state = Shifter_Wait;
            break;
        case Shifter_Wait: //wait states
            if(!QueueIsEmpty(shiftList)){
                state = Shifter_Shift;
                shifting = 1;
            }
            else{
                state = Shifter_Wait;
            }
            break;
        case Shifter_Shift: //shifting
            state = Shifter_Wait;
            shifting = 0;
            break;
        default:
            state = Shifter_Wait;
            break;
    }
    switch (state) {
        case Shifter_Shift: ;
            unsigned char g = QueueDequeue(shiftList);
            //call some function to get to that gear
            if(g == 1){
                currentGear = currentGear >= 3 ? 3: currentGear + 1;
                
            }
            else{
                currentGear = currentGear > 0 ? currentGear - 1: 0;
            }
            LCD_DisplayString(1,speed[currentGear]);
            break;
        default:
            break;
    }
    
    return state;
}




int main(void){
    //Joystick Register/Button Register
    DDRA = 0x00; PORTA = 0xFF;
    //LCD SCREEN Register
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    adc_init();
    LCD_init();
    LCD_ClearScreen();
    
    //MY QUEUE
    shiftList = QueueInit(4);
    
    static task task1, task2, task3;
    task *tasks[] = {&task1,&task2, &task3};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // Period for the tasks
    unsigned long int SMTick1_calc = 50;
    unsigned long int SMTick2_calc = 100;
    unsigned long int SMTick3_calc = 100;
    //Calculating GCD
    unsigned long int tmpGCD = 1; // Normally initialized to 1
    tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
    tmpGCD = findGCD(tmpGCD, SMTick3_calc);
    //set the GCD
    unsigned long int GCD = tmpGCD;
    //Recalculate GCD periods for scheduler
    unsigned long int Motor_period = SMTick1_calc/GCD;
    unsigned long int Joystick_period = SMTick2_calc/GCD;
    unsigned long int Shifter_period = SMTick3_calc/GCD;
    //Motor Task
    task1.state = Motor_Start;//Task initial state.
    task1.period = Motor_period;//Task Period.
    task1.elapsedTime = Motor_period;//Task current elapsed time.
    task1.TickFct = &MotorTick;//Function pointer for the tick.
    
    //Joystick Task
    task2.state = Joystick_Start;//Task initial state.
    task2.period = Joystick_period;//Task Period.
    task2.elapsedTime = Joystick_period;//Task current elapsed time.
    task2.TickFct = &JoystickTick;//Function pointer for the tick.
    
    //Shifter Task
    task3.state = Shifter_Start;//Task initial state.
    task3.period = Shifter_period;//Task Period.
    task3.elapsedTime = Shifter_period;//Task current elapsed time.
    task3.TickFct = &ShifterTick;//Function pointer for the tick.
    
    TimerSet(GCD);
    TimerOn();
   
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

