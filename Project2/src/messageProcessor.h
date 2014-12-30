/*
 * messageProcessor.h
 *
 * Processes and relays received DHCP messages
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#ifndef MESSAGEPROCESSOR_H_
#define MESSAGEPROCESSOR_H_

// Process and relay any broadcasts from clients
void processClientMessage(void);

// Process and relay any replies from servers
void processServerMessage(void);

#endif /* MESSAGEPROCESSOR_H_ */
