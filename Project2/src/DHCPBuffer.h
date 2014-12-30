/*
 * DHCPBuffer.h
 *
 * Provides a buffer to store DHCP messages in.
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#ifndef DHCPBUFFER_H_
#define DHCPBUFFER_H_

typedef struct dhcpBuffer dhcpBuffer_t;

#include "Main.h"
#include "Include/HardwareProfile.h"
#include "Include/TCPIP_Stack/TCPIP.h"

// Buffer for DCHP Messages
struct dhcpBuffer {
    BOOTP_HEADER* BOOTPHeader; // Header
    BYTE *packetData; // Payload
    WORD dataLength; // Length of payload
    BOOL free; // Buffer is free or not
};

// Free DHCP Buffer
void freeDHCPBuffer(dhcpBuffer_t* buffer);

#endif /* DHCPBUFFER_H_ */
