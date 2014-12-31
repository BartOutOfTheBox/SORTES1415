/*
 * messageProcessor.c
 *
 * Processes and relays received DHCP messages
 *
 *  Created on: 16-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#include "messageProcessor.h"
#include "DHCPBuffer.h"
#include "Main.h"

// Relay a DHCP Packet on the given socket
BOOL sendMessage(UDP_SOCKET socket, dhcpBuffer_t* message);

BOOL isOfferMessage(dhcpBuffer_t* message);

// Process and relay any broadcasts from clients
void processClientMessage(void)
{
    if (!DHCPClientBuffer->free)
    {
        // Broadcast available. Process it and attempt to relay it.
        // Set Relay IP to own IP
        DHCPClientBuffer->BOOTPHeader->RelayAgentIP.Val = AppConfig.MyIPAddr.Val;
        // Forward message to server
        if (sendMessage(DHCPServerSocket, DHCPClientBuffer)) {
            // Free buffer
            freeDHCPBuffer(DHCPClientBuffer);
        }
    }
}

// Process and relay any replies from servers
void processServerMessage(void)
{
    if (!DHCPServerBuffer->free)
    {
        // Reply available. Attempt to relay it.
        // Broadcast message
        if (sendMessage(DHCPClientSocket, DHCPServerBuffer)) {
            if (isOfferMessage(DHCPServerBuffer)) {
                offerReceived = TRUE;
            }
            // Free buffer
            freeDHCPBuffer(DHCPServerBuffer);
        }
    }
}

BOOL sendMessage(UDP_SOCKET socket, dhcpBuffer_t* message) {
    // Total length of packet
    WORD size = sizeof(BOOTP_HEADER) + message->dataLength;
    // Ready the socket for transmision
    if(UDPIsPutReady(socket) < size) {
        UDPFlush();
        return FALSE;
    }

    // Fill in the header
    UDPPutArray((BYTE*)message->BOOTPHeader, sizeof(BOOTP_HEADER));
    // Fill in the payload
    UDPPutArray((BYTE*)message->packetData, message->dataLength);

    while(UDPTxCount < 300u)
            UDPPut(0);

    // Transmit the packet
    UDPFlush();
    return TRUE;
}

BOOL isOfferMessage(dhcpBuffer_t* message) {
    WORD dataCount = 64+128+(16-sizeof(MAC_ADDR)) + sizeof(DWORD);
    BYTE* Option;
    BYTE* Len;
    BYTE* i;

    while(dataCount < message->dataLength)
    {
        Option = (message->packetData + dataCount++);

        if(*Option == DHCP_END_OPTION)
            return FALSE;
        Len = (BYTE*)(message->packetData + dataCount++);

        if (*Option == DHCP_MESSAGE_TYPE) {
            i = (BYTE*)(message->packetData + dataCount);
            return (*i == DHCP_OFFER_MESSAGE);
        }
        dataCount += *Len;
    }
    return FALSE;

}
