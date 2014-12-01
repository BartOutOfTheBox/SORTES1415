/*
 * 	main.c
 *
 *  C file containing all behaviour regarding editing and displaying 
 *	alarm and clock time, IO handling, interrupt handling, and initializations.
 *
 *  Created on: 20-nov.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */


#define __18F97J60
#define __SDCC__

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <malloc.h>

#include "Include/HardwareProfile.h"
#include "Include/LCDBlocking.h"

#include "main.h"
#include "time.h"
#include "clock.h"
#include "alarm.h"

// This is where the heap of the programm is located.
// The size of 256 chars is more than sufficient.
// Only two time structs have memory allocated in the heap.
unsigned char _MALLOC_SPEC heap[256];

static volatile long int interrupts;
// If the state has changed since the last update.
static volatile bool stateChange;
// If the buttons have been pressed since the last checkButtons() call.
static volatile bool button1Pressed;
static volatile bool button2Pressed;

/**
 * Possible edit states of the alarmclock.
 * EditHours: currently editing the number of hours of the alarm OR clock.
 * EditMin: currently editing the number of minutes of the alarm OR clock.
 * EditSec: currently editing the number of seconds of the alarm OR clock.
 * NoEdit: currently not editing the alarm OR clock time.
 */
typedef enum { EditHours, EditMin, EditSec, NoEdit } editState_t;
static editState_t currentEditState;

/**
 * Possible states of the alarmclock.
 * SetTime: settting the time of the alarmclock.
 * SetAlarm: setting the alarm time of the alarmclock.
 * Display: displaying both alarm and clock time of the alarmclock.
 */
typedef enum { SetTime, SetAlarm, Display } state_t;
static state_t currentState;

static int lastClockSecondsPrint;
static int lastAlarmSecondsPrint;

void init(void);
void initTimer(void);
void initLeds(void);
void initButtons(void);
void startDisplay(void);
void startTimeEdit(void);
void startAlarmEdit(void);
void checkButtons(void);
void updateTimeDisplay(bool forceUpdate);
void updateBoard(void);
void updateLeds(void);
void displayString(BYTE pos, char* text);

// Inittialize and start running the alarmclock.
void main(void)
{
    init();
    while(1) {
        checkButtons();
        updateAlarm();
        updateBoard();
    }
}

void init(void)
{
	// Initialize the heap in the declared heap character array.
    _initHeap(heap, 256);

	// A number that will never be printed.
	// This enforces the next number of seconds to always be printed.
    lastClockSecondsPrint = -1;
    lastAlarmSecondsPrint = -1;
    interrupts            = 0;

    INTCONbits.GIE     = 1;   //enable high priority interrupts
    RCONbits.IPEN      = 1;   //enable interrupts priority levels

	// Initialize the used io hardware.
    initButtons();
    initLeds();
    LCDInit();

	// Initialize the AlarmClock
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

    INTCONbits.T0IF  = 0;
    T0CONbits.TMR0ON = 1;
}



/*******************************************************************************
 * State machine
 ******************************************************************************/

// Disable both the alarm and the clock. Start editing the hours on the clock.
void startTimeEdit(void)
{
    currentState = SetTime;
    currentEditState = EditHours;
    disableClock();
    disableAlarm();
    stateChange = True;
}

// Disable the alarm. Start editing the hours of the alarm time.
void startAlarmEdit(void)
{
    currentState = SetAlarm;
    currentEditState = EditHours;
    disableAlarm();
    stateChange = True;
}

// Enable both the alarm and the clock. Not editing anything.
void startDisplay(void)
{
    currentState = Display;
    currentEditState = NoEdit;
    enableClock();
    enableAlarm();
    stateChange = True;
}

void checkButtons()
{
    if (button2Pressed) {
        button2Pressed = False;	// Unset the button pressed flag.
		// The user knows the alarm has sounded. No need to further annoy him ;)
        if (isAlarmSounding()) {
            enableAlarm();
        }
        else if (currentState == SetTime) {
            if (currentEditState == EditHours) {
				// Add an hour to the clock.
                addSecondsToClock(3600);
            } else if (currentEditState == EditMin) {
				// Add a minute to the clock.
                addSecondsToClock(60);
            } else if (currentEditState == EditSec) {
				// Add a second to the clock.
                addSecondsToClock(1);
            }
        } else if (currentState == SetAlarm) {
            if (currentEditState == EditHours) {
				// Add an hour to the alarmtime.
                addSecondsToAlarm(3600);
            } else if (currentEditState == EditMin) {
				// Add a minute to the alarmtime.
                addSecondsToAlarm(60);
            } else if (currentEditState == EditSec) {
				// Add a second to the alarmtime.
                addSecondsToAlarm(1);
            }
        } else {
			// Start editing the time, if no edit mode was enabled.
            startTimeEdit();
        }
    }
    else if (button1Pressed) {
        button1Pressed = False;	// Unset the button pressed flag.
		// The user knows the alarm has sounded. No need to further annoy him ;)
        if (isAlarmSounding()) {
            enableAlarm();
        }
        else if (currentState == SetTime || currentState == SetAlarm) {
            if (currentEditState == EditHours) {
				// Start editing the number of minutes.
                currentEditState = EditMin;
            } else if (currentEditState == EditMin) {
				// Start editing the number of seconds.
                currentEditState = EditSec;
            } else if (currentEditState == EditSec) {
                if (currentState == SetTime) {
					// Start the clock, and start editing the alarmtime, 
					// if no alarmtime was previously set.
                    if (!isAlarmSet()) {
                        enableClock();
                        startAlarmEdit();
                    }
                    else {
						// Go to display mode.
                        startDisplay();
                    }
                }
                else {
					// Go to display mode.
                    startDisplay();
                }
            }
        }
        else {
			// Start editing the alarmtime, if no edit mode was enabled.
            startAlarmEdit();
        }
    }
}

/*******************************************************************************
 * Display & Led
 * Updates the display and the output leds, when needed.
 ******************************************************************************/

void updateBoard(void)
{
    bool updateDisplay = False;
    if (stateChange) {
        stateChange = False;
        updateDisplay = True;
        memset(LCDText, ' ', 32);

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
    updateTimeDisplay(updateDisplay);
    updateLeds();
}

/*******************************************************************************
 * Function displayString:
 * Writes the first characters of the string in the remaining
 * space of the 32 positions LCD, starting at pos
 * (does not use strlcopy, so can use up to the 32th place)
*******************************************************************************/

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
}

// Indicate which editState we're currently in, 
// by blinking the relevant time unit on the screen.
// eg. HH:MM:SS -> alternating between HH:  :SS and HH:MM:SS, 
// when editing the minutes.
void blinkTimeEdit(char* timeString)
{
    if (currentEditState == EditHours 
		&& ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[0] = ' ';
        timeString[1] = ' ';
    }
    else if (currentEditState == EditMin 
			&& ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[3] = ' ';
        timeString[4] = ' ';
    }
    else if (currentEditState == EditSec 
			&& ((interrupts / (ticksPerSecond / 2)) % 2) != 0) {
        timeString[6] = ' ';
        timeString[7] = ' ';
    }
}

// Update the displayed time, and write it to the display.
// If forceUpdate is set, the display will be updated, 
// even if no changes are detected.
void updateTimeDisplay(bool forceUpdate)
{
    bool update = False;
    time_t *clockTimeStruct = updateAndGetClockTime();
    time_t *alarmTimeStruct = updateAndGetAlarmTime();
    char clockTimeString[9];
    char alarmTimeString[9];

    sprintf(clockTimeString, "%02d:%02d:%02d", 
			clockTimeStruct->hours, 
			clockTimeStruct->minutes, 
			clockTimeStruct->seconds);
    sprintf(alarmTimeString, "%02d:%02d:%02d", 
			alarmTimeStruct->hours,
			alarmTimeStruct->minutes,
			alarmTimeStruct->seconds);

    if (currentState == SetTime) {
        blinkTimeEdit(clockTimeString);
        displayString(7, clockTimeString);
		// Always update the display, when editing.
        update = True;
    }
    else if (currentState == SetAlarm) {
        blinkTimeEdit(alarmTimeString);
        displayString(23, alarmTimeString);
        // Always update the display, when editing.
		update = True;
    }
    else {
        if (lastClockSecondsPrint != clockTimeStruct->secondsOfTheDay) {
            // Change detected. Display update is needed.
			update = True;
        }
        if (lastAlarmSecondsPrint != alarmTimeStruct->secondsOfTheDay) {
            // Change detected. Display update is needed.
			update = True;
        }
        displayString(7, clockTimeString);
        displayString(23, alarmTimeString);
        lastClockSecondsPrint = clockTimeStruct->secondsOfTheDay;
        lastAlarmSecondsPrint = alarmTimeStruct->secondsOfTheDay;
    }
	// Update the display if forced, or a change is detected.
    if (update || forceUpdate) {
        LCDUpdate();
    }
}

// Update the output leds.
void updateLeds(void)
{
	// Check if the clock led should be on.
    if (showClockLed()) {
        LATJbits.LATJ0=1; // switch LED 1 on
    }
    else {
        LATJbits.LATJ0=0; // switch LED 1 off
    }
	// Check if the alarm led should be on.
    if (showAlarmLed()) {
        LATJbits.LATJ1=1; // switch LED 2 on
    }
    else {
        LATJbits.LATJ1=0; // switch LED 2 off
    }
}

void LowISR(void) __interrupt 2
{
    // Low priority interrupts are currently not used.
}

void HighISR(void) __interrupt 1
{
    if (INTCONbits.T0IF) { // Timer0 Interrupt
        INTCONbits.T0IF  = 0; // Reset the Timer0 Interrupt flag.
        T0CONbits.TMR0ON = 1; // Turn Timer0 on again.
        if (LONG_MAX - 1 < interrupts) {
			//prevent an overflow.
            interrupts = 0;
        }
        interrupts++;
		//Let the clock tick.
        tickClock();
    }
    if(INTCON3bits.INT1F) //Button2 Interrupt
    {
        INTCON3bits.INT1F  = 0;   //clear INT1 flag
        if(BUTTON0_IO);  //just read the bit
        button2Pressed = True;
    }
    else if(INTCON3bits.INT3IF) //Button1 Interrupt
    {
        INTCON3bits.INT3IF  = 0;   //clear INT3 flag
        if(BUTTON1_IO);  //just read the bit
        button1Pressed = True;
    }
}
