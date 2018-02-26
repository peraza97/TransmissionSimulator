#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "./../headers/bit.h"
#include "./../headers/timer.h"
#include "./../headers/scheduler.h"
#include "./../headers/adc.h"
#include "./../headers/joystick.h"
#include "./../headers/queue.h"
#include "./../headers/nokia.h"
#include "./../headers/servo.h"
#include "./../headers/motor.h"


#define left1 2500
#define mid1  1500
#define right1 600

#define left2 2200
#define mid2  1300
#define right2 500

//gobal variables
//stores the display message for each gear
unsigned char * speed[] = {"  Gear 1", "  Gear 2", "  Gear 3","  Gear 4"};
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
            break;
        case Off: //MOTOR OFF STATE
            if(button){
                state = On_Hold;
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
            PORTB |= (0x01 << 1);
            break;
        case Off_Hold:
            Ignition = 0x00;
            PORTB &= ~(0x01 << 1);
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
                //check if you can even shift
                if((stick == 1 && currentGear< 3) || (stick == 2 && currentGear > 0)){
                state = Shift;
                QueueEnqueue(shiftList,stick);
                }
                else{
                    state = Wait;
                }
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
enum Shifter_States {Shifter_Start, Shifter_Wait, Shifter_Set, Shifter_Shift} shifter_State;
int ShifterTick(int state){
    static unsigned char g;
    switch (state) {
        case Shifter_Start: //start state
            state = Shifter_Wait;
            g = 100;
            break;
        case Shifter_Wait: //wait states
            if(!QueueIsEmpty(shiftList)){
                state = Shifter_Set;
                g = QueueDequeue(shiftList);
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
        case Shifter_Set:; //set state state
            shifting = 1;
            break;
        case Shifter_Shift: ; //shift state
            turnServo2(mid2);
            turnServo1(mid1);
            if(g == 1){
                currentGear = currentGear + 1;
            }
            else{
                currentGear = currentGear - 1;
            }
            //set to position 1
            if(currentGear == 0){
                turnServo2(left2);
                turnServo1(right1);
                motorGear1();
            }
            //set to position 2
            else if(currentGear == 1){
                turnServo2(right2);
                turnServo1(right1);
                motorGear2();
            }
            //set to position 3
            else if(currentGear == 2){
                turnServo2(left2);
                turnServo1(left1);
                motorGear3();
            }
            //set to position 4
            else if(currentGear == 3){
                turnServo2(right2);
                turnServo1(left1);
                motorGear4();
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

//LCD TASK
enum LCD_States {LCD_Start, LCD_Off, LCD_display, LCD_shift} lcd_state;
int LCDTick(int state){
    //transition
    switch (state) {
        case LCD_Start: //LCD Start
            state = LCD_Off;
            break;
        case LCD_Off: //LCD Off
            if(Ignition){
                LCD_clear();
                state = LCD_display;
            }
            else{
                state = LCD_Off;
            }
            break;
        case LCD_display: //LCD Display
            if(Ignition && !shifting){
                state = LCD_display;
            }
            else if(Ignition && shifting){
                LCD_clear();
                state = LCD_shift;
            }
            else{
                LCD_clear();
                state = LCD_Off;
            }
            break;
        case LCD_shift: //LCD Shifting
            if(shifting){
                state = LCD_shift;
            }
            else{
                LCD_clear();
                state = LCD_display;
            }
            break;
        default:
            state = LCD_Off;
            break;
    }
    switch (state) {
        case LCD_Off: //LCD off
            LCD_write_english_string(0,3,"  Motor off");
            break;
        case LCD_display: //LCD display
            LCD_write_english_string(0,3,speed[currentGear]);
            break;
        case LCD_shift: //LCD shift
            LCD_write_english_string(0,3,"Currently Shifting");
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
    DDRC = 0xFF; PORTC = 0x00;
    //SERVOS will be on portB
    DDRB = 0xFF; PORTB = 0x00;
    
    //init servo
    servosInit();
    //init motor
    motorInit();
    
    //TODO
    set the motor to the proper speed
    set the servos to the proper location
    
    
    //init adc
    adc_init();
    //LCD STUFF
    LCD_init();
    LCD_clear();
    
    //MY QUEUE
    shiftList = QueueInit(4);
    
    static task task1, task2, task3, task4;
    task *tasks[] = {&task1,&task2, &task3, &task4};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // Period for the tasks
    unsigned long int SMTick1_calc = 50;
    unsigned long int SMTick2_calc = 100;
    unsigned long int SMTick3_calc = 100;
    unsigned long int SMTick4_calc = 50;
    //Calculating GCD
    unsigned long int tmpGCD = 1; // Normally initialized to 1
    tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
    tmpGCD = findGCD(tmpGCD, SMTick3_calc);
    tmpGCD = findGCD(tmpGCD, SMTick4_calc);
    //set the GCD
    unsigned long int GCD = tmpGCD;
    //Recalculate GCD periods for scheduler
    unsigned long int Motor_period = SMTick1_calc/GCD;
    unsigned long int Joystick_period = SMTick2_calc/GCD;
    unsigned long int Shifter_period = SMTick3_calc/GCD;
    unsigned long int LCD_period = SMTick4_calc/GCD;
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
    
    //LCD Task
    task4.state = LCD_Start;//Task initial state.
    task4.period = LCD_period;//Task Period.
    task4.elapsedTime = LCD_period;//Task current elapsed time.
    task4.TickFct = &LCDTick;//Function pointer for the tick.
    
    
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

