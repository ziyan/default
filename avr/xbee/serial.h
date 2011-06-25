#ifndef AVR_ZIYAN_SERIAL_H
#define AVR_ZIYAN_SERIAL_H
#include "hardware.h"

#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE 9600
#endif

#include <inttypes.h>

#include "queue.h"

queue_t serial_in, serial_out;

void serial_init(void);
uint16_t serial_available(void);
char serial_getchar(void);
void serial_putchar(char c);
void serial_flush(void);

#endif
