#include "common.h"

#include "cmdrecv.h"
#include "uart.h"

#include "mdb.h"
#include "mdb_billv.h"

void cmdrecv_uart_tick()
{
    static char packet_state = OUT_PACKET;
    static char cmd_buf[BUFFMAX];
    static int cmd_index = 0;

    signed char recv;

    for (;;)
    {
        
        cli();
        recv = uart_dequeue();
        sei();

        if (recv == -1)
        {
            return;
        }
        else
        {
            if (packet_state == OUT_PACKET)
            {
                if (recv == STX)
                {
                    packet_state = IN_PACKET;

                    memset(cmd_buf, 0x0, BUFFMAX);
                    cmd_index = 0;
                }
                else
                {
                    // ignore
                }
            }
            else // packet_state == IN_PACKET
            {
                if (recv == ETX)
                {
                    packet_state = OUT_PACKET;
                    cmd_buf[cmd_index] = 0x0; /* padding a terminal char */

                    cmdrecv_process(cmd_buf);
                    memset(cmd_buf, 0x0, BUFFMAX);
                    cmd_index = 0;
                    return;
                }
                else
                {
                    cmd_buf[cmd_index] = recv;
                    cmd_index++;
                    if (cmd_index >= BUFFMAX)
                    { // a huge packet
                        packet_state = OUT_PACKET;
                        memset(cmd_buf, 0x0, BUFFMAX);
                        cmd_index = 0;
                        return;
                    }
                }
            }
        }
    
    }
}


void cmdrecv_p_list_clear(p_list *head)
{
    p_list *curr, *tmp;

    curr = head;
    while (curr)
    {
        tmp = curr->next;
        free(curr);
        curr = tmp;
    }
}

void cmdrecv_process(char *p_buf)
{
    // printf("process: %s, length: %d\n", p_buf, strlen(p_buf));
    
    p_list *para_head, *curr;
    char *p;
    unsigned int p_counter = 0;
    unsigned int p1, p2, p3;
    int rev;
    para_head = (p_list *)malloc(sizeof(p_list));

    unsigned char buf[32];

    /* splitting */
    p = strtok(p_buf, ",");
    curr = para_head;
    while (p)
    {
        p_counter++;
        
        curr->para = p;
        curr->next = (p_list *)malloc(sizeof(p_list));
        curr = curr->next;

        p = strtok(NULL, ",");
    }
    curr->next = NULL;
    
    /* processing */
    // printf("number of p: %d\n", p_counter);

    if (!strcmp(para_head->para, "billv"))
    {
        if (!strcmp(para_head->para, "poll"))
        {
            // uart_print("poll\n");
        }
        else if (!strcmp(para_head->next->para, "enable"))
        {
            uart_print("billv enable\n");
            mdb_billv_enable();
        }
        else if (!strcmp(para_head->next->para, "disable"))
        {
            uart_print("billv disable\n");
            mdb_billv_disable();
        }
        else if (!strcmp(para_head->next->para, "escrow"))
        {
            if (!strcmp(para_head->next->next->para, "in"))
            {
                mdb_billv_escrow(1);
            }
            else
            {
                mdb_billv_escrow(0);
            }
        }
        else if (!strcmp(para_head->next->para, "stacker"))
        {
            mdb_billv_stacker_info();
        }
        else if (!strcmp(para_head->next->para, "info"))
        {
            mdb_billv_info();
        }
    }
    else
    {
        uart_print("unimplement function\n");
    }
    
    cmdrecv_p_list_clear(para_head);
}
