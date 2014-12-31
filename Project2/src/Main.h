/*
 * Main.c
 *
 * DHCPRelay Main App
 * Based on MainDemo.h
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */
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

extern BOOL offerReceived;


#endif // _MAIN_H
size_t  strlcpy(char *dst, const char *src, size_t siz);
