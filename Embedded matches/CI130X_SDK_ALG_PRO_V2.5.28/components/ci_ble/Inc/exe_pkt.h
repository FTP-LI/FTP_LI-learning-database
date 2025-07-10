/*============================================================================*/
/* @file exe_pkt.h
 * @brief  The packet format definitions at Extreme layer.
 * @author luwei
 * @date 2020/02
 */

#ifndef __EXE_PKT_H__
#define __EXE_PKT_H__

#include "ble_ll.h"
#include "ble_l2cap.h"
#include "ble_att.h"
#include "ble_smp.h"

__PACKED struct _exe_adv_pkt_t {
  ///tx: the length of whole BLE packet, including header, CRC fileds.
  ///rx: the timestamp in tick on sync.
  uint32_t xpkt_len;            //08
  ble_adv_pkt_t ble;
  uint8_t crc[3];
} __PACKED_GCC;
typedef struct _exe_adv_pkt_t exe_adv_pkt_t;

__PACKED struct _exe_llcp_pkt_t {
  uint32_t xpkt_len;             //08
  ble_data_header_t   header;   //0c
  ble_llcp_pdu_t      llcp;     //0e
  uint8_t crc[3];
} __PACKED_GCC;
typedef struct _exe_llcp_pkt_t exe_llcp_pkt_t;

__PACKED struct _exe_data_pkt_t {
  uint32_t xpkt_len;             //08
  ble_data_header_t   header;   //0c
  ble_l2cap_header_t  l2cap;    //0e
  __PACKED union {
    ble_att_pdu_t     att;      //12
    ble_smp_pdu_t     smp;
    ble_l2cap_signaling_pdu_t signal;
  } pdu;
  uint8_t crc[3];
} __PACKED_GCC;
typedef struct _exe_data_pkt_t exe_data_pkt_t;

#endif /* #ifndef __EXE_PKT_H__ */

