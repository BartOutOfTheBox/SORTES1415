/*
 * main.c
 *
 *  Created on: 20-nov.-2014
 *      Author: Arne
 */


#define __18F97J60
#define __SDCC__
#include "Include/HardwareProfile.h"

#include <string.h>
#include <stdlib.h>
#include <pic18fregs.h>  //defines the address corresponding to the symbolic
                         //names of the sfr

#include "Include/LCDBlocking.h"

#define CLOCK_FREQ 30000000 // 30 Mhz
#define EXEC_FREQ CLOCK_FREQ/4 // 4 clock cycles to execute an instruction

#define INTERRUPTS_PER_SECOND 117187
#define PRESCALER 4

int interrupts;

typedef enum { false, true } bool;

// wait for approx 1ms
void delay_1ms(void) {
    TMR0H=(0x10000-EXEC_FREQ/1000)>>8;
    TMR0L=(0x10000-EXEC_FREQ/1000)&0xff;
    T0CONbits.TMR0ON=0; // disable timer0
    T0CONbits.T08BIT=0; // use timer0 16-bit counter
    T0CONbits.T0CS=0; // use timer0 instruction cycle clock
    T0CONbits.PSA=1; // disable timer0 prescaler
    INTCONbits.T0IF=0; // clear timer0 overflow bit
    T0CONbits.TMR0ON=1; // enable timer0
    while (!INTCONbits.T0IF) {} // wait for timer0 overflow
    INTCONbits.T0IF=0; // clear timer0 overflow bit
    T0CONbits.TMR0ON=0; // disable timer0
}

// wait for some ms
void delay_ms(unsigned int ms) {
    while (ms--) {
        delay_1ms();
    }
}

void displayString(BYTE pos, char* text);

void main(void)
   {
   LED1_TRIS = 0; //configure 2nd led pin as output (red)

   LCDInit();
   delay_ms(100);


   displayString(0, "Good morning");
   displayString(16, "Louvain-la-Neuve");

   LCDUpdate();

   /* main loop of this toy program: turn led 2 on and off every second */
   LED1_IO=1;
   while(1)
   {
      delay_ms(1000);
      LED1_IO^=1;
   }
}


/*************************************************
 Function displayString:
 Writes the first characters of the string in the remaining
 space of the 32 positions LCD, starting at pos
 (does not use strlcopy, so can use up to the 32th place)
*************************************************/

void displayString(BYTE pos, char* text)
{
   BYTE        l = strlen(text);/*number of actual chars in the string*/
   BYTE      max = 32-pos;    /*available space on the lcd*/
   char       *d = (char*)&LCDText[pos];
   const char *s = text;
   size_t      n = (l<max)?l:max;
   /* Copy as many bytes as will fit */
    if (n != 0)
      while (n-- != 0)*d++ = *s++;
   LCDUpdate();
}
