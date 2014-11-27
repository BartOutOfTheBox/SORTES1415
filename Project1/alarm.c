/*
 * alarm.c
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
 */
#include <malloc.h>
#include "alarm.h"
#include "time.h"
#include "main.h"
#include "clock.h"

typedef enum { AlarmDisabled, AlarmSounding, AlarmEnabled, AlarmInitial } alarmState_t;
alarmState_t currentAlarmState;

long int alarmTime;
long int alarmEnd;

time_t * alarmTimeStruct;

void initAlarm(void) {
    alarmTimeStruct = (time_t *) malloc(sizeof (time_t));
    alarmTimeStruct->hours = 0;
    alarmTimeStruct->minutes = 0;
    alarmTimeStruct->seconds = 0;
    alarmTimeStruct->secondsOfTheDay = 0;
    alarmTime = 0;
    currentAlarmState = AlarmInitial;
}

bool isAlarmSounding(void) {
    return (currentAlarmState == AlarmSounding);
}

bool isAlarmSet(void) {
    return (currentAlarmState != AlarmInitial);
}

void enableAlarm(void) {
    currentAlarmState = AlarmEnabled;
}

void disableAlarm(void) {
    currentAlarmState = AlarmDisabled;
}

void updateAlarm(void) {
    if (currentAlarmState == AlarmEnabled) {
        if (alarmTime == getClockSeconds()) {
            currentAlarmState = AlarmSounding;
            alarmEnd = (getClockSeconds() + 30) % 86400;
        }
    }
    else if (currentAlarmState == AlarmSounding && getClockSeconds() == alarmEnd) {
        currentAlarmState = AlarmEnabled;
    }
}

void addSecondsToAlarm(long int newSeconds) {
    alarmTime = (alarmTime + newSeconds) % 86400;
}

time_t *updateAndGetAlarmTime(void) {
    updateTimeStruct(alarmTime, alarmTimeStruct);
    return alarmTimeStruct;
}


bool shouldTurnOnAlarmLed(void) {
    return (currentAlarmState == AlarmSounding && (getClockSeconds() % 2) == (alarmTime % 2));
}
