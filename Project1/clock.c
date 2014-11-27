/*
 * clock.c
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne
 */
#include <limits.h>
#include "main.h"
#include "clock.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "time.h"

typedef enum { ClockDisabled, ClockRunning } clockState_t;

static volatile long int clockTicks;
static volatile long int clockSeconds;
static clockState_t currentState;
static time_t *clockTimeStruct;

void initClock(void)
{
    clockTimeStruct                     = (time_t *) malloc(sizeof (time_t));
    clockTimeStruct->hours              = 0;
    clockTimeStruct->minutes            = 0;
    clockTimeStruct->seconds            = 0;
    clockTimeStruct->secondsOfTheDay    = 0;
    currentState                        = ClockDisabled;
    clockTicks                          = 0;
    clockSeconds                        = 0;
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
    if (currentState == ClockRunning) {
        clockTicks++;
        if (clockTicks >= ticksPerSecond) {
            clockSeconds++;
            if (clockSeconds >= SECONDS_PER_DAY) {
                clockSeconds = clockSeconds % SECONDS_PER_DAY;
            }
            clockTicks = clockTicks % ticksPerSecond;
        }
    }
}

void addSecondsToClock(long int newSeconds)
{
    clockSeconds = (clockSeconds + newSeconds) % SECONDS_PER_DAY;
}


time_t *updateAndGetClockTime(void)
{
    updateTimeStruct(clockSeconds, clockTimeStruct);
    return clockTimeStruct;
}

bool showClockLed(void)
{
    return (currentState == ClockRunning && (clockTicks < ticksPerSecond / 2));
}

