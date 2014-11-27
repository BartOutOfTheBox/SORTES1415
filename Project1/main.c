/*
 * main.c
 *
 *  Created on: 20-nov.-2014
 *      Author: Arne
 */


#define __18F97J60
#define __SDCC__

#include "Include/HardwareProfile.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pic18fregs.h>  //defines the address corresponding to the symbolic
                         //names of the sfr
#include <limits.h>

#include "Include/LCDBlocking.h"
#include "Include/TCPIP_Stack/Delay.h"
#include "main.h"
#include "Clock.h"
#include "Alarm.h"
#include <malloc.h>
#include "time.h"

void init(void);
void initTimer(void);
void initButtons(void);
void initLeds(void);
void startTimer(void);
void updateTime();
void displayString(BYTE pos, char* text);
void startTimeEdit(void);
void startAlarmEdit(void);
void updateClock(void);
void updateLeds(void);
void startDisplay(void);
void checkButtons(void);

unsigned char _MALLOC_SPEC heap[256];

long int interrupts;
volatile bool stateChange;

typedef enum { EditHours, EditMin, EditSec, NoEdit } editState;
editState currentEditState;

typedef enum { SetTime, SetAlarm, Display } state;
state currentState;

bool button1Pressed;
bool button2Pressed;
char previousText[8];

int lastClockSecondsPrint;
int lastAlarmSecondsPrint;

void main(void)
{
    init();
    while(1) {
        checkButtons();
        updateClock();
    }
}

void checkButtons() {
    if (button2Pressed) {
        button2Pressed = False;
        if (isAlarmSounding()) {
            enableAlarm();
        }
        else if (currentState == SetTime) {
            if (currentEditState == EditHours) {
                addSecondsToClock(3600);
            } else if (currentEditState == EditMin) {
                addSecondsToClock(60);
            } else if (currentEditState == EditSec) {
                addSecondsToClock(1);
            }
        } else if (currentState == SetAlarm) {
            if (currentEditState == EditHours) {
                addSecondsToAlarm(3600);
            } else if (currentEditState == EditMin) {
                addSecondsToAlarm(60);
            } else if (currentEditState == EditSec) {
                addSecondsToAlarm(1);
            }
        } else {
            startTimeEdit();
        }
    }
    else if (button1Pressed) {
        button1Pressed = False;
        if (isAlarmSounding()) {
            enableAlarm();
        }
        else if (currentState == SetTime || currentState == SetAlarm) {
            if (currentEditState == EditHours) {
                currentEditState = EditMin;
            } else if (currentEditState == EditMin) {
                currentEditState = EditSec;
            } else if (currentEditState == EditSec) {
                if (currentState == SetTime) {
                    if (!isAlarmSet()) {
                        enableClock();
                        startAlarmEdit();
                    }
                    else {
                        startDisplay();
                    }
                }
                else {
                    startDisplay();
                }
            }
        }
        else {
            startAlarmEdit();
        }
    }
}

void init(void)
{
    _initHeap(heap, 256);

    lastClockSecondsPrint = -1;
    lastAlarmSecondsPrint = -1;
    interrupts            = 0;

    INTCONbits.GIE     = 1;   //enable high priority interrupts
    RCONbits.IPEN      = 1;   //enable interrupts priority levels

    initButtons();
    initLeds();
    LCDInit();

    startTimeEdit();
    initClock();
    initAlarm();
    initTimer();

    stateChange = True;
}

void initButtons(void)
{
    button2Pressed      = False;
    BUTTON0_TRIS        = 1; //configure button0 as input
    INTCON3bits.INT1P   = 1;   //connect INT1 interrupt (button 2) to high prio
    INTCON2bits.INTEDG1 = 0;   //INT1 interrupts on falling edge
    INTCON3bits.INT1E   = 1;   //enable INT1 interrupt (button 2)
    INTCON3bits.INT1F   = 0;   //clear INT1 flag

    button1Pressed      = False;
    BUTTON1_TRIS        = 1; //configure button1 as input
    INTCON2bits.INT3IP  = 1;   //connect INT1 interrupt (button 1) to high prio
    INTCON2bits.INTEDG3 = 0;   //INT1 interrupts on falling edge
    INTCON3bits.INT3E   = 1;   //enable INT1 interrupt (button 1)
    INTCON3bits.INT3IF  = 0;   //clear INT1 flag
}

void initLeds(void)
{
    TRISJbits.TRISJ0    = 0; // configure PORTJ0 for output (LED)
    TRISJbits.TRISJ1    = 0; // configure PORTJ1 for output (LED)
    LATJbits.LATJ0      = 0;
    LATJbits.LATJ1      = 0;
}

void initTimer(void)
{
    TMR0H = 0x00000000;
    TMR0L = 0x00000000;
    T0CONbits.TMR0ON=0; // disable timer0
    T0CONbits.T08BIT=1; // use timer0 8-bit counter
    T0CONbits.T0CS=0; // use timer0 instruction cycle clock
    T0CONbits.PSA=1; // disable timer0 prescaler
    INTCONbits.T0IE=1; // enable timer0 interrupts

    startTimer();
}


/*************************************************
 Function displayString:
 Writes the first characters of the string in the remaining
 space of the 32 positions LCD, starting at pos
 (does not use strlcopy, so can use up to the 32th place)
*************************************************/

void displayString(BYTE pos, char* text)
{
   BYTE        l = strlen(text);/*number of actual chars in the string*/
   BYTE      max = 32-pos;    /*available space on the lcd*/
   char       *d = (char*)&LCDText[pos];
   const char *s = text;
   size_t      n = (l<max)?l:max;
   /* Copy as many bytes as will fit */
    if (n != 0)
      while (n-- != 0)*d++ = *s++;
    //LCDUpdate();
}

void startTimeEdit(void) {
    currentState = SetTime;
    currentEditState = EditHours;
    disableClock();
    disableAlarm();
    stateChange = True;
}

void startAlarmEdit(void) {
    currentState = SetAlarm;
    currentEditState = EditHours;
    disableAlarm();
    stateChange = True;
}

void startDisplay(void) {
    currentState = Display;
    currentEditState = NoEdit;
    enableClock();
    enableAlarm();
    stateChange = True;
}

void updateClock(void) {
    checkAlarm();

    if (stateChange) {
        stateChange = False;
        LCDErase();
        if (currentState == Display) {
            displayString(0, "Time:");
            displayString(16, "Alarm:");
        }
        else if (currentState == SetAlarm) {
            displayString(0, "Enter the alarm");
            displayString(16, "Alarm:");
        }
        else if (currentState == SetTime) {
            displayString(16, "Enter the time");
            displayString(0, "Time:");
        }
    }
    updateTime();
    updateLeds();
}

void blinkTimeEdit(char* timeString) {
    if (currentEditState == EditHours && ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[0] = ' ';
        timeString[1] = ' ';
    }
    else if (currentEditState == EditMin && ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[3] = ' ';
        timeString[4] = ' ';
    }
    else if (currentEditState == EditSec && ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[6] = ' ';
        timeString[7] = ' ';
    }
}

void updateTime(void)
{
    bool update = False;
    time_t *clockTimeStruct = updateAndGetClockTime();
    time_t *alarmTimeStruct = updateAndGetAlarmTime();
    char clockTimeString[8];
    char alarmTimeString[8];

    sprintf(clockTimeString, "%02d:%02d:%02d", clockTimeStruct->hours, clockTimeStruct->minutes, clockTimeStruct->seconds);
    sprintf(alarmTimeString, "%02d:%02d:%02d", alarmTimeStruct->hours, alarmTimeStruct->minutes, alarmTimeStruct->seconds);

    if (currentState == SetTime) {
        blinkTimeEdit(clockTimeString);
        displayString(7, clockTimeString);
        update = True;
    }
    else if (currentState == SetAlarm) {
        blinkTimeEdit(alarmTimeString);
        displayString(23, alarmTimeString);
        update = True;
    }
    else {
        if (lastClockSecondsPrint != clockTimeStruct->secondsOfTheDay) {
            update = True;
        }
        if (lastAlarmSecondsPrint != alarmTimeStruct->secondsOfTheDay) {
            update = True;
        }
        displayString(7, clockTimeString);
        displayString(23, alarmTimeString);
        lastClockSecondsPrint = clockTimeStruct->secondsOfTheDay;
        lastAlarmSecondsPrint = alarmTimeStruct->secondsOfTheDay;
    }
    if (update) {
        LCDUpdate();
    }
}

void updateLeds(void) {
    if (showClockLed()) {
        LATJbits.LATJ0=1; // switch LED on
    }
    else {
        LATJbits.LATJ0=0; // switch LED on
    }
    if (showAlarmLed()) {
        LATJbits.LATJ1=1; // switch LED off
    }
    else {
        LATJbits.LATJ1=0; // switch LED off
    }
}

void startTimer(void)
{
    INTCONbits.T0IF=0;
    T0CONbits.TMR0ON=1;
}

long int getInterrupts(void) {
    return interrupts;
}

void LowISR(void) __interrupt 2
{
    //put the code here
}

void HighISR(void) __interrupt 1
{
    if (INTCONbits.T0IF) { // Timer0 Interrupt
        INTCONbits.T0IF = 0;
        startTimer();
        if (LONG_MAX - 1 < interrupts) {
            interrupts = 0;
        }
        interrupts++;
        tickClock();
    }
    if(INTCON3bits.INT1F)
    {
        INTCON3bits.INT1F  = 0;   //clear INT1 flag
        if(BUTTON0_IO);  //just read the bit
        button2Pressed = True;
    }
    else if(INTCON3bits.INT3IF)
    {
        INTCON3bits.INT3IF  = 0;   //clear INT1 flag
        if(BUTTON1_IO);  //just read the bit
        button1Pressed = True;
    }
    //updateClock();
}
