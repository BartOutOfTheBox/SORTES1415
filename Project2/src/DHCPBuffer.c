/*
 * DHCPBuffer.c
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */

#include "DHCPBuffer.h"
#include <malloc.h>
#include "main.h"

dhcpBuffer_t* createNewBuffer(void) {
    dhcpBuffer_t* buffer = (dhcpBuffer_t *) malloc(sizeof(dhcpBuffer_t));
    if (!buffer) {
        DisplayString(0, "no room for buffer");
        return 0;
    }
    buffer->head = 0;
    buffer->tail = 0;
    return buffer;
}

void addToDHCPBuffer(dhcpBuffer_t* packetBuffer, BOOTP_HEADER* header, BYTE* packetData, WORD dataLength)
{
    dhcpBufferItem_t* newItem;
    char debugString[32];

    // Check if we got a valid buffer. If not, free the data.
    if (!packetBuffer)
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
    if (!packetBuffer)
            {
                DisplayString(0, "no buffer"); // For debugging purposes
            }
    // Set the proper pointers
    newItem->BOOTPHeader = header;
    if (!packetBuffer)
            {
                DisplayString(1, "no buffer"); // For debugging purposes
            }
    newItem->packetData = packetData;
    if (!packetBuffer)
            {
                DisplayString(2, "no buffer"); // For debugging purposes
            }
    newItem->dataLength = dataLength;
    if (!packetBuffer)
            {
                DisplayString(3, "no buffer"); // For debugging purposes
            }
    newItem->next = 0;
    if (!packetBuffer)
    {
        DisplayString(4, "no buffer"); // For debugging purposes
    }

    // If we have no tail, our buffer is still empty
    if (!packetBuffer->tail)
    {
        packetBuffer->head = newItem;
    }
    else
    {
        packetBuffer->tail->next = newItem;
    }
    if (!packetBuffer)
        {
            DisplayString(6, "no buffer"); // For debugging purposes
        }
    packetBuffer->tail = newItem;
    sprintf(debugString, "buf:%d",
            packetBuffer);
            DisplayString(16, debugString);

    DisplayString(8, "added");

}

dhcpBufferItem_t* getFromDHCPBuffer(dhcpBuffer_t* buffer)
{
    dhcpBufferItem_t* bufferItem;
    char debugString[32];

    if (!buffer) {
        return 0;
    }

    sprintf(debugString, "%d",
                       buffer);
        DisplayString(24, debugString);

    if (!buffer->head) {
        return 0;
    }

    DisplayString(30, "9");


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
