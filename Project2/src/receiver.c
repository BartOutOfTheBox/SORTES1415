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

/**
 * Listen on a socket and store any received dhcp packets in the given buffer
 *
 * @param socket The socket to read from
 * @param buffer The buffer to store the DHCP packet in
 */
void listen(UDP_SOCKET socket, dhcpBuffer_t* buffer);

void receiveDHCPFromClientTask(void)
{
    if (!DHCPClientBuffer) {
        return;
    }
    else if (DHCPClientBuffer->free) {
        // Buffer free, listen for incoming broadcasts
        listen(DHCPClientSocket, DHCPClientBuffer);
    }
}

void receiveDHCPFromServerTask(void)
{
    if (!DHCPServerBuffer)
        return;
    else if (DHCPServerBuffer->free) {
        // Buffer free, listen for incoming replies
        listen(DHCPServerSocket, DHCPServerBuffer);
    }
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
    BYTE i;
    BYTE* Option;
    BYTE* Len;
    char debugString[32]; // for debugging purposes
    // Check to see if a valid DHCP packet has arrived
    availableData = UDPIsGetReady(socket);
    if(availableData < 241u)
        return;
    // Read the header
    BOOTPHeader = (BOOTP_HEADER *) malloc(sizeof(BOOTP_HEADER));
    if (!BOOTPHeader) {
        DisplayString(16, "no room for header");
        UDPDiscard();
        return;
    }

    // Retrieve the BOOTP header
    UDPGetArray((BYTE*)BOOTPHeader, sizeof(BOOTP_HEADER));
    // Use UDPGet instead of UDPGetArray as the latter seems to cause overflow
    //for (i = 0; i < sizeof(BOOTP_HEADER) && UDPGet((BYTE *)BOOTPHeader + i); i++);

    availableData = availableData - sizeof(BOOTP_HEADER);
    packetData = (BYTE *) malloc(availableData);
    if (!packetData)
    {
        // Not enough room for payload. Free header and discard the UDP packet
        DisplayString(0, "no room for data");
        free((unsigned char _MALLOC_SPEC*) BOOTPHeader);
        UDPDiscard();
        return;
    }

    // Read 10 bytes of hardware address,
    // server host name, and boot file name
    // Let the server/client decide if it is needed or not
    for(i = 0; i < 64+128+(16-sizeof(MAC_ADDR)); i++)
        UDPGet(packetData + i);

    dataCount = i;
    // Obtain Magic Cookie.
    // Let the server/client verify it
    UDPGetArray((BYTE*)(packetData + dataCount), sizeof(DWORD));
    dataCount += sizeof(DWORD);
    // Obtain options
    while(dataCount < availableData)
    {
        // Get option type
        Option = (BYTE*)(packetData + dataCount++);
        if(!UDPGet(Option))
            break;
        if(*Option == DHCP_END_OPTION)
            break;
        Len = (BYTE*)(packetData + dataCount++);

        // Get option length
        UDPGet(Len);

        // Read option
        //for (i = 0; i < *Len && UDPGet((BYTE*)(packetData + dataCount + i)); i++);
        UDPGetArray((BYTE*)(packetData + dataCount), *Len);
        dataCount += *Len;
    }
    if (dataCount > availableData) {
        dataCount = availableData;
    }
    // Discard any remaining bytes in the packet that didn't get read
    UDPDiscard();
    ///////////////////////////
    // DEBUG #2
    //
    // Prints address of buffer on first packet
    // Prints 0 of second buffer
    ///////////////////////////
//    sprintf(debugString, "%d",
//                   buffer);
//    DisplayString(16, debugString);


    // Store the data
    buffer->BOOTPHeader = BOOTPHeader;
    buffer->packetData = packetData;
    buffer->dataLength = dataCount;
    buffer->free = FALSE;

    UDPDiscard();
}
