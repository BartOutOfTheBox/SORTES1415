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
#include "time.h"

alarmState currentAlarmState;

long int alarmStart;
long int alarmEnd;

time_t * alarmTimeStruct;
long int alarmTime;

void initAlarm(void) {
    alarmTimeStruct = (time_t *) malloc(sizeof (time_t));
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

time_t *updateAndGetAlarmTime(void) {
    updateTimeStruct(alarmTime, alarmTimeStruct);
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
            alarmStart = alarmTime;
            alarmEnd = (clockSeconds + 30) % 86400;
        }
    }
    else if (currentAlarmState == alarmSounding && clockSeconds == alarmEnd) {
        currentAlarmState = alarmEnabled;
    }
}

alarmState getAlarmState(void) {
    return currentAlarmState;
}

bool showAlarmLed(void) {
    return (currentAlarmState == alarmSounding && (clockSeconds % 2) != 0);
}
