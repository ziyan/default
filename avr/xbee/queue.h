#ifndef AVR_ZIYAN_QUEUE_H
#define AVR_ZIYAN_QUEUE_H
#include "hardware.h"
#ifndef QUEUE_SIZE
#define QUEUE_SIZE 1024
#endif
#include <inttypes.h>

typedef struct {
	uint8_t q[QUEUE_SIZE];
	uint16_t first;
	uint16_t last;
	volatile uint16_t size;
} queue_t;

queue_t *queue_create(void);
void queue_clear(queue_t *q);
void queue_free(queue_t *q);
void queue_push(queue_t *q, uint8_t c);
uint8_t queue_pop(queue_t *q);


#endif
