/*
 * 	alarm.c
 *
 *	C file containing all behaviour related to the alarm part of the clock.
 *	This includes enabling and disabling the alarm, setting the alarm,
 *	checking if the alarm should sound, sounding the alarm,
 *	and setting the alarm time.
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 *		
 */
#include <malloc.h>
#include "alarm.h"
#include "time.h"
#include "main.h"
#include "clock.h"

/**
 * Possible states of the alarm.
 * AlarmUnset: A time value representing the time this alarm should sound 
 *				- when enabled - has not been set yet.
 *				This is the initial state of the alarm, and will not
 *				be returned to after the alarmTime has been set once.
 * AlarmDisabled: The alarm is not currently active, and hence will never sound.
 * AlarmSounding: The alarm is currently triggered, and sounding.
 * AlarmEnabled: The alarm is currently active, 
 *				and waiting for the right time to sound.
 */
typedef enum { 	AlarmDisabled,
				AlarmSounding,
				AlarmEnabled,
				AlarmUnset } alarmState_t;
static alarmState_t currentState;

// When the alarm should stop sounding, stored in ticks.
static long int alarmEnd;
// When the alarm should sound, stored in ticks.
static long int alarmTime;

// A time struct representing the time the alarm should sound.
static time_t *alarmTimeStruct;

void initAlarm(void)
{
	// Allocate memmory for the time struct representing 
	// the time the alarm should sound.
    alarmTimeStruct                  = (time_t *) malloc(sizeof (time_t));
	// Temporarily initialized with zeroes.
    alarmTimeStruct->hours           = 0;
    alarmTimeStruct->minutes         = 0;
    alarmTimeStruct->seconds         = 0;
    alarmTimeStruct->secondsOfTheDay = 0;
    alarmTime                        = 0;
	// The user should still configure a personal alarm time, 
	// hence the current state is Unset.
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

// Check if the state of the alarm should change.
void updateAlarm(void)
{
    if (currentState == AlarmEnabled) {
		// Alarm is triggered when the alarmTime's 
		// hours, minutes, AND seconds match
		// the number of hours, minutes, and seconds on the clock.
        if (alarmTime == getClockSeconds()) {
            currentState = AlarmSounding;
			// The alarm should sound for 30 seconds.
            alarmEnd = (getClockSeconds() + 30) % SECONDS_PER_DAY;
        }
    }
	// Let the alarm stop sounding after 30 seconds.
    else if (currentState == AlarmSounding && getClockSeconds() == alarmEnd) {
        currentState = AlarmEnabled;
    }
}

// Defer the alarmTime by the given number of seconds.
void addSecondsToAlarm(long int newSeconds)
{
    alarmTime = (alarmTime + newSeconds) % SECONDS_PER_DAY;
}

// Update and return a pointer to the struct representing the alarmTime.
time_t *updateAndGetAlarmTime(void)
{
    updateTimeStruct(alarmTime, alarmTimeStruct);
    return alarmTimeStruct;
}

// Whether the blinking led representing the alarm sound, should burn.
bool showAlarmLed(void)
{
	// When the alarm should sound, alternating between on and off every second.
    return (currentState == AlarmSounding 
				&& (getClockSeconds() % 2) == (alarmTime % 2));
}

