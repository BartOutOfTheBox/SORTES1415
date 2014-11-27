/*
 * Alarm.h
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
 */

#ifndef ALARM_H_
#define ALARM_H_
#include "main.h"
#include "time.h"

typedef enum { AlarmDisabled, AlarmSounding, AlarmEnabled, AlarmUnset } alarmState;

void initAlarm(void);
time_t * updateAndGetAlarmTime(void);
void disableAlarm(void);
void enableAlarm(void);
alarmState getAlarmState(void);
void checkAlarm(void);
void addSecondsToAlarm(long int newSeconds);
bool showAlarmLed(void);
bool isAlarmSounding(void);
bool isAlarmSet(void);

#endif /* ALARM_H_ */
