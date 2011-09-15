
#ifndef _CMDRECV_H
#define _CMDRECV_H

#define STX   '<'
#define ETX   '>'
#define IN_PACKET    1
#define OUT_PACKET   0

#define BUFFMAX      64


struct _p_list
{
    char *para;
    struct _p_list *next;
};
typedef struct _p_list p_list;


void cmdrecv_p_list_clear(p_list *head);
void cmdrecv_process(char *p_buf);
void cmdrecv_uart_tick(void);


#endif
