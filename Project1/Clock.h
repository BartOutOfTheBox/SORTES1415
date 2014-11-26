/*
 * Clock.h
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne
 */

#ifndef CLOCK_H_
#define CLOCK_H_

void initClock(void);
void tickClock(void);
char* ticksToTime(long int ticks, char* timeString);
char* getClockTime(void);
void addTicksClock(long int newTicks);
void enableClock(void);
void disableClock(void);
long int getClockTicks(void);

#endif /* CLOCK_H_ */
