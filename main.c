/*
 * Author: chopin1998@gmail.com
 */


#include "common.h"
#include "mdb.h"

void init_uart(void);
void init_io(void);
void killdog(void);

uint8_t mcusr_mirror __attribute__((section (".noinit")));
void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init3")));


int main()
{
    unsigned int i=0;
    
    killdog();

    init_io();
    uart_init();
    mdb_uart_init();
    
    mdb_init();
    mdb_billv_init();

    sei();

    for(;;)
    {
        // mdb_timer_set(500);
        // while (!mdb_timer_is_shot());

        cmdrecv_uart_tick();
        
        mdb_polling();
        
        _delay_ms(250);
    }
    
    cli();
    wdt_enable(WDTO_500MS);     /* should not run here */
    for(;;);
}

void init_io()
{
    /*
     * PORTA
     *     all pins used as ADC input channel
     */
    DDRA = 0x00;
    PORTA = 0x00;

    /*
     *  PORTB
     */
    DDRB = 0xff;
    PORTB = 0xff;

    /*
     * PORTC
     */
    DDRC = 0x00;
    PORTC = 0xff;

    /*
     * PORTD
     */    
    DDRD = 0xfa;
    PORTD = 0xff;
}

void killdog(void)
{
    mcusr_mirror = MCUSR;
    MCUSR = 0;
    wdt_disable();
}
