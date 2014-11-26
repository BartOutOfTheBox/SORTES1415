/*
 * time.h
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
 */

#ifndef TIME_H_
#define TIME_H_

#define SECONDS_PER_DAY 86400

extern long int ticksPerSecond;

typedef struct {
    int hours;
    int minutes;
    int seconds;
    long int secondsOfTheDay;
}time_t;

void updateTimeStruct(long int secondsToday, time_t *timeStruct);

#endif /* TIME_H_ */
