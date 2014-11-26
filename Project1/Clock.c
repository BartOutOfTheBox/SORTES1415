/*
 * clock.c
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne
 */
#include <malloc.h>
#include "clock.h"
#include "time.h"
#include "main.h"


typedef enum { ClockDisabled, ClockRunning } clockState_t;

long int ticksSinceLastSecond;
long int clockSeconds;
clockState_t currentClockState;
time_t *clockTimeStruct;

void initClock(void) {
    clockTimeStruct                  = (time_t *) malloc(sizeof (time_t));
    clockTimeStruct->hours           = 0;
    clockTimeStruct->minutes         = 0;
    clockTimeStruct->seconds         = 0;
    clockTimeStruct->secondsOfTheDay = 0;
    currentClockState                = ClockDisabled;
    ticksSinceLastSecond             = 0;
    clockSeconds                     = 0;
}

void enableClock(void) {
    currentClockState = ClockRunning;
}
void disableClock(void) {
    currentClockState = ClockDisabled;
}

long int getClockSeconds(void) {
    return clockSeconds;
}

void tickClock(void) {
    if (currentClockState == ClockRunning) {
        ticksSinceLastSecond++;
        if (ticksSinceLastSecond == ticksPerSecond) {
            clockSeconds++;
            if (clockSeconds >= 86400) {
                clockSeconds = clockSeconds % 86400;
            }
            ticksSinceLastSecond = 0;
        }
    }
}

void addSecondsToClock(long int newSeconds) {
    clockSeconds = (clockSeconds + newSeconds) % 86400;
}

time_t *updateAndGetClockTime(void) {
    updateTimeStruct(clockSeconds, clockTimeStruct);
    return clockTimeStruct;
}

bool shouldTurnOnClockLed(void) {
    return (currentClockState == ClockRunning && (ticksSinceLastSecond < ticksPerSecond / 2));
}

