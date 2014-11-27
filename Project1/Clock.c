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

typedef enum { ClockDisabled, ClockRunning } clockState;

clockState currentClockState;
long int clockTicks;
time_t * clockTimeStruct;
long int clockSeconds;

void initClock(void) {
    clockTimeStruct = (time_t *) malloc(sizeof (time_t));
    clockTimeStruct->hours = 0;
    clockTimeStruct->minutes = 0;
    clockTimeStruct->seconds = 0;
    clockTimeStruct->secondsOfTheDay = 0;
    currentClockState = ClockDisabled;
    clockTicks = 0;
    clockSeconds = 0;
}

void tickClock(void) {
    if (currentClockState == ClockRunning) {
        clockTicks++;
        if (clockTicks == ticksPerSecond) {
            clockSeconds++;
            if (clockSeconds >= 86400) {
                clockSeconds = clockSeconds % 86400;
            }
            clockTicks = 0;
        }
    }
}

void addSecondsToClock(long int newSeconds) {
    clockSeconds = (clockSeconds + newSeconds) % 86400;
}

void enableClock(void) {
    currentClockState = ClockRunning;
}
void disableClock(void) {
    currentClockState = ClockDisabled;
}

time_t * updateAndGetClockTime(void) {
    updateTimeStruct(clockSeconds, clockTimeStruct);
    return clockTimeStruct;
}

void ticksToTime(long int secondsToday, time_t * timeStruct) {
    long int ticksWithoutHours;

    if (secondsToday == timeStruct->secondsOfTheDay) {
        return;
    }

    timeStruct->hours = secondsToday / 3600;

    ticksWithoutHours = secondsToday % 3600;
    timeStruct->minutes = ticksWithoutHours / 60;
    timeStruct->seconds = ticksWithoutHours % 60;
    timeStruct->secondsOfTheDay = secondsToday;
}

bool showClockLed(void) {
    return (currentClockState == ClockRunning && ((clockTicks / (ticksPerSecond / 2)) % 2) != 0);
}

