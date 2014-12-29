/*
 * DHCPBuffer.h
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */

#ifndef DHCPBUFFER_H_
#define DHCPBUFFER_H_

typedef struct dhcpBuffer dhcpBuffer_t;

#include "Main.h"
#include "Include/HardwareProfile.h"
#include "Include/TCPIP_Stack/TCPIP.h"

struct dhcpBuffer {
    BOOTP_HEADER* BOOTPHeader;
    BYTE *packetData;
    WORD dataLength;
    BOOL free;
};

void freeDHCPBuffer(dhcpBuffer_t* item);

#endif /* DHCPBUFFER_H_ */
