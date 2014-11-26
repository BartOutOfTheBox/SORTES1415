/*
 * clock.h
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "main.h"
#include "time.h"

void initClock(void);
void enableClock(void);
void disableClock(void);
long int getClockSeconds(void);
void tickClock(void);
void addSecondsToClock(long int newSeconds);
time_t *updateAndGetClockTime(void);
bool shouldTurnOnClockLed(void);

#endif /* CLOCK_H_ */
