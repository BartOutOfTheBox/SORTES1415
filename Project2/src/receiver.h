/*
 * receiver.h
 *
 * Checks for incoming DHCP messages and stores them in a buffer
 *
 *  Created on: 15-dec.-2014
 *      Author: Arne Van der Stappen & Bart Verhoeven
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

// Check for incoming replies from the server
void receiveDHCPFromServerTask(void);

// Check for incoming broadcasts from clients
void receiveDHCPFromClientTask(void);

#endif /* RECEIVER_H_ */
