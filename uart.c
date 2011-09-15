#include "common.h"
#include "uart.h"


void uart_init()
{
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    UBRR0H = 0;
    UBRR0L = 15;                /* 57600 @ 14745600hz */

    raw_queue.in = 0;
    raw_queue.out = 0;
    raw_queue.counter = 0;
}

void uart_print(unsigned char *buf)
{
    unsigned int len = strlen(buf);

    if (len > 0)
    {
        for (int i=0; i<len; i++)
        {
            // _putc(*(buf++));
            _putc(buf[i]);
        }
    }
}

void _putc(char c)
{
    if(c == '\n')
        _putc('\r');
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

signed char uart_dequeue()
{
    char rev;

    if(raw_queue.counter == 0)
    { // empty, no data...
        return -1;   
    }
    else
    { // dequeue normally
        rev = raw_queue.queue[raw_queue.out];
        raw_queue.out = (raw_queue.out + 1) % RawQueueSize;
        raw_queue.counter--;
        return rev;
    }
}

ISR(USART0_RX_vect)
{
    char recv;
    recv = UDR0;

    if (raw_queue.counter >= RawQueueSize)
    { // queue full !! extreme dangerous
        raw_queue.queue[0] = recv;
        raw_queue.in = 1;
        raw_queue.out = 0;
        raw_queue.counter = 1;
    }
    else
    { // enqueue normally
        raw_queue.queue[raw_queue.in] = recv;
        raw_queue.in = (raw_queue.in + 1) % RawQueueSize;
        raw_queue.counter++;
    }
}
