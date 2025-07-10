/*============================================================================*/
/* @file exe_int_ll.h
 * @brief EXE Link Layer stack internal header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_INT_LL_H__
#define __EXE_INT_LL_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble_ll.h"

/* The number of elements in tx queue: order of two */
#define STK_TX_QUEUE_ELEM_NUM           4
#define STK_TX_QUEUE_ELEM_MASK          (STK_TX_QUEUE_ELEM_NUM - 1)

extern uint8_t ll_gbuf_xpkt_rx[];
extern uint8_t ll_gbuf_xpkt_tx[];

extern uint32_t exe_bletick_wakeup_point;
extern uint32_t conn_bletick_b4smp;
extern uint32_t exe_bletick_event_expect_time;
extern uint32_t exe_bletick_event_tolerance_time;

/* flow control management. */
extern int8_t exe_rx_len;

/* tx queue management. */
extern uint8_t stk_buf_tx_queue[];
extern volatile uint8_t stk_u8_tx_queue_wptr;
extern volatile uint8_t stk_u8_tx_queue_rptr;
extern uint8_t *exe_tx_pkt_hold;

/* security. */
extern bool ll_link_is_encrypted;

void ll_adv_init(void);
void ll_adv_set_ind_rsp(void);
void exe_ll_set_channel(uint8_t chn_idx);

uint8_t ll_pkt_rx(uint32_t duration);

int8_t ll_conn_process_fc(bool is_rxd_good_pkt);
bool     ll_conn_process_conn_ind(ble_adv_pkt_t *p_pkt_adv);
void     ll_conn_process_llcp_update(ble_llcp_pdu_t *p_llcp, uint8_t update_type);
uint32_t ll_conn_sleep_prologue(void);
void     ll_conn_sleep_epilogue(uint32_t tick_wakeup);

bool stk_tx_packet_in_queue(uint8_t *p_xpkt_tx);

bool     ll_llcp_dispatch_opcode(ble_llcp_pdu_t *p_llcp);
void     ll_llcp_try_terminate_connection(void);

#endif /* #ifndef __EXE_INT_LL_H__ */
