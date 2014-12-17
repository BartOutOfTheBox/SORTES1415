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

void listen(UDP_SOCKET socket, dhcpBuffer_t* buffer) {
    WORD availableData;
    BYTE* packetData;
    WORD dataCount = 0;
    BOOTP_HEADER* BOOTPHeader;
    WORD oldBuffer = (WORD) buffer;
    char debugString[32];

    // Check to see if a valid DHCP packet has arrived
    if(UDPIsGetReady(socket) < 241u)
        return;

    BOOTPHeader = (BOOTP_HEADER *) malloc(sizeof(BOOTP_HEADER));
    if (!BOOTPHeader)
        return;

    if (!buffer) {

               DisplayString(0, "wrong place");
                        return;
                }

    sprintf(debugString, "%d",
                       buffer);
                        DisplayString(0, debugString);

    // Retrieve the BOOTP header
    UDPGetArray((BYTE*)BOOTPHeader, sizeof(BOOTP_HEADER));

    if (!buffer) {

           sprintf(debugString, "%d",
                   buffer);
                    DisplayString(16, debugString);
                    return;
            }

    availableData = UDPIsGetReady(socket);
    packetData = (BYTE *) malloc(availableData);
    if (!packetData)
    {
        DisplayString(0, "no room for data");
        free((unsigned char _MALLOC_SPEC*) BOOTPHeader);
        UDPDiscard();
        return;
    }

    while( dataCount < availableData && UDPGet(packetData + dataCount) ) {
        dataCount++;
    }

    addToDHCPBuffer(buffer, BOOTPHeader, packetData, availableData);

    UDPDiscard();
}
