/*
 * receiver.c
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */


#include "Include/receiver.h"
#include "Include/Main.h"
#include "Include/DHCPBuffer.h"
#include "Include/GenericTypeDefs.h"

void listen(UDP_SOCKET socket, dhcpBuffer_t* buffer);

void receiveDHCPFromClientTask(void)
{
    listen(DHCPClientSocket, &DHCPClientBuffer);
}

void listen(UDP_SOCKET socket, dhcpBuffer_t* buffer) {
    WORD availableData;
    BYTE* packetData;
    BOOTP_HEADER* BOOTPHeader;

    // Check to see if a valid DHCP packet has arrived
    if(UDPIsGetReady(socket) < 241u)
        return;

    DisplayString(0, "dhcp packet received!");

    BOOTPHeader = (BOOTP_HEADER *) malloc(sizeof(BOOTP_HEADER));
    if (!BOOTPHeader)
        return;

    // Retrieve the BOOTP header
    UDPGetArray((BYTE*)&BOOTPHeader, sizeof(BOOTP_HEADER));

    availableData = UDPIsGetReady(socket);
    packetData = (BYTE *) malloc(availableData);
    if (!packetData)
    {
        free((unsigned char _MALLOC_SPEC*) BOOTPHeader);
        UDPDiscard();
        return;
    }

    UDPGetArray(packetData, availableData);

    addToDHCPBuffer(buffer, (BOOTP_HEADER*) &BOOTPHeader, packetData, availableData);

    UDPDiscard();
}
