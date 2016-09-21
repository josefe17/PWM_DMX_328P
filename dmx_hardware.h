/*
 * dmx_hardware.h
 *
 * Created: 20/09/2016 22:20:12
 *  Author: josefe
 */ 

#include <avr/io.h>

#ifndef DMX_HARDWARE_H_
#define DMX_HARDWARE_H_

/*Hardware registers*/
#define DMX_RX_DATA		UDR0
#define DMX_RXIF		(UCSR0A & (1<<RXC0))
#define DMX_RX_STATUS	UCSR0A
#define DMX_BRL			UBRR0L
#define DMX_BRH			UBRR0H
#define DMX_UCSRA		UCSR0A
#define DMX_UCSRB		UCSR0B
#define DMX_UCSRC		UCSR0C
#define DMX_FE			FE0
#define DMX_DOR			DOR0

/*Preloads for desired baud rates and interrupts*/
/*Needs 250 Kbps*/
#define  BRL_LOAD 4
#define  BRH_LOAD 0
#define	 UCSRA_LOAD 0b00000000
#define  UCSRB_LOAD 0b10010100
#define	 UCSRB_RESET_LOAD 0b10000100
#define  UCSRC_LOAD 0b00000110



#endif /* DMX_HARDWARE_H_ */