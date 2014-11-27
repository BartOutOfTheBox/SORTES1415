/*
 * 	time.h
 *
 *	Time in ticks to time in seconds, minutes, and hours conversion functions.
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#ifndef TIME_H_
#define TIME_H_

#define SECONDS_PER_DAY 86400

extern long int ticksPerSecond;

// Struct representing a time.
// Useful for caching time conversions in human readable format.
typedef struct {
    int hours;
    int minutes;
    int seconds;
    long int secondsOfTheDay;
}time_t;

// Convert and store time in seconds to time in hours, minutes, and seconds.
void updateTimeStruct(long int secondsToday, time_t *timeStruct);

#endif /* TIME_H_ */
