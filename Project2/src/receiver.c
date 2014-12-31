/*
 * receiver.c
 *
 * Checks for incoming DHCP messages and stores them in a buffer
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
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

// Check for broadcasts from clients
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

// Check for replies from the server
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

    // Check to see if a valid DHCP packet has arrived
    availableData = UDPIsGetReady(socket);
    if(availableData < 241u)
        return;
    if(offerReceived) {
        // TCPIP Stack crashes on first message after OFFER. Reset.
        Reset();
        return;
    }
    // Allocate the header
    BOOTPHeader = (BOOTP_HEADER *) malloc(sizeof(BOOTP_HEADER));
    if (!BOOTPHeader) {
        UDPDiscard();
        return;
    }

    // Retrieve the BOOTP header
    //UDPGetArray((BYTE*)BOOTPHeader, sizeof(BOOTP_HEADER));
    for (i = 0; i < sizeof(BOOTP_HEADER) && UDPGet((BYTE *)BOOTPHeader + i); i++);

    availableData = availableData - sizeof(BOOTP_HEADER);
    // Allocate the payload
    packetData = (BYTE *) malloc(availableData);
    if (!packetData)
    {
        // Not enough room for payload. Free header and discard the UDP packet
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
    for (i = 0; i < sizeof(DWORD) && UDPGet((BYTE *)packetData + dataCount + i); i++);
    //UDPGetArray((BYTE*)(packetData + dataCount), sizeof(DWORD));
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
        if (Len + *Len > packetData + availableData) {
            // Invalid packet
            UDPDiscard();
            free((unsigned char _MALLOC_SPEC*) BOOTPHeader);
            free((unsigned char _MALLOC_SPEC*) packetData);
            return;
        }

        // Read option
        for (i = 0; i < *Len && UDPGet((BYTE*)(packetData + dataCount + i)); i++);
        dataCount += *Len;
        //dataCount += UDPGetArray((BYTE*)(packetData + dataCount), *Len);
    }
    if (dataCount > availableData) {
        dataCount = availableData;
    }
    // Discard any remaining bytes in the packet that didn't get read
    UDPDiscard();

    // Store the data
    buffer->BOOTPHeader = BOOTPHeader;
    buffer->packetData = packetData;
    buffer->dataLength = dataCount;
    buffer->free = FALSE;
}
