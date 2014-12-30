/*********************************************************************
 *
 *  Main Application Entry Point for DHCPRelay
 *  Module for Microchip TCP/IP Stack
 *   -Relays messages between clients and a DHCP Server
 *	 -Reference: AN833
 *
 *********************************************************************
 * FileName:        Main.c
 * Dependencies:    TCPIP.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author              Date         Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti	4/19/01	Original (Rev. 1.0)
 * Nilesh Rajbharti	2/09/02	Cleanup
 * Nilesh Rajbharti	5/22/02	Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti	7/9/02	Rev 2.1 (See version.log for detail)
 * Nilesh Rajbharti	4/7/03	Rev 2.11.01 (See version log for detail)
 * Howard Schlunder	10/1/04	Beta Rev 0.9 (See version log for detail)
 * Howard Schlunder	10/8/04	Beta Rev 0.9.1 Announce support added
 * Howard Schlunder	11/29/04Beta Rev 0.9.2 (See version log for detail)
 * Howard Schlunder	2/10/05	Rev 2.5.0
 * Howard Schlunder	1/5/06	Rev 3.00
 * Howard Schlunder	1/18/06	Rev 3.01 ENC28J60 fixes to TCP, 
 *				UDP and ENC28J60 files
 * Howard Schlunder	3/01/06	Rev. 3.16 including 16-bit micro support
 * Howard Schlunder	4/12/06	Rev. 3.50 added LCD for Explorer 16
 * Howard Schlunder	6/19/06	Rev. 3.60 finished dsPIC30F support, 
 *                              added PICDEM.net 2 support
 * Howard Schlunder	8/02/06	Rev. 3.75 added beta DNS, NBNS, and HTTP client
 *                              (GenericTCPClient.c) services
 * Howard Schlunder	12/28/06Rev. 4.00RC added SMTP, Telnet, substantially 
 *                              modified TCP layer
 * Howard Schlunder	4/09/07 Rev. 4.02 added TCPPerformanceTest, 
 *                              UDPPerformanceTest, Reboot and fixed some bugs
 * Howard Schlunder	x/xx/07 Rev. 4.03
 * HSchlunder & EWood	8/27/07 Rev. 4.11
 * HSchlunder & EWood	10/08/07Rev. 4.13
 * HSchlunder & EWood	11/06/07Rev. 4.16
 * HSchlunder & EWood	11/08/07Rev. 4.17
 * HSchlunder & EWood	11/12/07Rev. 4.18
 * HSchlunder & EWood	2/11/08 Rev. 4.19
 * HSchlunder & EWood   4/26/08 Rev. 4.50 Moved most code to other files 
 *                                        for clarity
 * KHesky               7/07/08 Added ZG2100-specific support
 * HSchlunder & EWood   7/24/08 Rev. 4.51
 * Howard Schlunder	11/10/08Rev. 4.55
 * Howard Schlunder	04/14/09Rev. 5.00
 * Howard Schlunder	07/10/09Rev. 5.10
 * Marc Lobelle         03/15/10- adapted to support sdcc compiler
 *                              - removed some code not related to networking
 *                              - removed serial line support
 *                              - added a few debuging functions
 * Arne Van der Stappen 15/12/15-Copied from MainDemo.c
 ********************************************************************/
/*
 * This symbol uniquely defines this file as the main entry point.
 * There should only be one such definition in the entire project,
 * and this file must define the AppConfig variable as described below.
 * The processor configuration will be included in HardwareProfile.h
 * if this symbol is defined.
 */
#define THIS_INCLUDES_THE_MAIN_FUNCTION
#define THIS_IS_STACK_APPLICATION

// Include functions specific to this stack application
#include "Include/TCPIP_Stack/Delay.h"
#include "Main.h"
#include "receiver.h"
#include "messageProcessor.h"

// Declare AppConfig structure and some other supporting stack variables
APP_CONFIG AppConfig;
BYTE AN0String[8];

// Heap
unsigned char _MALLOC_SPEC heap[256];

// UDP Socket for communication with DHCP Server
UDP_SOCKET DHCPServerSocket;
// DHCP Server Info
NODE_INFO DHCP_Server;

// UDP Socket for communication with DHCP Clients
UDP_SOCKET DHCPClientSocket;

// Buffer for broadcasts from clients
dhcpBuffer_t* DHCPClientBuffer;
// Buffer for replies from server
dhcpBuffer_t* DHCPServerBuffer;

// Application is still initializing
typedef enum { Initialization, ARPSend, WaitForARPReply, Relaying } relayState_t;
relayState_t currentState = Initialization;

// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);
static void InitializeBoard(void);
static void InitializeDHCPRelay(void);

// Time of last ARP request
WORD arpTime;
// Time to wait till next ARP Request
WORD arpMaxTime = (TICK_SECOND/4)/256;
// ARP retries
int arpRetryCount = 0;

//
// Main application entry point.
//
#if defined(__18CXX) || defined(__SDCC__)
void main(void)
#else
int main(void)
#endif
{
static TICK t = 0; 
TICK nt = 0;  //TICK is DWORD, thus 32 bits
BYTE loopctr = 0;  //ML Debugging
WORD lloopctr = 14; //ML Debugging

static DWORD dwLastIP = 0;
	
    // Initialize interrupts and application specific hardware
    InitializeBoard();

    // Initialize and display message on the LCD
    LCDInit();
    DelayMs(100);
    DisplayString(0,"Relaying the");
    DisplayString(16,"future, today.");

    // Initialize Timer0, and low priority interrupts, used as clock.
    TickInit();

    // Initialize Stack and application related variables in AppConfig.
    InitAppConfig();

    // Initialize core stack layers (MAC, ARP, TCP, UDP) and
    // application modules (HTTP, SNMP, etc.)
    StackInit();

    // Set DHCP Server IP Address
    DHCP_Server.IPAddr.Val = DEFAULT_SERVER_IP_ADDR_BYTE1 |
                       DEFAULT_SERVER_IP_ADDR_BYTE2<<8ul | DEFAULT_SERVER_IP_ADDR_BYTE3<<16ul |
                       DEFAULT_SERVER_IP_ADDR_BYTE4<<24ul;

    currentState = ARPSend;

    while(1)
    {
        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();

        // Blink LEDs every second.
        nt =  TickGetDiv256();
        if((nt - t) >= (DWORD)(TICK_SECOND/1024ul))
        {
            t = nt;
            if (currentState == ARPSend || currentState == WaitForARPReply) {
                // Blink LED 1 (middle one)
                LED1_IO ^= 1;
            }
            else {
                // Blink LED 0 (left one)
                LED0_IO ^= 1;
            }
            ClrWdt();  //Clear the watchdog
        }

        switch (currentState)
        {
            case ARPSend:
               ARPResolve(&DHCP_Server.IPAddr); // Lookup MAC for DHCP Server
               arpTime = (WORD)nt;
               currentState = WaitForARPReply;
               break;
            case WaitForARPReply:
                // If ARP is resolved, open sockets
                if (ARPIsResolved(&DHCP_Server.IPAddr, &DHCP_Server.MACAddr)) {
                    LED1_IO = 0; // Turn off LED1
                    InitializeDHCPRelay(); // Initialize Sockets
                    currentState = Relaying;
                }
                else {
                    // Time out if too much time is spent in this state
                    // Note that this will continuously send out ARP
                    // requests for an infinite time if the Gateway
                    // never responds
                    if((WORD)nt - arpTime > arpMaxTime)
                    {
                        // Exponentially increase timeout until we reach 6
                        // attempts then stay constant
                        if(arpRetryCount < 6)
                        {
                            arpRetryCount++;
                            arpMaxTime <<= 1;
                        }

                        // Retransmit ARP request
                        currentState = ARPSend;
                    }
                }
                break;
            case Relaying:
                // Check incoming broadcasts
                receiveDHCPFromClientTask();
                // Check incoming server replies
                receiveDHCPFromServerTask();
                // Process and relay received broadcasts
                processClientMessage();
                // Process and relay received server replies
                processServerMessage();
                break;
        }
    }//end of while(1)
}//end of main()

// Create the buffers and open the proper sockets
void InitializeDHCPRelay(void)
{
    // Initialize the heap in the declared heap character array.
    _initHeap(heap, 1024);

    DHCPClientBuffer = (dhcpBuffer_t*) malloc(sizeof(dhcpBuffer_t));
    DHCPClientBuffer->free = TRUE;
    DHCPServerBuffer = (dhcpBuffer_t*) malloc(sizeof(dhcpBuffer_t));
    DHCPServerBuffer->free = TRUE;

    // Open the socket to listen to client broadcasts
    DHCPClientSocket = UDPOpen(DHCP_SERVER_PORT, NULL, DHCP_CLIENT_PORT);
    if(DHCPClientSocket == INVALID_UDP_SOCKET) {
        DisplayString(0, "Invalid DHCP Client Socket");
        return;
    }

    // Open the socket to listen to server messages
    DHCPServerSocket = UDPOpen(DHCP_SERVER_PORT, &DHCP_Server, DHCP_SERVER_PORT);
    if(DHCPServerSocket == INVALID_UDP_SOCKET) {
        DisplayString(0, "Invalid DHCP Server Socket");
        return;
    }
}

/*************************************************
 Function DisplayString: 
 Writes a string to the LCD display
 starting at pos
*************************************************/
void DisplayString(BYTE pos, char* text)
{
   BYTE l= strlen(text)+1;
   BYTE max= 32-pos;
   strlcpy((char*)&LCDText[pos], text,(l<max)?l:max );
   LCDUpdate();

}


/****************************************************************************
  Function:
    static void InitializeBoard(void)

  Description:
    This routine initializes the hardware.  It is a generic initialization
    routine for many of the Microchip development boards, using definitions
    in HardwareProfile.h to determine specific initialization.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void InitializeBoard(void)
{	
	// LEDs
        LED0_TRIS = 0;  //LED0
	LED1_TRIS = 0;  //LED1
	LED2_TRIS = 0;  //LED2
	LED3_TRIS = 0;  //LED_LCD1
	LED4_TRIS = 0;  //LED_LCD2
	LED5_TRIS = 0;  //LED5=RELAY1
	LED6_TRIS = 0;  //LED7=RELAY2
#if (!defined(EXPLORER_16) &&!defined(OLIMEX_MAXI))    // Pin multiplexed with 
	// a button on EXPLORER_16 and not used on OLIMEX_MAXI
	LED7_TRIS = 0;
#endif
        LED_PUT(0x00);  //turn off LED0 - LED2
	RELAY_PUT(0x00); //turn relays off to save power

	//set clock to 25 MHz

	// Enable PLL but disable pre and postscalers: the primary oscillator
        // runs at the speed of the 25MHz external quartz
	OSCTUNE = 0x40;

	// Switch to primary oscillator mode, 
        // regardless of if the config fuses tell us to start operating using 
        // the the internal RC
	// The external clock must be running and must be 25MHz for the 
	// Ethernet module and thus this Ethernet bootloader to operate.
        if(OSCCONbits.IDLEN) //IDLEN = 0x80; 0x02 selects the primary clock
		OSCCON = 0x82;
	else
		OSCCON = 0x02;

	// Enable Interrupts
	RCONbits.IPEN = 1;		// Enable interrupt priorities
        INTCONbits.GIEH = 1;
        INTCONbits.GIEL = 1;

}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/

static void InitAppConfig(void)
{
    // We have a static IP, so no DHCP Client is needed
	AppConfig.Flags.bIsDHCPEnabled = FALSE;
	AppConfig.Flags.bInConfigMode = TRUE;

//ML using sdcc (MPLAB has a trick to generate serial numbers)
// first 3 bytes indicate manufacturer; last 3 bytes are serial number
	// set last 3 bits to 0 to match MAC-address used by bootloader,
	// using a different MAC will confuse your pc making you unable to
	// upload software
	AppConfig.MyMACAddr.v[0] = 0;
	AppConfig.MyMACAddr.v[1] = 0x04;
	AppConfig.MyMACAddr.v[2] = 0xA3;
	AppConfig.MyMACAddr.v[3] = 0x00;
	AppConfig.MyMACAddr.v[4] = 0x00;
	AppConfig.MyMACAddr.v[5] = 0x00;

//ML if you want to change, see TCPIPConfig.h
	AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | 
            MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | 
            MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
	AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
	AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | 
            MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | 
            MY_DEFAULT_MASK_BYTE4<<24ul;
	AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
	AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | 
            MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | 
            MY_DEFAULT_GATE_BYTE4<<24ul;
	AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | 
            MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | 
            MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | 
            MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
	AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | 
            MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | 
            MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | 
            MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
}

//
// PIC18 Interrupt Service Routines
//
// NOTE: Several PICs, including the PIC18F4620 revision A3 have a RETFIE
// FAST/MOVFF bug
// The interruptlow keyword is used to work around the bug when using C18

//LowISR
#if defined(__18CXX)
    #if defined(HI_TECH_C)
        void interrupt low_priority LowISR(void)
    #elif defined(__SDCC__)
        void LowISR(void) __interrupt (2) //ML for sdcc
    #else
        #pragma interruptlow LowISR
        void LowISR(void)
    #endif
    {
    TickUpdate();
    }
    #if !defined(__SDCC__) && !defined(HI_TECH_C)
           //automatic with these compilers
        #pragma code lowVector=0x18
    void LowVector(void){_asm goto LowISR _endasm}
    #pragma code // Return to default code section
    #endif


//HighISR
    #if defined(HI_TECH_C)
        void interrupt HighISR(void)
    #elif defined(__SDCC__)
        void HighISR(void) __interrupt(1) //ML for sdcc
    #else
        #pragma interruptlow HighISR
        void HighISR(void)
    #endif
    {
      //insert here code for high level interrupt, if any
    }
    #if !defined(__SDCC__) && !defined(HI_TECH_C)
           //automatic with these compilers
    #pragma code highVector=0x8
    void HighVector(void){_asm goto HighISR _endasm}
    #pragma code // Return to default code section
    #endif

#endif

/*-------------------------------------------------------------------------
 *
 * strlcpy.c
 *    strncpy done right
 *
 * This file was taken from OpenBSD and is used on platforms that don't
 * provide strlcpy().  The OpenBSD copyright terms follow.
 *-------------------------------------------------------------------------
 */

/*  $OpenBSD: strlcpy.c,v 1.11 2006/05/05 15:27:38 millert Exp $    */

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 * Function creation history:  http://www.gratisoft.us/todd/papers/strlcpy.html
 */
size_t
strlcpy(char *dst, const char *src, size_t siz)
{
    char       *d = dst;
    const char *s = src;
    size_t      n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0)
    {
        while (--n != 0)
        {
            if ((*d++ = *s++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0)
    {
        if (siz != 0)
            *d = '\0';          /* NUL-terminate dst */
        while (*s++)
            ;
    }

    return (s - src - 1);       /* count does not include NUL */
}
