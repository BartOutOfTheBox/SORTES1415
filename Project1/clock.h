/*
 * 	clock.h
 *
 *	H file containing all definitions related to counting time.
 *	This includes receiving clock ticks, enabling and disabling the clock,
 *	getting the current time, and indicating if the clock is running.
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
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
// Increase the time of the clock by the given number of seconds.
void addSecondsToClock(long int newSeconds);
// Update and return a pointer to the struct representing the clockTimeStruct.
time_t *updateAndGetClockTime(void);
// Whether the blinking led representing state of the clock 
// (running vs. disabled) should burn.
bool showClockLed(void);

#endif /* CLOCK_H_ */
