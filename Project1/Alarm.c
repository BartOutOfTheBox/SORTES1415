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

typedef enum { AlarmDisabled, AlarmSounding, AlarmEnabled, AlarmUnset } alarmState_t;
static alarmState_t currentState;

static long int alarmEnd;
static long int alarmTime;

static time_t *alarmTimeStruct;

void initAlarm(void)
{
    alarmTimeStruct                  = (time_t *) malloc(sizeof (time_t));
    alarmTimeStruct->hours           = 0;
    alarmTimeStruct->minutes         = 0;
    alarmTimeStruct->seconds         = 0;
    alarmTimeStruct->secondsOfTheDay = 0;
    alarmTime                        = 0;
    currentState                     = AlarmUnset;
}

bool isAlarmSounding(void)
{
    return (currentState == AlarmSounding);
}

bool isAlarmSet(void)
{
    return (currentState != AlarmUnset);
}

void enableAlarm(void)
{
    currentState = AlarmEnabled;
}

void disableAlarm(void)
{
    currentState = AlarmDisabled;
}

void updateAlarm(void)
{
    if (currentState == AlarmEnabled) {
        if (alarmTime == getClockSeconds()) {
            currentState = AlarmSounding;
            alarmEnd = (getClockSeconds() + 30) % 86400;
        }
    }
    else if (currentState == AlarmSounding && getClockSeconds() == alarmEnd) {
        currentState = AlarmEnabled;
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
    return (currentState == AlarmSounding && (getClockSeconds() % 2) == (alarmTime % 2));
}

