#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "./../headers/bit.h"
#include "./../headers/timer.h"
#include "./../headers/scheduler.h"
#include "./../headers/adc.h"
#include "./../headers/joystick.h"
#include "./../headers/queue.h"
#include "./../headers/nokia.h"
#include "./../headers/servo.h"
#include "./../headers/motor.h"


/*gobal variables */

//memory adress storing the current gear in eeprom
#define EEPROM_ADDRESS 0x00

//transmission
unsigned char * mode[] = {"Auto ","Manual "};
unsigned char transmission = 0x00;

//stores the display message for each gear
unsigned char * gears[] = {"  Gear 1", "  Gear 2", "  Gear 3","  Gear 4"};
//stores the currentGear of the tranmission
unsigned char currentGear = 0x00;

//tracks if the car is on or not
unsigned char ignition = 0x00;

//am i currently shifting
unsigned char is_shifting = 0x00;

Queue shiftList;

//Motor TICK FUNCTION
enum Motor_States {Motor_Start, Off, On, On_Hold, Off_Hold} motor_State;

int MotorTick(int state){
    unsigned char button = ~PINA & 0x04;
    switch (state) {
        case Motor_Start: //MOTOR START STATE
            state = Off;
            break;
        case Off: //MOTOR OFF STATE
            if(button){
                if(!transmission){ //want to turn on the motor only if we are in automatic
                updateMotor(currentGear);
                }
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
            ignition = 0x01;
            //enable signal to lcd93
            PORTB |= (0x01 << 1);
            break;
        case Off_Hold:
            ignition = 0x00;
            //disable singal to motor
            PORTB &= ~(0x01 << 1);
            break;
        default:
            break;
    }
    
    return state;
}

//Transmission TICK FUNCTION
enum Transmission_States {Tran_Start, Tran_Auto, Tran_Auto_Hold, Tran_Manual, Tran_Manual_Hold} T_State;

int TransmissionToggleTick(int state){
    unsigned char toggle = ~PINA & 0x08;
    switch (state) {
        case Tran_Start:
            state = Tran_Auto;
            break;
        case Tran_Auto: //tran auto
            if(toggle && !ignition){
                transmission = 1;
                state = Tran_Manual_Hold;
                updateServos(4);
                updateMotor(4);
                LCD_clear();
            }
            else{
                state = Tran_Auto;
            }
            break;
        case Tran_Manual_Hold: //tran manual hold
            if(toggle){
                state = Tran_Manual_Hold;
            }
            else {
                state = Tran_Manual;
            }
            break;
        case Tran_Manual: //tran manual
            if(toggle && !ignition){
                //going to automatic, shift to gear 1
                LCD_clear();
                transmission = 0;
                currentGear = 0;
                updateServos(4);
                updateServos(currentGear);
                updateMotor(currentGear);
                state = Tran_Auto_Hold;
            }
            else{
                state = Tran_Manual;
            }
            break;
        case Tran_Auto_Hold: //tran auto hold
            if(toggle){
                state = Tran_Auto_Hold;
            }
            else{
                state = Tran_Auto;
            }
            break;
        default:
            state = Tran_Start;
            break;
    }
 
    return state;
}

//JOYSTICK TICK FUNCTION
enum JoyStick_States {Joystick_Start, Joystick_Manual, Joystick_Wait, Auto_Shift} joystick_State;

int JoystickTick(int state){
    //variables
    unsigned char stick = getJoystick();
    unsigned char clutchY = ~PINA & 0x20;
    unsigned char clutchX = ~PINA & 0x10;
    //static variables
    static long tempX;
    static long tempY;
    switch (state) {
        case Joystick_Start: //start state
            state = Joystick_Wait;
            tempX = 1500;
            tempY = 1400;
            break;
        case Joystick_Wait: //wait state
            if(transmission == 0 && ignition && stick && !is_shifting){
                if((stick == 1 && currentGear< 3) || (stick == 2 && currentGear > 0)){ //automatic transmission
                    state = Auto_Shift;
                    QueueEnqueue(shiftList,stick);
                }
                else{
                    state = Joystick_Wait;
                }
            }
            else if(transmission == 1 && ignition && (clutchY || clutchX)){             //manual transmission
                state = Joystick_Manual;
            }
    
            else{                                                                       //go to wait state
                state = Joystick_Wait;
            }
            break;
        case Auto_Shift: // Input state
            if(ignition && stick){
                state = Auto_Shift;
            }
            else{
                state = Joystick_Wait;
            }
            break;
        case Joystick_Manual:// Manual state
            if(clutchX || clutchY){
                state = Joystick_Manual;
            }
            else{
                state = Joystick_Wait;
            }
            break;
        default:
            state = Joystick_Wait;
            break;
    }
    //actions
    switch (state) {
        case Joystick_Manual: //update the motors directly
            LCD_write_english_string(0,0,"                                        ");
            //y is up down
            if(clutchY){
                tempY = convertInput(0,adc_read(0));
                OCR3A = ICR3 - tempY;
            }
            //x is left right
            else if(clutchX){
                tempX = convertInput(1,adc_read(1));
                OCR3B = ICR3 - tempX;
            }
            LCD_joystick(10,0,tempY);
            LCD_joystick(50,0,tempX);
            manualMotorUpdate(tempY, tempX);
            break;
        case Joystick_Wait:
            LCD_write_english_string(0,0,"                                        ");
            break;
            
        default:
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
            is_shifting = 1;
            break;
        case Shifter_Shift: ; //shift state
            //update current gear
            if(g == 1){
                currentGear = currentGear + 1;
            }
            else if(g == 2){
                currentGear = currentGear - 1;
            }
            //write to this eeprom
            eeprom_write_byte((uint8_t*)EEPROM_ADDRESS, currentGear);
            //set servos to middle position
            updateServos(4);
            //update the servos
            updateServos(currentGear);
            //update motors
            updateMotor(currentGear);
            break;
        case Shifter_Wait: //waiting state
            is_shifting = 0;
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
            if(ignition){
                LCD_clear();
                state = LCD_display;
            }
            else{
                state = LCD_Off;
            }
            break;
        case LCD_display: //LCD Display
            if(ignition && !is_shifting){
                state = LCD_display;
            }
            else if(ignition && is_shifting){
                LCD_clear();
                state = LCD_shift;
            }
            else{
                LCD_clear();
                state = LCD_Off;
            }
            break;
        case LCD_shift: //LCD Shifting
            if(is_shifting){
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
            LCD_write_english_string(0,5, "                  ");
            LCD_write_english_string(20,5, mode[transmission]);
            break;
        case LCD_display: ; //LCD display
            if(!transmission){
                LCD_write_english_string(0,3,gears[currentGear]);
            }
            else{
                LCD_write_english_string(0,2,"  Manual Mode");
            }
            break;
        case LCD_shift: //LCD shift
            LCD_write_english_string(0,3,"  Currently");
            LCD_write_english_string(0,4,"  Shifting");
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
    
    //get the value in eeprom
    uint8_t ByteOfData;
    ByteOfData = eeprom_read_byte((uint8_t*)EEPROM_ADDRESS);

    if(ByteOfData == 0 || ByteOfData ==1 ||  ByteOfData ==2 || ByteOfData ==3){
        currentGear = ByteOfData;
    }

    //update my servos and motors first thing
    updateServos(4);
    updateServos(currentGear);
    
    //init adc
    adc_init();
    //LCD STUFF
    LCD_init();
    LCD_clear();
    
    //MY QUEUE
    shiftList = QueueInit(4);
    
    static task task1, task2, task3, task4, task5;
    task *tasks[] = {&task1,&task2, &task3, &task4, &task5};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    // Period for the tasks
    unsigned long int SMTick1_calc = 50;
    unsigned long int SMTick2_calc = 50;
    unsigned long int SMTick3_calc = 100;
    unsigned long int SMTick4_calc = 100;
    unsigned long int SMTick5_calc = 50;
    //Calculating GCD
    unsigned long int tmpGCD = 1; // Normally initialized to 1
    tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
    tmpGCD = findGCD(tmpGCD, SMTick3_calc);
    tmpGCD = findGCD(tmpGCD, SMTick4_calc);
    tmpGCD = findGCD(tmpGCD, SMTick5_calc);
    //set the GCD
    unsigned long int GCD = tmpGCD;
    //Recalculate GCD periods for scheduler
    unsigned long int Motor_period = SMTick1_calc/GCD;
    unsigned long int Transmission_period = SMTick2_calc/GCD;
    unsigned long int Joystick_period = SMTick3_calc/GCD;
    unsigned long int Shifter_period = SMTick4_calc/GCD;
    unsigned long int LCD_period = SMTick5_calc/GCD;
    //Motor Task
    task1.state = Motor_Start;//Task initial state.
    task1.period = Motor_period;//Task Period.
    task1.elapsedTime = Motor_period;//Task current elapsed time.
    task1.TickFct = &MotorTick;//Function pointer for the tick.
    
    //Transmission Task
    task2.state = Tran_Start;//Task initial state.
    task2.period = Transmission_period;//Task Period.
    task2.elapsedTime = Transmission_period;//Task current elapsed time.
    task2.TickFct = &TransmissionToggleTick;//Function pointer for the tick.
    
    //Joystick Task
    task3.state = Joystick_Start;//Task initial state.
    task3.period = Joystick_period;//Task Period.
    task3.elapsedTime = Joystick_period;//Task current elapsed time.
    task3.TickFct = &JoystickTick;//Function pointer for the tick.
    
    //Shifter Task
    task4.state = Shifter_Start;//Task initial state.
    task4.period = Shifter_period;//Task Period.
    task4.elapsedTime = Shifter_period;//Task current elapsed time.
    task4.TickFct = &ShifterTick;//Function pointer for the tick.
    
    //LCD Task
    task5.state = LCD_Start;//Task initial state.
    task5.period = LCD_period;//Task Period.
    task5.elapsedTime = LCD_period;//Task current elapsed time.
    task5.TickFct = &LCDTick;//Function pointer for the tick.
    
    
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

