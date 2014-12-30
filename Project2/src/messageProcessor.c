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

// Process and relay any broadcasts from clients
void processClientMessage(void)
{
    if (!DHCPClientBuffer->free)
    {
        // Broadcast available. Process it and attempt to relay it.
        //DisplayString(0, "got client msg"); // For debugging purposes
        // Set Relay IP to own IP
        DHCPClientBuffer->BOOTPHeader->RelayAgentIP.Val = AppConfig.MyIPAddr.Val;
        // Forward message to server
        if (sendMessage(DHCPServerSocket, DHCPClientBuffer)) {
            // Free buffer
            freeDHCPBuffer(DHCPClientBuffer);
            //DisplayString(16, "sent to server");
        }
    }
}

// Process and relay any replies from servers
void processServerMessage(void)
{
    if (!DHCPServerBuffer->free)
    {
        // Reply available. Attempt to relay it.
        //DisplayString(0, "got server msg"); // For debugging purposes
        // Broadcast message
        if (sendMessage(DHCPClientSocket, DHCPServerBuffer)) {
            // Free buffer
            freeDHCPBuffer(DHCPServerBuffer);
            //DisplayString(16, "broadcasted");
        }
    }
}

BOOL sendMessage(UDP_SOCKET socket, dhcpBuffer_t* message) {
    // Total length of packet
    WORD size = sizeof(BOOTP_HEADER) + message->dataLength;
    //DisplayString(0, "sending message");
    // Ready the socket for transmision
    if(UDPIsPutReady(socket) < size) {
        UDPFlush();
        return FALSE;
    }

    // Fill in the header
    UDPPutArray((BYTE*)message->BOOTPHeader, sizeof(BOOTP_HEADER));
    // Fill in the payload
    UDPPutArray((BYTE*)message->packetData, message->dataLength);

    // Transmit the packet
    UDPFlush();
    return TRUE;
}
