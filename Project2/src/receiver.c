/*
 * receiver.c
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */


#include "receiver.h"
#include "Main.h"
#include "DHCPBuffer.h"
#include "Include/GenericTypeDefs.h"

void listen(UDP_SOCKET socket, dhcpBuffer_t* buffer);

void receiveDHCPFromClientTask(void)
{
    if (!DHCPClientBuffer)
        DisplayString(0, "buffer = gone");
    else
        listen(DHCPClientSocket, DHCPClientBuffer);
}

/**
 * Listen on a socket and store any received dhcp packets in the given buffer
 *
 * @param socket The socket to read from
 * @param buffer The buffer to store the DHCP packet in
 */
void listen(UDP_SOCKET socket, dhcpBuffer_t* buffer) {
    WORD availableData; // data in packet
    BYTE* packetData; // packetData
    WORD dataCount;
    BOOTP_HEADER* BOOTPHeader; // packetHeader
    char debugString[32]; // for debugging purposes

    // Check to see if a valid DHCP packet has arrived
    availableData = UDPIsGetReady(socket);
    if(availableData < 241u)
        return;

    // Read the header
    BOOTPHeader = (BOOTP_HEADER *) malloc(sizeof(BOOTP_HEADER));
    if (!BOOTPHeader)
        return;

    // Retrieve the BOOTP header
    dataCount = 0;
    // Use UDPGet instead of UDPGetArray as the latter seems to cause overflow
    while (dataCount < sizeof(BOOTP_HEADER) && UDPGet((BYTE *)BOOTPHeader + dataCount)) {
        dataCount++;
    }
//    UDPGetArray((BYTE*)BOOTPHeader, sizeof(BOOTP_HEADER));

    ///////////////////////////
    // DEBUG #2
    //
    // Prints address of buffer on first packet
    // Prints 0 of second buffer
    ///////////////////////////
//    sprintf(debugString, "%d",
//                   buffer);
//    DisplayString(16, debugString);


    // Check how much data is left
    availableData = availableData - sizeof(BOOTP_HEADER);
    packetData = (BYTE *) malloc(availableData);
    if (!packetData)
    {
        DisplayString(0, "no room for data");
        free((unsigned char _MALLOC_SPEC*) BOOTPHeader);
        UDPDiscard();
        return;
    }

    // Get the remaining data of the packet
    dataCount = 0;
    // Use UDPGet instead of UDPGetArray as the latter seems to cause overflow
    while( dataCount < availableData && UDPGet(packetData + dataCount) ) {
        dataCount++;
    }

    // Add it to the buffer
    addToDHCPBuffer(buffer, BOOTPHeader, packetData, availableData);
    //addToDHCPBuffer(0, BOOTPHeader, packetData, availableData);

    // Discard the rest of the packet
    UDPDiscard();
}
