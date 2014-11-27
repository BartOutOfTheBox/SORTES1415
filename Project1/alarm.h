/*
 * alarm.h
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
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
void updateAlarm(void);
void addSecondsToAlarm(long int newSeconds);
time_t *updateAndGetAlarmTime(void);
bool showAlarmLed(void);

#endif /* ALARM_H_ */
