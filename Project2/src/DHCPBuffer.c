/*
 * DHCPBuffer.c
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne
 */

#include "DHCPBuffer.h"
#include <malloc.h>
#include "main.h"

void freeDHCPBuffer(dhcpBuffer_t* buffer)
{
    // Fail if invalid item given
    if (!buffer)
        return;

    // Free the header
    if (buffer->BOOTPHeader)
        free((unsigned char _MALLOC_SPEC*) buffer->BOOTPHeader);
    buffer->BOOTPHeader = 0;

    // Free the payload
    if (buffer->packetData)
        free((unsigned char _MALLOC_SPEC*) buffer->packetData);
    buffer->packetData = 0;
    buffer->dataLength = 0;

    // Mark buffer as free
    buffer->free = TRUE;
}
