#ifndef _UART_H
#define _UART_H 1

#define RawQueueSize 64
typedef struct
{
    char queue[RawQueueSize];
    unsigned char in;
    unsigned char out;
    unsigned char counter;
} _raw_queue;
static volatile _raw_queue raw_queue;


void _putc(char);
void uart_print(unsigned char *);
void uart_init(void);
signed char uart_dequeue(void);


#endif
