#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void) {
	DDRB=0xFF;
	PORTB=0x00;

	TCCR1A = 0xA3;
	TCCR1B = 0x05;
	OCR1A = 255;
	TIMSK1 = _BV(OCIE1A);
	sei();
	while(1) { }
	return 0;
}

ISR(TIMER1_COMPA_vect)
{
	PORTB = ~PORTB;
}
