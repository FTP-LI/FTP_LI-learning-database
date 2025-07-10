/*============================================================================*/
/* @file ble_ll.h
 * @spec   Low Engergy Control volume, Vol 6.
 * @author luwei
 * @date 2020/02
 */

#ifndef __BLE_LL_H__
#define __BLE_LL_H__

#include <stdint.h>
#include <stdbool.h>
#include "exe_hal_tc.h"

/* Part B, Table 2.17:  Data channel PDU Header field */
#define LLID_LL_DATA_PDU_CONTINUE       0x1
#define LLID_LL_DATA_PDU_EMPTY          0x1
#define LLID_LL_DATA_PDU_START          0x2
#define LLID_LL_DATA_PDU_FULL           0x2
#define LLID_LL_CONTROL_PDU             0x3

/* Vol 2, Part E, 7.8.5  LE Set  Advertising Parameters command
   The default is 0x07 (all three channels enabled). */
#define ADV_CH_MAP_37			        (0x1 << 0)
#define ADV_CH_MAP_38			        (0x1 << 1)
#define ADV_CH_MAP_39			        (0x1 << 2)
#define ADV_CH_MAP_ALL			        (0x7)

/* Part B, Table 2.3:  Advertising physical channel PDU header’s PDU Type field encoding */
typedef enum {
  ADV_IND = 0x0,
  ADV_DIRECT_IND = 0x1,
  ADV_NONCONN_IND = 0x2,
  SCAN_REQ = 0x3,
  AUX_SCAN_REQ = 0x3,
  SCAN_RSP = 0x4,
  CONNECT_IND = 0x5,
  AUX_CONNECT_REQ = 0x5,
  ADV_SCAN_IND = 0x6,
  ADV_EXT_IND = 0x7,
  AUX_ADV_IND = 0x7,
  AUX_SCAN_RSP = 0x7,
  AUX_SYNC_IND = 0x7,
  AUX_CHAIN_IND = 0x7,
  AUX_CONNECT_RSP = 0x8,
} ll_adv_pdu_type_t;

/* Part B, Table 2.18:   LL Control PDU Opcodes */
typedef enum {
  LLCP_CONNECTION_UPDATE_IND, //0x00
  LLCP_CHANNEL_MAP_IND,
  LLCP_TERMINATE_IND,
  LLCP_ENC_REQ,
  LLCP_ENC_RSP,
  LLCP_START_ENC_REQ,
  LLCP_START_ENC_RSP,
  LLCP_UNKNOWN_RSP,
  LLCP_FEATURE_REQ,           //0x08
  LLCP_FEATURE_RSP,
  LLCP_PAUSE_ENC_REQ,
  LLCP_PAUSE_ENC_RSP,
  LLCP_VERSION_IND,
  LLCP_REJECT_IND,
  LLCP_SLAVE_FEATURE_REQ,
  LLCP_CONNECTION_PARAM_REQ,
  LLCP_CONNECTION_PARAM_RSP,  //0x10
  LLCP_REJECT_EXT_IND,
  LLCP_PING_REQ,
  LLCP_PING_RSP,
  /* 4.2, 5.0 */
  LLCP_LENGTH_REQ, 
  LLCP_LENGTH_RSP, 
  LLCP_PHY_REQ,
  LLCP_PHY_RSP,
  LLCP_PHY_UPDATE_IND,        //0x18
  LLCP_MIN_USED_CHANNELS_IND,
  /* 5.1 */
  LLCP_CTE_REQ,               //0x1A
  LLCP_CTE_RSP,
  LLCP_PERIODIC_SYNC_IND,
  LLCP_CLOCK_ACCURACY_REQ,
  LLCP_CLOCK_ACCURACY_RSP,
} ll_control_pdu_opcode_t;

/* Part B, 2.3   ADVERTISING PHYSICAL CHANNEL PDU */
__PACKED struct _ble_adv_header_t {
  ///Advertising physical channel PDU header’s PDU Type, @ref ll_adv_pdu_type_t
  uint8_t Type  :4; //0c
  uint8_t RFU   :1;
  uint8_t ChSel :1;
  uint8_t TxAdd :1;
  uint8_t RxAdd :1;
  ///Indicates the length of the payload in octets.
  uint8_t Length;   //0d
} __PACKED_GCC;
typedef struct _ble_adv_header_t ble_adv_header_t;

/* Part B, 2.4   DATA PHYSICAL CHANNEL PDU */
__PACKED struct _ble_data_header_t {
  ///The LLID indicates whether the packet is an LL Data PDU or an LL Control PDU. 
  uint8_t llid :2;  //0c
  ///Next Expected Sequence Number.
  uint8_t nesn :1;
  ///Sequence Number.
  uint8_t sn   :1;
  ///More Data.
  uint8_t md   :1;
  ///CTEInfo Present.
  uint8_t cp   :1; /* 5.1 */
  uint8_t RFU  :2;
  ///Indicates the size, in octets, of the Payload and MIC, if included.
  uint8_t Length;   //0d
  ///Indicates the  type and length of the Constant Tone Extension.
  //uint8_t CTEInfo; /* 5.1 */
} __PACKED_GCC;
typedef struct _ble_data_header_t ble_data_header_t;

/* Part B, 2.3.1.1  ADV_IND */
__PACKED struct _ble_adv_ind_t {
  uint8_t AdvA[6];
  uint8_t AdvData[31];
} __PACKED_GCC;
typedef struct _ble_adv_ind_t ble_adv_ind_t;

/* Part B, 2.3.1.2  ADV_DIRECT_IND */
__PACKED struct _ble_adv_direct_ind_t {
  uint8_t AdvA[6];
  uint8_t TargetA[6];
} __PACKED_GCC;
typedef struct _ble_adv_direct_ind_t ble_adv_direct_ind_t;

/* Part B, 2.3.1.3  ADV_NONCONN_IND */
__PACKED struct _ble_adv_nonconn_ind_t {
  uint8_t AdvA[6];
  uint8_t AdvData[31];
} __PACKED_GCC;
typedef struct _ble_adv_nonconn_ind_t ble_adv_nonconn_ind_t;

/* Part B, 2.3.1.4  ADV_SCAN_IND */
__PACKED struct _ble_adv_scan_ind_t {
  uint8_t AdvA[6];
  uint8_t AdvData[31];
} __PACKED_GCC;
typedef struct _ble_adv_scan_ind_t ble_adv_scan_ind_t;

/* Part B, 2.3.2.1  SCAN_REQ and AUX_SCAN_REQ */
__PACKED struct _ble_adv_scan_req_t {
  uint8_t ScanA[6]; //0e
  uint8_t AdvA[6];  //14
} __PACKED_GCC;
typedef struct _ble_adv_scan_req_t ble_adv_scan_req_t;

/* Part B, 2.3.2.2  SCAN_RSP */
__PACKED struct _ble_adv_scan_rsp_t {
  uint8_t AdvA[6];
  uint8_t ScanRspData[31];
} __PACKED_GCC;
typedef struct _ble_adv_scan_rsp_t ble_adv_scan_rsp_t;

/* Part B, 2.3.3.1  CONNECT_IND  and AUX_CONNECT_REQ */
__PACKED struct _ble_adv_connect_ind_t {
  uint8_t InitA[6];        //0e
  uint8_t AdvA[6];         //14
  uint32_t AA;             //1a
  uint8_t CRCInit[3]; //@4   1e
  uint8_t WinSize;    //@7   22
  uint16_t WinOffset; //@8   23
  uint16_t Interval;  //@10  25
  uint16_t Latency;   //@12  27
  uint16_t Timeout;   //@14  29
  uint8_t ChM[5];     //@16  2a
  uint8_t Hop :5;     //@21  2f
  uint8_t SCA :3;
} __PACKED_GCC;
typedef struct _ble_adv_connect_ind_t ble_adv_connect_ind_t;

/* 2.3   ADVERTISING PHYSICAL CHANNEL PDU */
__PACKED struct _ble_adv_pkt_t {
  ble_adv_header_t header;
  __PACKED union {
    ble_adv_ind_t         adv_ind;
    ble_adv_direct_ind_t  direct_ind;
    ble_adv_nonconn_ind_t nonconn_ind;
    ble_adv_scan_ind_t    scan_ind;
    ///Scan request by master.
    ble_adv_scan_req_t    scan_req;
    ///Scan response by slave.
    ble_adv_scan_rsp_t    scan_rsp;
    ///Indicates that master initiates connection.
    ble_adv_connect_ind_t connect_ind;
  } payload;
} __PACKED_GCC;
typedef struct _ble_adv_pkt_t ble_adv_pkt_t;

/* 2.4.2.1  LLCP_CONNECTION_UPDATE_IND */
__PACKED struct _ble_ll_connection_update_ind_t {
  uint8_t  WinSize;   //0f
  uint16_t WinOffset; //10
  uint16_t Interval;  //12
  uint16_t Latency;   //14
  uint16_t Timeout;   //16
  uint16_t Instant;   //18
} __PACKED_GCC;
typedef struct _ble_ll_connection_update_ind_t ble_ll_connection_update_ind_t;

__PACKED struct _ble_ll_channel_map_ind_t {
  uint8_t  ChM[5];  //0f
  uint16_t Instant; //14
} __PACKED_GCC;
typedef struct _ble_ll_channel_map_ind_t ble_ll_channel_map_ind_t;

__PACKED struct _ble_ll_terminate_ind_t {
  uint8_t ErrorCode;
} __PACKED_GCC;
typedef struct _ble_ll_terminate_ind_t ble_ll_terminate_ind_t;

__PACKED struct _ble_ll_enc_req_t {
  uint8_t Rand[8];
  uint16_t EDIV;
  uint8_t SKDm[8];
  uint8_t IVm[4];
} __PACKED_GCC;
typedef struct _ble_ll_enc_req_t ble_ll_enc_req_t;

__PACKED struct _ble_ll_enc_rsp_t {
  uint8_t SKDs[8];
  uint8_t IVs[4];
} __PACKED_GCC;
typedef struct _ble_ll_enc_rsp_t ble_ll_enc_rsp_t;

__PACKED struct _ble_ll_feature_req_t {
  uint8_t FeatureSet[8];
} __PACKED_GCC;
typedef struct _ble_ll_feature_req_t ble_ll_feature_req_t;

__PACKED struct _ble_ll_feature_rsp_t {
  uint8_t FeatureSet[8];
} __PACKED_GCC;
typedef struct _ble_ll_feature_rsp_t ble_ll_feature_rsp_t;

__PACKED struct _ble_ll_version_ind_t {
  uint8_t VersNr;
  uint16_t CompId;
  uint16_t SubVersNr;
} __PACKED_GCC;
typedef struct _ble_ll_version_ind_t ble_ll_version_ind_t;

/* 2.4.2  LL Control PDU */
__PACKED struct _ble_llcp_pdu_t {
  uint8_t Opcode;
  __PACKED union {
  	//uint8_t                        data[23-1];
    ble_ll_connection_update_ind_t connection_update_ind;
    ble_ll_channel_map_ind_t       channel_map_ind;
    ble_ll_terminate_ind_t         terminate_ind;
    ble_ll_enc_req_t               enc_req;
    ble_ll_enc_rsp_t               enc_rsp;
    ble_ll_feature_req_t           feature_req;
    ble_ll_feature_rsp_t           feature_rsp;
    ble_ll_version_ind_t           version_ind;
  } payload;
} __PACKED_GCC;
typedef struct _ble_llcp_pdu_t ble_llcp_pdu_t;

#endif /* #ifndef __BLE_LL_H__ */

