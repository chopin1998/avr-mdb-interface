#ifndef _MDB_BILLV_H
#define _MBD_BILLV_H


#define MDB_BILLV_POLL_STATUS_DEFECTIVE_MOTOR    0x01
#define MDB_BILLV_POLL_STATUS_SENSOR_PROBLEM     0x02
#define MDB_BILLV_POLL_STATUS_BUSY               0x03
#define MDB_BILLV_POLL_STATUS_ROM_ERROR          0x04
#define MDB_BILLV_POLL_STATUS_JAMMED             0x05
#define MDB_BILLV_POLL_STATUS_JUST_RESET         0x06
#define MDB_BILLV_POLL_STATUS_BILL_REMOVED       0x07
#define MDB_BILLV_POLL_STATUS_NO_CASHBOX         0x08
#define MDB_BILLV_POLL_STATUS_DISABLED           0x09
#define MDB_BILLV_POLL_STATUS_INVALID_ESCROW     0x0a
#define MDB_BILLV_POLL_STATUS_BILL_REJECTED      0x0b
#define MDB_BILLV_POLL_STATUS_STACKED_REMOVE     0x0c
#define MDB_BILLV_POLL_STATUS_INVALID_TYIES_BASE 0x40

#define MDB_BILLV_ACCEPT_STATUS_STACKED          0x00
#define MDB_BILLV_ACCEPT_STATUS_ESCROW           0x01
#define MDB_BILLV_ACCEPT_STATUS_RETURNED         0x02
#define MDB_BILLV_ACCEPT_STATUS_TO_RECYCLER      0x03
#define MDB_BILLV_ACCEPT_STATUS_DISABLED         0x04
#define MDB_BILLV_ACCEPT_STATUS_MANUAL_FILL      0x05
#define MDB_BILLV_ACCEPT_STATUS_MANUAL_DISPENSE  0x06
#define MDB_BILLV_ACCEPT_STATUS_TO_CASHBOX       0x07


typedef struct
{
    unsigned char poll_info[64];
} _billv_device_info;
_billv_device_info mdb_billv_dev_info;

typedef struct
{
    unsigned char   level;
    unsigned short  country;
    unsigned short  scaling_factor;
    unsigned char   decimal_place;
    unsigned short  stacker_capacity;
    unsigned short  security_level;
    unsigned char   escrow;
    unsigned char   type[16];  
} _mdb_setup_info;
_mdb_setup_info mdb_setup_info;



void mdb_billv_init(void);

void mdb_billv_handler(void);

unsigned char mdb_billv_parse_setupinfo(unsigned char *buf,
                                        unsigned char len);

void mdb_billv_enable(void);
void mdb_billv_disable(void);
void mdb_billv_escrow(unsigned char statck);
void mdb_billv_stacker_info(void);
void mdb_billv_info(void);

void mdb_billv_status(void);

#endif
