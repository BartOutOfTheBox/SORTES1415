/*
 * DHCPBuffer.h
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */

#ifndef DHCPBUFFER_H_
#define DHCPBUFFER_H_

typedef struct dhcpBufferItem dhcpBufferItem_t;
typedef struct dhcpBuffer dhcpBuffer_t;

#include "Main.h"
#include "Include/HardwareProfile.h"
#include "Include/TCPIP_Stack/TCPIP.h"

struct dhcpBufferItem {
    BOOTP_HEADER* BOOTPHeader;
    BYTE *packetData;
    WORD dataLength;
    struct dhcpBufferItem *next;
};

struct dhcpBuffer {
    dhcpBufferItem_t* head;
    dhcpBufferItem_t* tail;
};

dhcpBuffer_t* createNewBuffer(void);
void addToDHCPBuffer(dhcpBuffer_t* buffer, BOOTP_HEADER* header, BYTE* data, WORD dataLength);

dhcpBufferItem_t* getFromDHCPBuffer(dhcpBuffer_t* buffer);
void freeDHCPBufferItem(dhcpBufferItem_t* item);

#endif /* DHCPBUFFER_H_ */
