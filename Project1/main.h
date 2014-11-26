/*
 * main.h
 *
 *  Created on: 25-nov.-2014
 *      Author: Arne
 */

#ifndef MAIN_H_
#define MAIN_H_

extern long int ticksPerSecond;

long long getInterrupts(void);

typedef struct {
    int hours;
    int minutes;
    int seconds;
    int secondsOfTheDay;
}time;

typedef enum { false, true } bool;
#endif /* MAIN_H_ */
