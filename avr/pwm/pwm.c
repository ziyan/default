#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void serial_init(unsigned int bittimer)
{
	/* Set the baud rate */
	UBRR0H = (unsigned char) (bittimer >> 8);
	UBRR0L = (unsigned char) bittimer;
	/* set the framing to 8N1 */
	UCSR0C = (3 << UCSZ00);
	/* Engage! */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	return;
}

unsigned char serial_read(void)
{
	while( !(UCSR0A & (1 << RXC0)) )
		;
	return UDR0;
}

void serial_write(unsigned char c)
{
	while ( !(UCSR0A & (1 << UDRE0)) )
		;
	UDR0 = c;
}

void sleep(double sec)
{
	double maxms, last;
	int sleeps;

	sec *= 1000.0; /* turn seconds into milliseconds */
	maxms = 262144000 / F_CPU;
	sleeps = sec / maxms;
	last = sec - (sleeps * maxms);

	while (sleeps--)
		_delay_ms(maxms);
	_delay_ms(last);
	return;
}

#define SPEED 38400

int main(void) {
	//DDRB=0xFF;
	//PORTB=0x00;

	DDRD |= _BV(PD6);       // Set PD6(OC0A) Output
	TCCR0A = 0b10000011;    // Compare Match Low // 8 bit High Speed PWM
	TCCR0B = 0b00000101;    // clk/1024 125nsX1024=128us
	OCR0A = 128;            // Set Output Compare Register

	sei();
	
	/* let the preprocessor calculate this */
	serial_init( ( F_CPU / SPEED / 16 ) - 1);

	unsigned char c;
	while (1) {
		/* read a character and echo back the next one */
		//serial_write( serial_read() + 1);
		c = serial_read();
		serial_write(c);
		OCR0A = c;
	}
	return 0;
}

ISR(TIMER0_COMPA_vect)
{
}
