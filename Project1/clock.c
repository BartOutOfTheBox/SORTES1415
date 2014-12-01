/*
 * 	clock.c
 *
 *	C file containing all behaviour related to counting time.
 *	This includes increasing the time when receiving clock ticks,
 *	enabling and disabling the clock, and indicating if the clock is running.
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */
#include <limits.h>
#include "main.h"
#include "clock.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "time.h"

/**
 * Possible states of the clock.
 * ClockDisabled: The clock is currently not running,
 				and time will not be increased when receiving ticks.
 * ClockRunning: The clock is running, and the time will be updated when needed.
 */
typedef enum { ClockDisabled, ClockRunning } clockState_t;

// The number of clockTicks since the last second.
static volatile long int clockTicks;
// The number of seconds since the last day.
static volatile long int clockSeconds;

static clockState_t currentState;
// A time struct representing the time of the clock.
static time_t *clockTimeStruct;
// Skipping a tick every x seconds. 
// This variable indicates the amount of seconds after the reset.
static int skipTick;

void initClock(void)
{
	// Allocate memmory for the time struct representing 
	// the current time of the clock.
    clockTimeStruct                     = (time_t *) malloc(sizeof (time_t));
	// Temporarily initialized with zeroes.
    clockTimeStruct->hours              = 0;
    clockTimeStruct->minutes            = 0;
    clockTimeStruct->seconds            = 0;
    clockTimeStruct->secondsOfTheDay    = 0;
	// The clock is not running. The user still needs to set a proper time.
    currentState                        = ClockDisabled;
    clockTicks                          = 0;
    clockSeconds                        = 0;
	skipTick							= 0;
}

void enableClock(void)
{
    currentState = ClockRunning;
}

void disableClock(void)
{
    currentState = ClockDisabled;
}

long int getClockSeconds(void)
{
    return clockSeconds;
}

void tickClock(void)
{
	//Only increase the clock, when it's enabled.
    if (currentState == ClockRunning) {
		if (skipTick == 4) {
			// R aeset the amount of seconds after a tickSkip.
		    skipTick = 0;
		} else {
			clockTicks++;
		}
		// If a second has passed
        if (clockTicks >= ticksPerSecond) {
			// increase the amount of seconds after a tickSkipt.
			skipTick++;
            clockSeconds++;
			// If a day has passed
            if (clockSeconds >= SECONDS_PER_DAY) {
				// Reset the number of seconds since the last day.
                clockSeconds = clockSeconds % SECONDS_PER_DAY;
            }
			// Reset the number of clockTicks since the last second.
            clockTicks = clockTicks % ticksPerSecond;
        }
    }
}

// Increase the time of the clock by the given number of seconds.
void addSecondsToClock(long int newSeconds)
{
    clockSeconds = (clockSeconds + newSeconds) % SECONDS_PER_DAY;
}

// Update and return a pointer to the struct representing the clockTimeStruct.
time_t *updateAndGetClockTime(void)
{
    updateTimeStruct(clockSeconds, clockTimeStruct);
    return clockTimeStruct;
}

// Whether the blinking led representing state of the clock 
// (running vs. disabled) should burn.
bool showClockLed(void)
{
	// If the clock is running, and blink every half second.
    return (currentState == ClockRunning && (clockTicks < ticksPerSecond / 2));
}

