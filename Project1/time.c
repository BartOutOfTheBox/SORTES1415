/*
 * 	time.c
 *
 *	C file containing time in ticks to time in 
 *	seconds, minutes, and hours conversion code.
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#include "time.h"

long int ticksPerSecond = 24318;

// Convert and store time in ticks to time in hours, minutes, and seconds.
void updateTimeStruct(long int secondsToday, time_t *timeStruct) {
    long int secondsWithoutHours;

    if (secondsToday == timeStruct->secondsOfTheDay) {
        return;
    }

    timeStruct->hours = secondsToday / 3600;

    secondsWithoutHours = secondsToday % 3600;
    timeStruct->minutes = secondsWithoutHours / 60;
    timeStruct->seconds = secondsWithoutHours % 60;
    timeStruct->secondsOfTheDay = secondsToday;
}
