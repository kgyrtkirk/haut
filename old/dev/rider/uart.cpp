/*
 * uart.cpp
 *
 *  Created on: Mar 1, 2013
 *      Author: kirk
 */

#include	"Arduino.h"
#include	"stdio.h"
#include	"uart.h"


void uart_init() {
//	Serial.begin(9600);
	Serial.begin(115200);

	// fill in the UART file descriptor with pointer to writer.
	fdev_setup_stream(&uartout, uart_putchar, NULL, _FDEV_SETUP_WRITE);

	// The uart is the standard output device STDOUT.
	stdout = &uartout;

}

static int uart_putchar(char c, FILE *stream) {
	Serial.write(c);
	return 0;
}
