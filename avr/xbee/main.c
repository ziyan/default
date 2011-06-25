#include "hardware.h"
#include "serial.h"
#include "xbee.h"
#include <util/delay.h>
#include <avr/io.h>

int main(void) {
	DDRB = 0xFF;
	PORTB = 0;
	serial_init();
	uint16_t addr = 0xFFFEU;
	uint64_t addr64 = 0x0000FFFFUL;
	int i;
	for(;;) {
		addr = xbee_send(1, addr64, addr, 0, "Hello World!\r\n", 14);
		addr64 = 0;
		xbee_data_t *data = xbee_receive_data();
		for(i=0;i<data->length;++i) {
			PORTB = data->data[i];
			_delay_ms(500);
		}
		xbee_free_data(data);
	}
	return 0;
}

