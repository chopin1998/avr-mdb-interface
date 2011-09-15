#include "common.h"

#include "mdb.h"
#include "mdb_uart.h"

#include "uart.h"


ISR(TIMER0_COMPA_vect)
{
    if (mdb_timer.down_count <= 1) {
        mdb_timer.shot = 1;
    }
    
    mdb_timer.down_count--;
}

/*
 * used as fixed freq timer
 * worked @ 200Hz, CTC mode
 */
void mdb_timer_init()
{
    TCCR0A = _BV(WGM01);        /* ctc mode */
    TCCR0B = _BV(CS02) | _BV(CS00); /* 1024 preClock */

    OCR0A = 72 - 1;                /* 200Hz */

    TIMSK0 = _BV(OCIE0A);       /* enable irq */
}

void mdb_timer_set(unsigned int val)
{
    mdb_timer.down_count = (val / 5) ? (val / 5) : 1;
    mdb_timer.shot = 0;
}

unsigned char mdb_timer_is_shot()
{
    if (mdb_timer.shot) {
        mdb_timer.shot = 0;
        return 1;
    } else {
        return 0;
    }
}

void mdb_init()
{
    //////////////// mdb utils
    // mdb_timer_init();

    //////////////// mdb interval structs
}

char mdb_tick(unsigned char *buf, unsigned char *length, unsigned int timeout)
{
    char rev=MDB_TICK_OK;
    signed short ch;
    unsigned char chksum = 0;
    unsigned int timeout_count=0;
    unsigned char len = 0;

    memset(buf, 0x00, *length);
    for (len=0;;)
    {
        cli();
        ch = mdb_uart_deq();
        sei();

        if (ch == -1) {
            timeout_count++;

            if (timeout_count > timeout) {
                rev = MDB_TICK_TIMEOUT;
                break;
            }
            _delay_us(1);
            
        } else {
            /* debug output */

            timeout_count = 0;
            buf[len] = ch;
            len++;
            
            if (ch & 0x100) {   /* last byte detected, checking sum */
                
                if (len == 1) {
                    rev = MDB_TICK_OK;
                    break;
                } else {
                    
                    if ((ch&0xff) != chksum) {
                        rev = MDB_TICK_CHKSUM_ERROR;
                        break;
                    } else {
                        len--;
                        buf[len] = '\x00';
                        rev = MDB_TICK_OK;
                        break;
                    }
                }
            } else {
                // if (len > MDB_BLOCK_LIMIT || len > *length) {
                if (len > MDB_BLOCK_LIMIT) {
                    rev = MDB_TICK_OVERLEN;
                    break;
                }
                chksum += ch;
            }
        }
    }

    *length = len;
    return rev;
}

void mdb_ack(void)
{
    mdb_uart_putc(0, MDB_RESP_ACK);
}

void mdb_debug_data(unsigned char *buf, unsigned char len)
{
    unsigned char tmp[16];

    uart_print("dump --> ");
    for(unsigned char i=0; i<len; i++)
    {
        sprintf(tmp, "0x%02x, ", buf[i]);
        uart_print(tmp);
    }
    uart_print("\n");
}

void mdb_send(unsigned char addr, unsigned char cmd,
              unsigned char *buf, unsigned char len)
{
    unsigned int i=0;
    unsigned char chksum;

    chksum = addr|cmd;
    
    if (len != 0) {
        mdb_uart_putc(1, addr|cmd);
        for (i=0; i<len; i++)
        {
            chksum += buf[i];
            mdb_uart_putc(0, buf[i]);
        }
        mdb_uart_putc(0, chksum);
    } else { // len == 0
        mdb_uart_putc(1, addr|cmd);
        mdb_uart_putc(0, chksum);
    }
    
}

void mdb_polling()
{
    static char count=0;
    
    mdb_billv_handler();
}

