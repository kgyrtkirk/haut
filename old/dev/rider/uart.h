/*
 * uart.h
 *
 *  Created on: Mar 1, 2013
 *      Author: kirk
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>

// create a FILE structure to reference our UART output function

static FILE uartout = {0} ;

void uart_init();
static int uart_putchar(char c, FILE *stream);


#endif /* UART_H_ */
