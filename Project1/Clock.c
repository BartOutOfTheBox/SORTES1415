/*
 * Clock.c
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

long int clockTicks;
long int clockSeconds;
clockState_t currentClockState;
time_t * clockTimeStruct;

void initClock(void)
{
    clockTimeStruct = (time_t *) malloc(sizeof (time_t));
    clockTimeStruct->hours = 0;
    clockTimeStruct->minutes = 0;
    clockTimeStruct->seconds = 0;
    clockTimeStruct->secondsOfTheDay = 0;
    currentClockState = ClockDisabled;
    clockTicks = 0;
    clockSeconds = 0;
}

void enableClock(void)
{
    currentClockState = ClockRunning;
}

void disableClock(void)
{
    currentClockState = ClockDisabled;
}

long int getClockSeconds(void)
{
    return clockSeconds;
}

void tickClock(void)
{
    if (currentClockState == ClockRunning) {
        clockTicks++;
        if (clockTicks >= ticksPerSecond) {
            clockSeconds++;
            if (clockSeconds >= 86400) {
                clockSeconds = clockSeconds % 86400;
            }
            clockTicks = clockTicks % ticksPerSecond;
        }
    }
}

void addSecondsToClock(long int newSeconds)
{
    clockSeconds = (clockSeconds + newSeconds) % 86400;
}


time_t *updateAndGetClockTime(void)
{
    updateTimeStruct(clockSeconds, clockTimeStruct);
    return clockTimeStruct;
}

bool showClockLed(void)
{
    return (currentClockState == ClockRunning && (clockTicks < ticksPerSecond / 2));
}

