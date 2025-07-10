/*============================================================================*/
/* @file exe_int_host.h
 * @brief EXE Host stack internal header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_INT_HOST_H__
#define __EXE_INT_HOST_H__

#include <stdint.h>
#include <stdbool.h>
#include "exe_pkt.h"

typedef void (* smp_func_arg0_t)(void);
typedef void (* smp_func_arg1_t)(uint8_t *);
typedef void (* smp_func_arg2_t)(uint8_t *, bool);

extern bool smp_bool_sec_req_is_sent;
extern bool smp_bool_sec_req_is_required4ios;
/* SMP过程是否开始。*/
extern bool smp_bool_pair_req_is_rxed;

/* remove later since C51 dislikes function pointer. */
extern smp_func_arg1_t func_smp_init;
extern smp_func_arg1_t func_pair_init;
extern smp_func_arg1_t func_smp_enc;
extern smp_func_arg2_t func_smp_brx;

extern void smp_init(uint8_t *);
extern void smp_encrypt_tx_pkt(uint8_t *);
extern void smp_process_rx_pkt(uint8_t *, bool);

bool smp_is_pair_in_progress(void);

exe_data_pkt_t *att_dispatch_opcode(exe_data_pkt_t *req);
exe_data_pkt_t *l2cap_signaling_dispatch_opcode(exe_data_pkt_t *req);

const uint8_t *hid_get_report_map(void);
int hid_get_report_map_size(void);
void hid_notification_enable(uint8_t flag);
uint8_t hid_notification_is_enabled(void);

#endif /* #ifndef __EXE_INT_HOST_H__ */

