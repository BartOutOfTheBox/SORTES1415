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

typedef enum { clockDisabled, clockRunning } clockState;

clockState currentClockState;
long int clockTicks;

void initClock(void) {
    currentClockState = clockDisabled;
    clockTicks = 80000L;
}

void tickClock(void) {
    if (currentClockState == clockRunning) {
        if (LONG_MAX - 1 >= clockTicks) {
            clockTicks++;
            clockTicks = (clockTicks % (getClockTicksPerSecond() * 86400));
        }
    }
}

void addTicksClock(long int newTicks) {
    if (ULONG_MAX - newTicks >= clockTicks) {
        clockTicks = clockTicks + newTicks;
        clockTicks = (clockTicks % (getClockTicksPerSecond() * 86400));
    }
}
void enableClock(void) {
    currentClockState = clockRunning;
}
void disableClock(void) {
    currentClockState = clockDisabled;
}

char* getClockTime(void) {
	char clockTimeString[16];
    return ticksToTime(clockTicks, clockTimeString);
}

long int getClockTicks(void) {
    return clockTicks;
}

char* ticksToTime(long int ticks, char* timeString) {
    long int secondsToday = ticks / getClockTicksPerSecond();
    int hours;
    int minutes;
    int seconds;

    hours = secondsToday / 3600;
    minutes = (secondsToday % 3600) / 60;
    seconds = (secondsToday % 3600) % 60;

    sprintf(timeString, "%02d:%02d:%02d", hours, minutes, seconds);

    return timeString;
}

