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
static processorState_t clientProcessorState;
static processorState_t serverProcessorState;

static dhcpBufferItem_t* clientMessage;
static dhcpBufferItem_t* serverMessage;

void processAndForwardClientMessage(void);

void processClientMessage(void)
{
    switch (clientProcessorState)
    {
        case Polling:
            clientMessage = getFromDHCPBuffer(DHCPClientBuffer);
            if (clientMessage) {
                DisplayString(0, "got client message");
                clientProcessorState = Processing;
            }
            break;
        case Processing:
            DisplayString(0, "processing broadcast");
            clientProcessorState = Polling;
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
            serverProcessorState = Polling;
            break;
    }
}

void processAndForwardClientMessage(void)
{
    freeDHCPBufferItem(clientMessage);
    DisplayString(0, "goodbye message!");
    clientMessage = 0;
}
