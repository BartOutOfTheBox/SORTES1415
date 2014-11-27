/*
 * Alarm.c
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
 */
#include <malloc.h>
#include "Alarm.h"
#include "time.h"
#include "main.h"
#include "Clock.h"

typedef enum { AlarmDisabled, AlarmSounding, AlarmEnabled, AlarmUnset } alarmState;
alarmState currentAlarmState;

long int alarmEnd;
long int alarmTime;

time_t *alarmTimeStruct;

void initAlarm(void)
{
    alarmTimeStruct = (time_t *) malloc(sizeof (time_t));
    alarmTimeStruct->hours = 0;
    alarmTimeStruct->minutes = 0;
    alarmTimeStruct->seconds = 0;
    alarmTimeStruct->secondsOfTheDay = 0;
    alarmTime = 0;
    currentAlarmState = AlarmUnset;
}

bool isAlarmSounding(void)
{
    return (currentAlarmState == AlarmSounding);
}

bool isAlarmSet(void)
{
    return (currentAlarmState != AlarmUnset);
}

void enableAlarm(void)
{
    currentAlarmState = AlarmEnabled;
}

void disableAlarm(void)
{
    currentAlarmState = AlarmDisabled;
}

void updateAlarm(void)
{
    if (currentAlarmState == AlarmEnabled) {
        if (alarmTime == clockSeconds) {
            currentAlarmState = AlarmSounding;
            alarmEnd = (clockSeconds + 30) % 86400;
        }
    }
    else if (currentAlarmState == AlarmSounding && clockSeconds == alarmEnd) {
        currentAlarmState = AlarmEnabled;
    }
}

void addSecondsToAlarm(long int newSeconds)
{
    alarmTime = (alarmTime + newSeconds) % 86400;
}

time_t *updateAndGetAlarmTime(void)
{
    updateTimeStruct(alarmTime, alarmTimeStruct);
    return alarmTimeStruct;
}

bool showAlarmLed(void)
{
    return (currentAlarmState == AlarmSounding && (clockSeconds % 2) == (alarmTime % 2));
}

