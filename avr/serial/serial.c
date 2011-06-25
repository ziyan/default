#define F_CPU 8000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

typedef struct queue {
	char* q;
	unsigned int first;
	unsigned int last;
	volatile unsigned int size;
	unsigned int max;
} queue_t;

static queue_t *queue_create(unsigned int size) {
	queue_t *queue = (queue_t*)malloc(sizeof(queue_t));
	if(queue==NULL) return NULL;
	queue->size = 0;
	queue->max = size;
	queue->q = (char*)malloc(sizeof(char)*size);
	queue->first = 0;
	queue->last = 0;
	return queue;
}

static int queue_push(queue_t *q, char c) {
	while ((q->size) >= (q->max)) ;
	q->size++;
	q->q[q->last] = c;
	q->last = (q->last + 1) % q->max;
	return 0;
}

static char queue_pop(queue_t *q) {
	while ((q->size) <= 0) ;
	q->size--;
	char c = q->q[q->first];
	q->first = (q->first + 1) % q->max;	
	return c;
}

static queue_t *serial_in, *serial_out;

static void serial_init(unsigned int speed)
{
	unsigned int bittimer = ( F_CPU / speed / 16 ) - 1;
	/* Set the baud rate */
	UBRR0H = (unsigned char) (bittimer >> 8);
	UBRR0L = (unsigned char) bittimer;
	/* set the framing to 8N1 */
	UCSR0C = (3 << UCSZ00);
	/* Engage! */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	return;
}

ISR(USART_RX_vect)
{
	queue_push(serial_in, UDR0);
}

ISR(USART_UDRE_vect)
{
	if((serial_out->size) > 0)
		UDR0 = queue_pop(serial_out);
	if((serial_out->size) <= 0)
		UCSR0B &= ~( 1 << UDRIE0 );
}


static int serial_stdin(FILE *stream)
{
	return queue_pop(serial_in);
}
static int serial_stdout(char c, FILE *stream)
{
	queue_push(serial_out, c);
	if (c=='\n')
		UCSR0B |= ( 1 << UDRIE0 );
	return 0;
}
static FILE serial_stdio = FDEV_SETUP_STREAM(serial_stdout, serial_stdin, _FDEV_SETUP_RW);



int main (void)
{
	DDRB = 0xFF;
	PORTB = 0x00;

	serial_in = queue_create(256);
	serial_out = queue_create(256);

	serial_init(38400);

	stdin = stderr = stdout = &serial_stdio;

	printf("Hello World!\n");

	sei();
	
	for(;;) {
		cli();
		printf("Type in something:\n");
		sei();
		char str[100];
		scanf("%s",str);
		cli();
		printf("You typed %s\n", str);
		sei();
	}
	return 0;
}

