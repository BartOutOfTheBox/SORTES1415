/*
 * DHCPBuffer.c
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */

#include "DHCPBuffer.h"
#include <malloc.h>
#include "main.h"

void addToDHCPBuffer(dhcpBuffer_t* buffer, BOOTP_HEADER* header, BYTE* packetData, WORD dataLength)
{
    dhcpBufferItem_t* newItem;

    // Check if we got a valid buffer. If not, free the data.
    if (!buffer)
    {
        //DisplayString(0, "no buffer"); // For debugging purposes
        free((unsigned char _MALLOC_SPEC*) header);
        free((unsigned char _MALLOC_SPEC*) packetData);
        return;
    }

    // Try to allocate a new buffer item
    newItem = (dhcpBufferItem_t*) malloc(sizeof(dhcpBufferItem_t));
    if (!newItem)
    {
        //DisplayString(0, "no mem for newItem"); // For debugging purposes
        free((unsigned char _MALLOC_SPEC*) header);
        free((unsigned char _MALLOC_SPEC*) packetData);
        return;
    }

    // Set the proper pointers
    newItem->BOOTPHeader = header;
    newItem->packetData = packetData;
    newItem->dataLength = dataLength;

    // If we have no tail, our buffer is still empty
    if (!buffer->tail)
    {
        buffer->head = newItem;
    }
    else
    {
        buffer->tail->next = newItem;
    }
    buffer->tail = newItem;

    DisplayString(0, "added to buffer");
}

dhcpBufferItem_t* getFromDHCPBuffer(dhcpBuffer_t* buffer)
{
    dhcpBufferItem_t* bufferItem;

    if (!buffer)
        return 0;

    if (!buffer->head)
        return 0;

    bufferItem = buffer->head;

    buffer->head = bufferItem->next;

    if(!buffer->head)
        buffer->tail = 0;

    bufferItem->next = 0;
    return bufferItem;
}

void freeDHCPBufferItem(dhcpBufferItem_t* item)
{
    if (!item)
        return;

    if (item->BOOTPHeader)
        free((unsigned char _MALLOC_SPEC*) item->BOOTPHeader);

    if (item->packetData)
        free((unsigned char _MALLOC_SPEC*) item->packetData);

    free((unsigned char _MALLOC_SPEC*) item);
}
