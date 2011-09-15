#ifndef _MDB_UART_H
#define _MDB_UART_H 1

#define MDB_RX_MAX 96
typedef struct
{
    unsigned short queue[MDB_RX_MAX];
    unsigned char in;
    unsigned char out;
    unsigned char count;

} _mdb_queue;
volatile _mdb_queue mdb_rx;

#define IN_PACKET    1
#define OUT_PACKET   0

void mdb_uart_init(void);
void mdb_uart_putc(char mode, char data);
void mdb_uart_flush(void);
void mdb_uart_send(unsigned short *buf, unsigned char length);
signed short mdb_uart_deq(void);

#endif
