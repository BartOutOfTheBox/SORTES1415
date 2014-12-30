/*
 * DHCPBuffer.c
 *
 * Provides a buffer to store DHCP messages in.
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#include "DHCPBuffer.h"
#include <malloc.h>

void freeDHCPBuffer(dhcpBuffer_t* buffer)
{
    // Fail if invalid buffer given
    if (!buffer)
        return;

    // Free the header
    if (buffer->BOOTPHeader)
        free((unsigned char _MALLOC_SPEC*) buffer->BOOTPHeader);
    buffer->BOOTPHeader = 0; // Reset pointer

    // Free the payload
    if (buffer->packetData)
        free((unsigned char _MALLOC_SPEC*) buffer->packetData);
    buffer->packetData = 0; // Reset pointer
    buffer->dataLength = 0; // Set length to 0

    // Mark buffer as free
    buffer->free = TRUE;
}
