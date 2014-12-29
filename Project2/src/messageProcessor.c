/*
 * messageProcessor.c
 *
 *  Created on: 16-dec.-2014
 *      Author: Arne
 */

#include "messageProcessor.h"
#include "DHCPBuffer.h"
#include "main.h"

typedef enum { Polling, Processing } processorState_t;
static processorState_t clientProcessorState = Polling;
static processorState_t serverProcessorState = Polling;

static dhcpBufferItem_t* clientMessage;
static dhcpBufferItem_t* serverMessage;

BOOL processAndForwardClientMessage(void);
BOOL processAndForwardServerMessage(void);
BOOL sendMessage(UDP_SOCKET socket, dhcpBufferItem_t* message);

void processClientMessage(void)
{
    switch (clientProcessorState)
    {
        case Polling:
            clientMessage = getFromDHCPBuffer(DHCPClientBuffer);
            if (clientMessage != 0) {
                DisplayString(0, "got client message");
                clientProcessorState = Processing;
            }
            break;
        case Processing:
            //DisplayString(0, "processing broadcast");
            if (processAndForwardClientMessage()) {
                clientProcessorState = Polling;
            }
            break;
    }
}

void processServerMessage(void)
{
    switch (serverProcessorState)
    {
        case Polling:
            serverMessage = getFromDHCPBuffer(DHCPServerBuffer);
            if (serverMessage) {
                serverProcessorState = Processing;
            }
            break;
        case Processing:
            DisplayString(0, "processing server message");
            if (processAndForwardServerMessage()) {
                serverProcessorState = Polling;
            }
            break;
    }
}

BOOL processAndForwardClientMessage(void)
{
    if (!clientMessage)
        return TRUE;
    clientMessage->BOOTPHeader->RelayAgentIP.Val = AppConfig.MyIPAddr.Val;
    if (sendMessage(DHCPServerSocket, clientMessage)) {
        DisplayString(0, "goodbye message!");
        freeDHCPBufferItem(clientMessage);
        clientMessage = 0;
        return TRUE;
    }
    return FALSE;
}

BOOL processAndForwardServerMessage(void)
{
    return FALSE;
}

BOOL sendMessage(UDP_SOCKET socket, dhcpBufferItem_t* message) {
    WORD size = sizeof(BOOTP_HEADER) + message->dataLength;
    //DisplayString(0, "sending message");
    if(UDPIsPutReady(socket) < size) {
        //DisplayString(0, "not enough putroom");
        return FALSE;
    }

    UDPPutArray((BYTE*)message->BOOTPHeader, sizeof(BOOTP_HEADER));
    UDPPutArray((BYTE*)message->packetData, message->dataLength);

    // Transmit the packet
    UDPFlush();
    DisplayString(0, "flushing");
    return TRUE;
}
