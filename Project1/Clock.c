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

typedef enum { clockDisabled, clockRunning } clockState;

clockState currentClockState;
long int clockTicks;
time * clockTimeStruct;
long int clockSeconds;

void initClock(void) {
    clockTimeStruct = (time *) malloc(sizeof (time));
    clockTimeStruct->hours = 0;
    clockTimeStruct->minutes = 0;
    clockTimeStruct->seconds = 0;
    clockTimeStruct->secondsOfTheDay = 0;
    currentClockState = clockDisabled;
    clockTicks = 0;
    clockSeconds = 0;
}

void tickClock(void) {
    if (currentClockState == clockRunning) {
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

void addSecondsClock(long int newSeconds) {
    clockSeconds = (clockSeconds + newSeconds) % 86400;
}

void enableClock(void) {
    currentClockState = clockRunning;
}
void disableClock(void) {
    currentClockState = clockDisabled;
}

time * updateAndGetClockTime(void) {
    ticksToTime(clockSeconds, clockTimeStruct);
    return clockTimeStruct;
}

void ticksToTime(long int secondsToday, time * timeStruct) {
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
    return (currentClockState == clockRunning && ((clockTicks / (ticksPerSecond / 2)) % 2) != 0);
}

