#include "hardware.h"

#include <stdlib.h>

#include "queue.h"

queue_t *queue_create(void) {
	queue_t *queue = (queue_t*)malloc(sizeof(queue_t));
	if(queue==NULL) return NULL;
	queue->size = 0;
	queue->first = 0;
	queue->last = 0;
	return queue;
}

void queue_clear(queue_t *q) {
	q->size = 0;
	q->first = 0;
	q->last = 0;
}

void queue_free(queue_t *q) {
	free(q);
}


void queue_push(queue_t *q, uint8_t c) {
	while ((q->size) >= QUEUE_SIZE) ;
	q->size++;
	q->q[q->last] = c;
	q->last = (q->last + 1) % QUEUE_SIZE;
}

uint8_t queue_pop(queue_t *q) {
	while ((q->size) <= 0) ;
	q->size--;
	uint8_t c = q->q[q->first];
	q->first = (q->first + 1) % QUEUE_SIZE;	
	return c;
}

