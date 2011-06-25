// Frequency needed by the delay function.
// The chip is factory set to run on internal
// oscillator at 1 MHz so the value is 1000000UL
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB = 0xFF; // PB0-PB4 output
	PORTB = 0x00; // Set all pins low
	
	// Turn LED on for 1 sec and then off for 1 sec forever
	for(;;)
	{
		_delay_ms(1000);
		PORTB++;
	}	
	return 0;
}

