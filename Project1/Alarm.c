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
#include <malloc.h>

alarmState currentAlarmState;

int alarmStart;

time * alarmTimeStruct;
long int alarmTime;

void initAlarm(void) {
    alarmTimeStruct = (time *) malloc(sizeof (time));
    alarmTimeStruct->hours = 0;
    alarmTimeStruct->minutes = 0;
    alarmTimeStruct->seconds = 0;
    alarmTimeStruct->secondsOfTheDay = 0;
    alarmTime = 0;
    currentAlarmState = alarmUnset;
}

void enableAlarm(void) {
    currentAlarmState = alarmEnabled;
}

void disableAlarm(void) {
    currentAlarmState = alarmDisabled;
}

time *updateAndGetAlarmTime(void) {
    ticksToTime(alarmTime, alarmTimeStruct);
    return alarmTimeStruct;
}

void addSecondsAlarm(long int newSeconds) {
    alarmTime = alarmTime + newSeconds;
    alarmTime = (alarmTime % 86400);
}

void checkAlarm(void) {
    if (currentAlarmState == alarmEnabled) {
        if (alarmTime == clockSeconds) {
            currentAlarmState = alarmSounding;
            alarmStart = getInterrupts();
        }
    }
    else if (currentAlarmState == alarmSounding) {
        long int thirtySecondsAgo = getInterrupts() - 30 * ticksPerSecond;
        if (thirtySecondsAgo < 0) {
            thirtySecondsAgo = LONG_MAX - thirtySecondsAgo;
        }
        if (thirtySecondsAgo >= alarmStart) {
            enableAlarm();
        }
    }
}

alarmState getAlarmState(void) {
    return currentAlarmState;
}

bool showAlarmLed(void) {
    return (currentAlarmState == alarmSounding && (clockSeconds % 2) != 0);
}
