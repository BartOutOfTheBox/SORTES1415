/*
 * Debug.h
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "GenericTypeDefs.h"

#if !defined(THIS_IS_STACK_APPLICATION)
    extern BYTE AN0String[8];
#endif

// An actual function defined in Main.c for displaying the current IP
// address on the LCD.
#if defined(__SDCC__)
    void DisplayIPValue(DWORD IPVal);
    void DisplayString(BYTE pos, char* text);
    void DisplayWORD(BYTE pos, WORD w);
#else
    void DisplayIPValue(IP_ADDR IPVal);
#endif

#endif /* DEBUG_H_ */
