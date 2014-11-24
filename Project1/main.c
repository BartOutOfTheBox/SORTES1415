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
#include <pic18fregs.h>  //defines the address corresponding to the symbolic
                         //names of the sfr

#include "Include/LCDBlocking.h"
#include "Include/TCPIP_Stack/Delay.h"

#define CLOCK_FREQ 30000000 // 30 Mhz
#define EXEC_FREQ CLOCK_FREQ/4 // 4 clock cycles to execute an instruction

#define INTERRUPTS_PER_SECOND 95

unsigned long int interrupts;
unsigned long int lastSeconds = 0;

typedef enum { false, true } bool;

void init(void);
void clockInit(void);
void startTimer(void);
void updateTime();
void displayString(BYTE pos, char* text);
void startTimeEdit(void);

bool hourEdit = false;
bool minEdit = false;
bool secEdit = false;
bool blinkState = false;

long int lastBlinkChange;

typedef enum { setTime, running } state;
state currentState;

void main(void)
{
    init();
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

void init(void) {
    startTimeEdit();
    LCDInit();
    DelayMs(100);
    displayString(0, "Time: 00:00:00");
    clockInit();
}

void startTimeEdit(void) {
    currentState = setTime;
    hourEdit = true;
    blinkState = true;
    lastBlinkChange = interrupts;
}

void updateTime(void)
{
    unsigned long int seconds;
    char time[8];
    char hoursChar[3];
    char minutesChar[3];
    char secondsChar[3];
    int hours;
    int min;
    int sec;
    bool blinkUpdated = false;

    seconds = interrupts / INTERRUPTS_PER_SECOND;

    if (currentState == setTime) {
        if (lastBlinkChange + INTERRUPTS_PER_SECOND / 2 >= interrupts) {
            blinkState = !blinkState;
            lastBlinkChange = interrupts;
            blinkUpdated = true;
        }
    }

    if (seconds == lastSeconds && !blinkUpdated) {
        return;
    }
    if (seconds == 86400) {
        seconds = 0;
        interrupts = 0;
    }
    lastSeconds = seconds;
    hours = seconds / 3600;
    min = (seconds % 3600) / 60;
    sec = (seconds % 3600) % 60;

    if (currentState == setTime && hourEdit && !blinkState) {
        sprintf(hoursChar, "  ");
    }
    else {
        sprintf(hoursChar, "%02d", hours);
    }
    if (currentState == setTime && minEdit && !blinkState) {
        sprintf(minutesChar, "  ");
    }
    else {
        sprintf(minutesChar, "%02d", min);
    }
    if (currentState == setTime && secEdit && !blinkState) {
        sprintf(secondsChar, "  ");
    }
    else {
        sprintf(secondsChar, "%02d", sec);
    }

    sprintf(time, "%s:%s:%s", hoursChar, minutesChar, secondsChar);
    displayString(6, time);
}

void clockInit(void)
{
    interrupts = 987654;
    T0CONbits.TMR0ON=0; // disable timer0
    T0CONbits.T08BIT=0; // use timer0 16-bit counter
    T0CONbits.T0CS=0; // use timer0 instruction cycle clock
    T0CONbits.PSA=1; // disable timer0 prescaler
    INTCONbits.T0IE=1; // enable timer0 interrupts
    INTCONbits.GIE=1;   //enable high priority interrupts

    updateTime();
}

void startTimer(void)
{
    INTCONbits.T0IF=0;
    T0CONbits.TMR0ON=1;
}

void LowISR(void) __interrupt 2
{
    //put the code here
}

void HighISR(void) __interrupt 1
{
    if (INTCONbits.T0IF) { // Timer0 Interrupt
        startTimer();
        interrupts++;
        updateTime();
    }
}
