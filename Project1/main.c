/*
 * main.c
 *
 *  Created on: 20-nov.-2014
 *      Author: Arne
 */


#define __18F97J60
#define __SDCC__
#define INTERRUPTS_PER_SECOND 24318

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

void init(void);
void initTimer(void);
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
long int ticksPerSecond = INTERRUPTS_PER_SECOND;
volatile bool stateChange;

typedef enum { editHours, editMin, editSec, noEdit } editState;
editState currentEditState;

typedef enum { setTime, setAlarm, display } state;
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
        button2Pressed = false;
        if (getAlarmState() == alarmSounding) {
            enableAlarm();
        }
        else if (currentState == setTime) {
            if (currentEditState == editHours) {
                addSecondsClock(3600);
            } else if (currentEditState == editMin) {
                addSecondsClock(60);
            } else if (currentEditState == editSec) {
                addSecondsClock(1);
            }
        } else if (currentState == setAlarm) {
            if (currentEditState == editHours) {
                addSecondsAlarm(3600);
            } else if (currentEditState == editMin) {
                addSecondsAlarm(60);
            } else if (currentEditState == editSec) {
                addSecondsAlarm(1);
            }
        } else {
            startTimeEdit();
        }
    }
    else if (button1Pressed) {
        button1Pressed = false;
        if (getAlarmState() == alarmSounding) {
            enableAlarm();
        }
        else if (currentState == setTime || currentState == setAlarm) {
            if (currentEditState == editHours) {
                currentEditState = editMin;
            } else if (currentEditState == editMin) {
                currentEditState = editSec;
            } else if (currentEditState == editSec) {
                if (currentState == setTime) {
                    if (getAlarmState() == alarmUnset) {
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

void init(void) {
    int lastClockSecondsPrint = -1;
    int lastAlarmSecondsPrint = -1;

    _initHeap(heap, 256);
    button2Pressed = false;
    BUTTON0_TRIS = 1; //configure button0 as input
    RCONbits.IPEN      = 1;   //enable interrupts priority levels
    INTCON3bits.INT1P  = 1;   //connect INT1 interrupt (button 2) to high prio
    INTCON2bits.INTEDG1= 0;   //INT1 interrupts on falling edge
    INTCONbits.GIE     = 1;   //enable high priority interrupts
    INTCON3bits.INT1E  = 1;   //enable INT1 interrupt (button 2)
    INTCON3bits.INT1F  = 0;   //clear INT1 flag

    button1Pressed = false;
    BUTTON1_TRIS = 1; //configure button1 as input
    INTCON2bits.INT3IP  = 1;   //connect INT1 interrupt (button 1) to high prio
    INTCON2bits.INTEDG3 = 0;   //INT1 interrupts on falling edge
    INTCON3bits.INT3E   = 1;   //enable INT1 interrupt (button 1)
    INTCON3bits.INT3IF  = 0;   //clear INT1 flag

    TRISJbits.TRISJ0=0; // configure PORTJ0 for output (LED)
    TRISJbits.TRISJ1=0; // configure PORTJ1 for output (LED)
    LATJbits.LATJ0=0;
    LATJbits.LATJ1=0;

    LCDInit();
    startTimeEdit();
    initClock();
    initAlarm();
    initTimer();

    stateChange = true;
}

void initTimer(void)
{
    interrupts = 0;
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
    currentState = setTime;
    currentEditState = editHours;
    disableClock();
    disableAlarm();
    stateChange = true;
}

void startAlarmEdit(void) {
    currentState = setAlarm;
    currentEditState = editHours;
    disableAlarm();
    stateChange = true;
}

void startDisplay(void) {
    currentState = display;
    currentEditState = noEdit;
    enableClock();
    enableAlarm();
    stateChange = true;
}

void updateClock(void) {
    checkAlarm();

    if (stateChange) {
        stateChange = false;
        LCDErase();
        if (currentState == display) {
            displayString(0, "Time:");
            displayString(16, "Alarm:");
        }
        else if (currentState == setAlarm) {
            displayString(0, "Enter the alarm");
            displayString(16, "Alarm:");
        }
        else if (currentState == setTime) {
            displayString(16, "Enter the time");
            displayString(0, "Time:");
        }
    }
    updateTime();
    updateLeds();
}

void blinkTimeEdit(char* timeString) {
    if (currentEditState == editHours && ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[0] = ' ';
        timeString[1] = ' ';
    }
    else if (currentEditState == editMin && ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[3] = ' ';
        timeString[4] = ' ';
    }
    else if (currentEditState == editSec && ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[6] = ' ';
        timeString[7] = ' ';
    }
}

void updateTime(void)
{
    bool update = false;
    time *clockTimeStruct = updateAndGetClockTime();
    time *alarmTimeStruct = updateAndGetAlarmTime();
    char clockTimeString[8];
    char alarmTimeString[8];

    sprintf(clockTimeString, "%02d:%02d:%02d", clockTimeStruct->hours, clockTimeStruct->minutes, clockTimeStruct->seconds);
    sprintf(alarmTimeString, "%02d:%02d:%02d", alarmTimeStruct->hours, alarmTimeStruct->minutes, alarmTimeStruct->seconds);

    if (currentState == setTime) {
        blinkTimeEdit(clockTimeString);
        displayString(7, clockTimeString);
        update = true;
    }
    else if (currentState == setAlarm) {
        blinkTimeEdit(alarmTimeString);
        displayString(23, alarmTimeString);
        update = true;
    }
    else {
        if (lastClockSecondsPrint != clockTimeStruct->secondsOfTheDay) {
            update = true;
        }
        if (lastAlarmSecondsPrint != alarmTimeStruct->secondsOfTheDay) {
            update = true;
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
        button2Pressed = true;
    }
    else if(INTCON3bits.INT3IF)
    {
        INTCON3bits.INT3IF  = 0;   //clear INT1 flag
        if(BUTTON1_IO);  //just read the bit
        button1Pressed = true;
    }
    //updateClock();
}
