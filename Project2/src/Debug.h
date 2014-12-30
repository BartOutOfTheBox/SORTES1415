/*
 * Debug.h
 *
 * Used for debugging the TCPIP_Stack
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "Include/GenericTypeDefs.h"

#if !defined(THIS_IS_STACK_APPLICATION)
    extern BYTE AN0String[8];
#endif

// An actual function defined in Main.c for displaying the current IP
// address on the LCD.
#if defined(__SDCC__)
    void DisplayString(BYTE pos, char* text);
#endif

#endif /* DEBUG_H_ */
