/*
 * time.c
 *
 *  Created on: 26-nov.-2014
 *      Author: Arne
 */

#include "time.h"

long int ticksPerSecond = 24318;

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
