#include "common.h"
#include "mdb_uart.h"

void mdb_uart_init()
{
    UCSR1B = _BV(RXCIE1) | _BV(RXEN1) | _BV(TXEN1);
    UBRR1H = 0;
    UBRR1L = 95;                /* 9600bps @ 14745600Hz */

    UCSR1C |= _BV(UCSZ11) | _BV(UCSZ10); /* 9bit mode */
    UCSR1B |= _BV(UCSZ12);      /* 9bit mode */

    ////////////////
    ////////////////
    mdb_rx.in = 0;
    mdb_rx.out = 0;
    mdb_rx.count = 0;
}

void mdb_uart_send(unsigned short *buf, unsigned char length)
{   
    for (unsigned char i=0; i<length; i++)
    {
        mdb_uart_putc( buf[i] >> 8, buf[i]);
    }
}

void mdb_uart_flush()
{
    // mdb_uart_init();
    
    mdb_rx.in = 0;
    mdb_rx.out = 0;
    mdb_rx.count = 0;
}

void mdb_uart_putc(char mode, char data)
{
    loop_until_bit_is_set(UCSR1A, UDRE1);

    if (mode) {
        UCSR1B |= _BV(TXB81);
    } else {
        UCSR1B &= ~_BV(TXB81);
    }

    UDR1 = data;
}

/*
 * return -1, means no data in queue
 * otherwise, means a data
 */
signed short mdb_uart_deq()
{
    unsigned short rev;

    if (mdb_rx.count == 0) {
        rev = -1;              /* -1 means no data in queue */
    } else {
        rev = mdb_rx.queue[mdb_rx.out];
        mdb_rx.out = (mdb_rx.out + 1) % MDB_RX_MAX;
        mdb_rx.count--;
    }

    return rev;
}

ISR(USART1_RX_vect)
{
    static unsigned char mode;
    static unsigned short data;

    mode = (UCSR1B & _BV(RXB81)) >> RXB81;
    data = (mode << 8) | UDR1;

    if (mdb_rx.count >= MDB_RX_MAX) { /* queue full! */
        mdb_rx.queue[0] = data;
        mdb_rx.in = 1;
        mdb_rx.out = 0;
        mdb_rx.count = 1;
    } else {                    /* enqueue normally */
        mdb_rx.queue[mdb_rx.in] = data;
        mdb_rx.in = (mdb_rx.in + 1) % MDB_RX_MAX;
        mdb_rx.count++;
    }
}

