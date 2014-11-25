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
char* ticksToTime(unsigned long int);
char* getClockTime();
void addHour(void);
void addMin(void);
void addSec(void);
void addTicksClock(unsigned long int);
void enableClock(void);
void disableClock(void);

#endif /* CLOCK_H_ */
