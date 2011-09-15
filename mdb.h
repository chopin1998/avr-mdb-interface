#ifndef _MDB_H
#define _MDB_H

/* MDB protocol block limit */
#define MDB_BLOCK_LIMIT 36

/* MDB protocol bytes */
#define MDB_RESP_ACK    0x00    /* acknowledgemnt/ chksum correct */
#define MDB_RESP_RET    0xaa    /* retransmit the last sent data */
#define MDB_RESP_NAK    0xff    /* use silent 5ms instead */

/* MDB protocol address assignment  */
#define MDB_ADDR_VMC        0x00 /* reserved for vmc-self */
#define MDB_ADDR_CHANGER    0x08
#define MDB_ADDR_CASHLESS_1 0x10 /* cashless device #1 */
#define MDB_ADDR_COM_GW     0x18
#define MDB_ADDR_DISP       0x20
#define MDB_ADDR_EMS        0x28 /* energy management system */
#define MDB_ADDR_BILLV      0x30 /* bill validator */
// #define MDB_ADDR_RESERVED   0x38
#define MDB_ADDR_USD_1      0x40 /* universal satellite device #1 */
#define MDB_ADDR_USD_2      0x48
#define MDB_ADDR_USD_3      0x50
#define MDB_ADDR_COINH_1    0x58 /* coin hopper or tube dispenser 1 */
#define MDB_ADDR_CASHLESS_2 0x60 /* cashless device #2 */
#define MDB_ADDR_COINH_2    0x68 /* coin hopper or tube dispenser 2 */
// a MDB reserved address holl 
#define MDB_ADDR_EXPERI_1   0xe0 /* experimental peripheral #1 */
#define MDB_ADDR_EXPERI_2   0xe8 /* experimental peripheral #2 */
#define MDB_ADDR_VMSP_1     0xf0 /* vending machine specific peripheral #1 */
#define MDB_ADDR_VMSP_2     0xf8 /* vending machine specific peripheral #2 */

/* MDB protocol, Bill Validators standard command */
#define MDB_CMD_BILLV_RESET    0x00
#define MDB_CMD_BILLV_SETUP    0x01 
#define MDB_CMD_BILLV_SECUR    0x02
#define MDB_CMD_BILLV_POLL     0x03
#define MDB_CMD_BILLV_TYPE     0x04
#define MDB_CMD_BILLV_ESCROW   0x05
#define MDB_CMD_BILLV_STACKER  0x06
#define MDB_CMD_BILLV_EXPCMD   0x07



/*
 * MDB state machine
 */
enum {SM_IDLE,};

typedef struct
{
    unsigned char state;
} _mdb_sm;
volatile _mdb_sm mdb_sm;

typedef struct
{
    unsigned int down_count;
    unsigned char shot;

} _mdb_timer;
volatile _mdb_timer mdb_timer;


void mdb_timer_set(unsigned int);
unsigned char mdb_timer_is_shot(void);

void mdb_init(void);

enum {MDB_TICK_OK, MDB_TICK_TIMEOUT, MDB_TICK_CHKSUM_ERROR, MDB_TICK_OVERLEN};
char mdb_tick(unsigned char *buf, unsigned char *length, unsigned int timeout);


void mdb_debug_data(unsigned char *buf, unsigned char len);
void mdb_ack(void);
void mdb_send(unsigned char addr, unsigned char cmd,
              unsigned char *buf, unsigned char len);

void mdb_polling(void);

#endif
