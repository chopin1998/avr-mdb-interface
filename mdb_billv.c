#include "common.h"
#include "mdb.h"
#include "mdb_billv.h"

#include "uart.h"
#include "utils.h"

void mdb_billv_handler()
{
    unsigned char buf[MDB_BLOCK_LIMIT+1];
    unsigned char len=MDB_BLOCK_LIMIT+1;
    char rev, ch, bill_status, bill_type;
    char tmp[32];

    static unsigned char last_status[16];
    static unsigned char last_len;
    unsigned char same_to_last;
    
    mdb_uart_flush();

    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_POLL, NULL, 0);
    rev = mdb_tick(buf, &len, 5000);

    if (rev == MDB_TICK_OK) {
        same_to_last = 1;
        if (len == last_len) {
            for (unsigned char i=0; i<len; i++)
            {
                if (buf[i] != last_status[i]) {
                    same_to_last = 0;
                    break;
                }
            }
        } else {
            same_to_last = 0;
        }

        if (!same_to_last) {
            mdb_debug_data(buf, len);

            last_len = len;
            for (unsigned char i=0; i<len; i++)
            {
                last_status[i] = buf[i];
            }
        } else {
            
        }

        if (len == 1 && buf[0] == MDB_RESP_ACK) {
            // no news is good news...
            if (!same_to_last)    uart_print("<ack,>\n");
        } else {
            mdb_ack();

            if (!same_to_last) {
                uart_print("<");
            }
            
            for (unsigned char i=0; i<len; i++)
            {
                ch = buf[i];
                
                if (ch & 0x80) { /* got a bill */
                    bill_status = (ch & 0x70) >> 4;
                    bill_type = mdb_setup_info.type[ch & 0x0f];

                    sprintf(tmp, "b_status:0x%02x,$:%d,", bill_status, bill_type);
                    uart_print(tmp);
                } else {        /* a status byte */
                    switch (ch)
                    {
                    case MDB_BILLV_POLL_STATUS_DEFECTIVE_MOTOR:
                        if (!same_to_last)    uart_print("billv_motor_error,");
                        break;
                    case MDB_BILLV_POLL_STATUS_SENSOR_PROBLEM:
                        if (!same_to_last)    uart_print("billv_sensor_error,");
                        break;
                    case MDB_BILLV_POLL_STATUS_BUSY:
                        if (!same_to_last)    uart_print("billv_busy,");
                        break;
                    case MDB_BILLV_POLL_STATUS_ROM_ERROR:
                        if (!same_to_last)    uart_print("billv_rom_error,");
                        break;
                    case MDB_BILLV_POLL_STATUS_JAMMED:
                        if (!same_to_last)    uart_print("billv_jam,");
                        break;
                    case MDB_BILLV_POLL_STATUS_JUST_RESET:
                        if (!same_to_last)    uart_print("billv_justreset,");
                        mdb_billv_init();
                        break;
                    case MDB_BILLV_POLL_STATUS_BILL_REMOVED:
                        if (!same_to_last)    uart_print("billv_bill_removed,");
                        break;
                    case MDB_BILLV_POLL_STATUS_NO_CASHBOX:
                        if (!same_to_last)    uart_print("billv_no_cashbox,");
                        break;
                    case MDB_BILLV_POLL_STATUS_DISABLED:
                        if (!same_to_last)    uart_print("billv_disabled,");
                        break;
                    case MDB_BILLV_POLL_STATUS_INVALID_ESCROW:
                        if (!same_to_last)    uart_print("billv_invalid_escrow,");
                        break;
                    case MDB_BILLV_POLL_STATUS_BILL_REJECTED:
                        if (!same_to_last)    uart_print("billv_rejected,");
                        break;
                    case MDB_BILLV_POLL_STATUS_STACKED_REMOVE:
                        if (!same_to_last)    uart_print("billv_stacked_remove,");
                        break;
                    default:
                        sprintf(tmp, "0x%02x,", tmp[i]);
                        uart_print(tmp);
                        break;
                    }
                }
            }

            if (!same_to_last) {
                uart_print(">\n");
            }
        }
        
    } else {
        mdb_billv_init();
    }

    // printf("\n");
}

void mdb_billv_enable(void)
{
    unsigned char buf[16];
    unsigned char len;
    unsigned char rev;
    
    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_RESET, NULL, 0);
    // mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_RESET, "\xff\xff\xff\xff", 4);
    rev = mdb_tick(buf, &len, 5000);
}

void mdb_billv_disable(void)
{
    unsigned char buf[16];
    unsigned char len;
    unsigned char rev;
    
    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_TYPE, "\x00\x00\x00\x00", 4);
    rev = mdb_tick(buf, &len, 5000);
}

void mdb_billv_escrow(unsigned char stack)
{
    unsigned char buf[16];
    unsigned char len;
    unsigned char rev;

    if (stack) {
        mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_ESCROW, "\x0f", 1);
    } else {
        mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_ESCROW, "\x00", 1);
    }
    rev = mdb_tick(buf, &len, 50000);
}

void mdb_billv_init()
{
    unsigned char buf[MDB_BLOCK_LIMIT+1];
    unsigned char len=MDB_BLOCK_LIMIT+1;
    char rev;

    // printf("reset...");
    mdb_uart_flush();
    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_RESET, NULL, 0);
    rev = mdb_tick(buf, &len, 5000);
    if (rev == MDB_TICK_OK && buf[0] == MDB_RESP_ACK) {
        // printf("done\n");
    } else {
        // printf("no ack\n");
        return;
    }
    
    // printf("polling..");
    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_POLL, NULL, 0);
    rev = mdb_tick(buf, &len, 5000);
    if (buf[0] != MDB_RESP_ACK) {
        mdb_ack();
    }
    // printf("tick rev: %d, len: %d\n", rev, len);
    // mdb_debug_data(buf, len);

    // printf("setup info..\n");
    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_SETUP, NULL, 0);
    rev = mdb_tick(buf, &len, 5000);
    // printf("tick rev: %d, len: %d\n", rev, len);
    mdb_billv_parse_setupinfo(buf, len);
    mdb_ack();
    // mdb_debug_data(buf, len);

    // printf("setting securit..\n");
    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_SECUR, "\xff\xff", 2);
    rev = mdb_tick(buf, &len, 5000);
    // printf("tick rev: %d, len: %d\n", rev, len);
    // mdb_debug_data(buf, len);

    // printf("setting type..\n");
    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_TYPE, "\xff\xff\xff\xff", 4);
    rev = mdb_tick(buf, &len, 5000);
    // printf("tick rev: %d, len: %d\n", rev, len);
    // mdb_debug_data(buf, len);

    // printf("\n");
}

unsigned char mdb_billv_parse_setupinfo(unsigned char *buf, unsigned char len)
{
    unsigned char rev = 0;

    if (len != 27) {            /* the magic no defined by MDB spec */
        // printf("setup info len failed\n");
        rev = -1;
    } else {
        memcpy(&mdb_setup_info, buf, len);
    }

    return rev;
}

void mdb_billv_stacker_info()
{
    unsigned char buf[16], tmp[16];
    unsigned char len=0;
    char rev;

    mdb_send(MDB_ADDR_BILLV, MDB_CMD_BILLV_STACKER, NULL, 0);
    rev = mdb_tick(buf, &len, 5000);
    // printf("tick rev: %d, len: %d\n", rev, len);
    mdb_ack();
    // mdb_debug_data(buf, len);

    sprintf(tmp, "<%d%d,>\n", buf[0], buf[1]);
    uart_print(tmp);
}

void mdb_billv_info()
{
    unsigned char tmp[32];

    uart_print("<");

    sprintf(tmp, "level:%u,", mdb_setup_info.level);
    uart_print(tmp);

    sprintf(tmp, "country:0x%x,", conv_order_short(mdb_setup_info.country));
    uart_print(tmp);

    sprintf(tmp, "scaling_factor:%u,", conv_order_short(mdb_setup_info.scaling_factor));
    uart_print(tmp);

    sprintf(tmp, "decimal_place:%u,", mdb_setup_info.decimal_place);
    uart_print(tmp);

    sprintf(tmp, "stacker_capacity:%u,",
            conv_order_short(mdb_setup_info.stacker_capacity));
    uart_print(tmp);
    
    sprintf(tmp, "security_level:0x%x,",
            conv_order_short(mdb_setup_info.security_level));
    uart_print(tmp);

    sprintf(tmp, "escrow:%u,", mdb_setup_info.escrow);
    uart_print(tmp);

    uart_print("support:");
    for (unsigned char i=0; i<16; i++)
    {
        if (mdb_setup_info.type[i] != 0)
        {
            sprintf(tmp, "%u|", mdb_setup_info.type[i]);
            uart_print(tmp);
        }
    }

    
    uart_print(">\n");
}
