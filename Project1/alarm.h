/*
 * 	alarm.h
 *
 *	h file containing all definitions related to the alarm part of the clock.
 *	This includes enabling and disabling the alarm, setting the alarm,
 *	sounding the alarm, and setting the alarm time.
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 *		
 */

#ifndef ALARM_H_
#define ALARM_H_
#include "main.h"
#include "time.h"

void initAlarm(void);
bool isAlarmSounding(void);
bool isAlarmSet(void);
void enableAlarm(void);
void disableAlarm(void);
// Check if the state of the alarm should change.
void updateAlarm(void);
// Defer the alarmTime by the given number of seconds.
void addSecondsToAlarm(long int newSeconds);
// Update and return a pointer to the struct representing the alarmTime.
time_t *updateAndGetAlarmTime(void);
// Whether the blinking led representing the alarm sound, should burn.
bool showAlarmLed(void);

#endif /* ALARM_H_ */
