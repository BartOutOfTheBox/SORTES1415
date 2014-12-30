/*********************************************************************
 *
 *                  Headers for DHCPRelay Main App
 *
 *********************************************************************
 * FileName:        main.h
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

// DHCP Server IP
#define DEFAULT_SERVER_IP_ADDR_BYTE1        (192ul)
#define DEFAULT_SERVER_IP_ADDR_BYTE2        (168ul)
#define DEFAULT_SERVER_IP_ADDR_BYTE3        (96ul)
#define DEFAULT_SERVER_IP_ADDR_BYTE4        (2ul)

#if !defined(THIS_IS_STACK_APPLICATION)
	extern BYTE AN0String[8];
#endif

// An actual function defined in Main.c for displaying the current IP
// address on the LCD.
void DisplayString(BYTE pos, char* text);

// UDP Socket for communication with DHCP Server
extern UDP_SOCKET DHCPServerSocket;
// UDP Socket for communication with DHCP Clients
extern UDP_SOCKET DHCPClientSocket;

// Buffer for broadcasts from clients
extern dhcpBuffer_t* DHCPClientBuffer;
// Buffer for replies from server
extern dhcpBuffer_t* DHCPServerBuffer;


#endif // _MAIN_H
size_t  strlcpy(char *dst, const char *src, size_t siz);
