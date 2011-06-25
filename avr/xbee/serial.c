#include "hardware.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#include "serial.h"
#define SERIAL_BITTIMER  F_CPU/SERIAL_BAUDRATE/16-1


int serial_tx_flag = 0;

int serial_stdin(FILE *stream)
{
	return serial_getchar();
}

int serial_stdout(char c, FILE *stream)
{
	serial_putchar(c);
	serial_flush();
	return 0;
}

FILE serial_stdio = FDEV_SETUP_STREAM(serial_stdout, serial_stdin, _FDEV_SETUP_RW);

void serial_init(void)
{
	queue_clear(&serial_in);
	queue_clear(&serial_out);
	stdin = stderr = stdout = &serial_stdio;

	/* Set the baud rate */
	UBRR0H = (SERIAL_BITTIMER >> 8) & 0xff;
	UBRR0L = SERIAL_BITTIMER & 0xff;
	/* set the framing to 8N1 */
	UCSR0C = (3 << UCSZ00);
	/* Engage! */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	sei();
	return;
}

ISR(USART_RX_vect)
{
	queue_push(&serial_in, UDR0);
}

ISR(USART_UDRE_vect)
{
	if((serial_out.size) > 0) {
		UDR0 = queue_pop(&serial_out);
		serial_tx_flag = 1;
	}
	if((serial_out.size) <= 0) {
		UCSR0B &= ~( 1 << UDRIE0 );
		serial_tx_flag = 0;
	}
}

void serial_flush(void) {
	if(serial_tx_flag == 0 && (serial_out.size) > 0) {
		serial_tx_flag = 1;
		UCSR0B |= ( 1 << UDRIE0 );
	}
}

uint16_t serial_available(void) {
	return serial_in.size;
}


void serial_putchar(char c) {
	queue_push(&serial_out, c);
}

char serial_getchar(void) {
	return queue_pop(&serial_in);
}
