/*
 * DHCPBuffer.c
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */

#include "Include/DHCPBuffer.h"
#include <malloc.h>

void addToDHCPBuffer(dhcpBuffer_t* buffer, BOOTP_HEADER* header, BYTE* packetData, WORD dataLength)
{
    dhcpBufferItem_t* newItem;

    if (!buffer)
    {
        free((unsigned char _MALLOC_SPEC*) header);
        free((unsigned char _MALLOC_SPEC*) packetData);
        return;
    }

    newItem = (dhcpBufferItem_t*) malloc(sizeof(dhcpBufferItem_t));
    if (!newItem)
    {
        free((unsigned char _MALLOC_SPEC*) header);
        free((unsigned char _MALLOC_SPEC*) packetData);
        return;
    }

    newItem->BOOTPHeader = header;
    newItem->packetData = packetData;
    newItem->dataLength = dataLength;

    if (!buffer->tail)
    {
        buffer->head = newItem;
    }
    else
    {
        buffer->tail->next = newItem;
    }
    buffer->tail = newItem;
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
