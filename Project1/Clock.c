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

unsigned long int clockTicks;
unsigned long int getNbTicksClock(void);

void initClock(void) {
    currentClockState = clockDisabled;
    clockTicks = 0;
}

void tickClock(void) {
    if (currentClockState == clockRunning) {
        if (ULONG_MAX - 1 >= clockTicks) {
            clockTicks++;
            clockTicks = (clockTicks % (clockTicksPerSeconds * 86400));
        }
    }
}

unsigned long int getNbTicksClock(void) {
    return clockTicks;
}
void addTicksClock(unsigned long int newTicks) {
    if (ULONG_MAX - newTicks >= clockTicks) {
        clockTicks = clockTicks + newTicks;
        clockTicks = (clockTicks % (clockTicksPerSeconds * 86400));
    }
}
void addHour(void) {
    addTicksClock(clockTicksPerSeconds * 3600);
}
void addMin(void) {
    addTicksClock(clockTicksPerSeconds * 60);
}
void addSec(void) {
    addTicksClock(clockTicksPerSeconds);
}
void enableClock(void) {
    currentClockState = clockRunning;
}
void disableClock(void) {
    currentClockState = clockDisabled;
}

char* getClockTime(void) {
    return ticksToTime(clockTicks);
}

char* ticksToTime(unsigned long int clockTicks) {
    unsigned long int secondsToday = clockTicks / clockTicksPerSeconds;
    int hours;
    int minutes;
    int seconds;
    char timeString[16];

    hours = secondsToday / 3600;
    minutes = (secondsToday % 3600) / 60;
    seconds = (secondsToday % 3600) % 60;

    sprintf(timeString, "%02d:%02d:%02d", hours, minutes, seconds);

    return timeString;
}
