/*
 * Clock.h
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne
 */

#ifndef CLOCK_H_
#define CLOCK_H_
#include "main.h"
#include "time.h"

void initClock(void);
void tickClock(void);
void ticksToTime(long int secondsToday, time_t * timeStruct);
time_t *updateAndGetClockTime(void);
void addSecondsClock(long int newSeconds);
void enableClock(void);
void disableClock(void);
long int getClockTicks(void);
bool showClockLed(void);

extern long int clockSeconds;

#endif /* CLOCK_H_ */
