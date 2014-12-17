/*********************************************************************
 *
 *                  Headers for DHCPRelay Main App
 *
 *********************************************************************
 * FileName:        Main.h
 * Dependencies:    Compiler.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * E. Wood				4/26/08 Copied from MainDemo.c
 * A. Van der Stappen  12/15/14 Copied from MainDemo.h
 ********************************************************************/
#ifndef _MAIN_H
#define _MAIN_H

// define the processor we use
#define __18F97J60

// define the compiler we use
#define __SDCC__

#include "Include/HardwareProfile.h"
#include "Include/GenericTypeDefs.h"
#include "DHCPBuffer.h"
#include "Include/TCPIP_Stack/TCPIP.h"
#include <malloc.h>

#define BAUD_RATE       (19200)		// bps

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

// Include all headers for any enabled TCPIP Stack functions

void initializeDHCPRelay(void);

void printError(char* error);

extern UDP_SOCKET DHCPServerSocket;
extern UDP_SOCKET DHCPClientSocket;
extern NODE_INFO DHCP_Server;

extern dhcpBuffer_t* DHCPClientBuffer;
extern dhcpBuffer_t* DHCPServerBuffer;


#endif // _MAIN_H
size_t  strlcpy(char *dst, const char *src, size_t siz);
