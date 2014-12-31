/*
 * Main.c
 *
 * DHCPRelay Main App
 * Based on MainDemo.c
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */
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

BOOL offerReceived = FALSE;

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

        // Time
        nt = TickGetDiv256();

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
