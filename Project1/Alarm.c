/*
 * Alarm.c
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
 */
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Alarm.h"
#include "main.h"
#include "Clock.h"

alarmState currentAlarmState;

long int alarmTime;

void initAlarm(void) {
    currentAlarmState = alarmUnset;
    alarmTime = 0;
}

char* getAlarmTime(void) {
    char alarmTimeString[16];
    return ticksToTime(alarmTime, alarmTimeString);
}

void enableAlarm(void) {
    currentAlarmState = alarmEnabled;
}

void disableAlarm(void) {
    currentAlarmState = alarmDisabled;
}

void addTicksAlarm(long int newTicks) {
    if (ULONG_MAX - newTicks >= alarmTime) {
        alarmTime = alarmTime + newTicks;
        alarmTime = (alarmTime % (getClockTicksPerSecond() * 86400));
    }
}

void checkAlarm(void) {
    if (currentAlarmState != alarmEnabled) {
        return;
    }
    if (alarmTime == getClockTicks()) {
        currentAlarmState = alarmSounding;
    }
}

alarmState getAlarmState(void) {
    return currentAlarmState;
}
