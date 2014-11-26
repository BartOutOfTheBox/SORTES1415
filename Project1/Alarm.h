/*
 * Alarm.h
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
 */

#ifndef ALARM_H_
#define ALARM_H_

typedef enum { alarmUnset, alarmDisabled, alarmSounding, alarmEnabled } alarmState;

void initAlarm(void);
char* getAlarmTime(void);
void disableAlarm(void);
void enableAlarm(void);
alarmState getAlarmState(void);
void checkAlarm(void);
void addTicksAlarm(long int newTicks);

#endif /* ALARM_H_ */
