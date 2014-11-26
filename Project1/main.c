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

void init(void);
void initTimer(void);
void startTimer(void);
void updateTime();
void displayString(BYTE pos, char* text);
void startTimeEdit(void);
void startAlarmEdit(void);
void updateClock(void);

long int interrupts;
long int clockTicksPerSeconds = INTERRUPTS_PER_SECOND;

typedef enum { false, true } bool;
typedef enum { editHours, editMin, editSec, noEdit } editState;
editState currentEditState;

typedef enum { setTime, setAlarm, display } state;
state currentState;

void main(void)
{
    init();
}

void init(void) {
    BUTTON0_TRIS = 1; //configure button0 as input
    RCONbits.IPEN      = 1;   //enable interrupts priority levels
    INTCON3bits.INT1P  = 1;   //connect INT1 interrupt (button 2) to high prio
    INTCON2bits.INTEDG1= 0;   //INT1 interrupts on falling edge
    INTCONbits.GIE     = 1;   //enable high priority interrupts
    INTCON3bits.INT1E  = 1;   //enable INT1 interrupt (button 2)
    INTCON3bits.INT1F  = 0;   //clear INT1 flag

    BUTTON1_TRIS = 1; //configure button1 as input
    INTCON2bits.INT3IP  = 1;   //connect INT1 interrupt (button 2) to high prio
    INTCON2bits.INTEDG3= 0;   //INT1 interrupts on falling edge
    INTCON3bits.INT3E  = 1;   //enable INT1 interrupt (button 2)
    INTCON3bits.INT3IF  = 0;   //clear INT1 flag

    startTimeEdit();
    LCDInit();
    displayString(0, "Time:  00:00:00");
    displayString(16, "Alarm: 00:00:00");
    initClock();
    initAlarm();
    initTimer();

    updateClock();
}

void initTimer(void)
{
    interrupts = 0;
    T0CONbits.TMR0ON=0; // disable timer0
    T0CONbits.T08BIT=0; // use timer0 8-bit counter
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
   LCDUpdate();
}

void startTimeEdit(void) {
    currentState = setTime;
    currentEditState = editHours;
    disableClock();
    disableAlarm();
}

void startAlarmEdit(void) {
    currentState = setAlarm;
    currentEditState = editHours;
    disableAlarm();
}

void updateClock(void) {
    updateTime();
}

void blinkTimeEdit(char* timeString) {
    if (currentEditState == editHours && ((interrupts / (clockTicksPerSeconds / 2)) % 2) != 0) {
        timeString[0] = ' ';
        timeString[1] = ' ';
    }
    else if (currentEditState == editMin && ((interrupts / (clockTicksPerSeconds / 2)) % 2) != 0) {
        timeString[3] = ' ';
        timeString[4] = ' ';
    }
    else if (currentEditState == editSec && ((interrupts / (clockTicksPerSeconds / 2)) % 2) != 0) {
        timeString[6] = ' ';
        timeString[7] = ' ';
    }
}

void updateTime(void)
{
    char *clockTimeString = getClockTime();
    char *alarmTimeString = getAlarmTime();
//    char debug[16];

    if (currentState == setTime) {
        blinkTimeEdit(clockTimeString);
    }
    else if (currentState == setAlarm) {
        blinkTimeEdit(alarmTimeString);
    }
    displayString(7, clockTimeString);

    displayString(23, alarmTimeString);
//    sprintf(debug, "%ld", interrupts);
//    displayString(16, debug);
}

void startTimer(void)
{
    INTCONbits.T0IF=0;
    T0CONbits.TMR0ON=1;
}

long int getClockTicksPerSecond(void) {
    return clockTicksPerSeconds;
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
        if (ULONG_MAX - 1 < interrupts) {
            interrupts = 0;
        }
        interrupts++;
        tickClock();
        checkAlarm();
    }
    if(INTCON3bits.INT1F)
    {
        INTCON3bits.INT1F  = 0;   //clear INT1 flag
        if(BUTTON0_IO);  //just read the bit
        if (getAlarmState() == alarmSounding) {
            enableAlarm();
        }
        else if (currentState == setTime) {
            if (currentEditState == editHours) {
                addTicksClock(clockTicksPerSeconds * 3600);
            } else if (currentEditState == editMin) {
                addTicksClock(clockTicksPerSeconds * 60);
            } else if (currentEditState == editSec) {
                addTicksClock(clockTicksPerSeconds);
            }
        } else if (currentState == setAlarm) {
            if (currentEditState == editHours) {
                addTicksAlarm(clockTicksPerSeconds * 3600);
            } else if (currentEditState == editMin) {
                addTicksAlarm(clockTicksPerSeconds * 60);
            } else if (currentEditState == editSec) {
                addTicksAlarm(clockTicksPerSeconds);
            }
        } else {
            startTimeEdit();
        }
    }
    else if(INTCON3bits.INT3IF)
    {
        INTCON3bits.INT3IF  = 0;   //clear INT1 flag
        if(BUTTON1_IO);  //just read the bit
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
                    enableClock();
                    if (getAlarmState() == alarmUnset) {
                        startAlarmEdit();
                    }
                    else {
                        enableAlarm();
                        currentEditState = noEdit;
                        currentState = display;
                    }
                }
                else {
                    enableAlarm();
                    currentEditState = noEdit;
                    currentState = display;
                }
            }
        }
        else {
            startAlarmEdit();
        }
    }
    updateClock();
}
