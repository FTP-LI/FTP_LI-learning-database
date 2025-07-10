/*============================================================================*/
/* @file ble_l2cap.h
 * @spec   LOGICAL LINK CONTROL AND ADAPTATION PROTOCOL SPECIFICATION, Vol 3, Part A.
 * @author luwei
 * @date 2020/02
 */

#ifndef __BLE_L2CAP_H__
#define __BLE_L2CAP_H__

/* Vol 3, Part A, Table 2.2:  CID name space on LE-U logical link */
#define L2CAP_CID_SIGNALING             0x0001
#define L2CAP_CID_ATT                   0x0004
#define L2CAP_CID_SIGNALING_LE          0x0005
#define L2CAP_CID_SMP                   0x0006
#define L2CAP_CID_APPLE                 0x003A

/* Table 4.2:  Signaling command codes */
typedef enum {
  L2CAP_SIG_Command_reject = 0x01,
  L2CAP_SIG_Connection_request = 0x02,
  L2CAP_SIG_Connection_response = 0x03,
  L2CAP_SIG_Disconnection_request = 0x06,
  L2CAP_SIG_Disconnection_response = 0x07,
  L2CAP_SIG_Information_request = 0x0A,
  L2CAP_SIG_Connection_Parameter_Update_request = 0x12,
  L2CAP_SIG_Connection_Parameter_Update_response = 0x13,
  L2CAP_SIG_LE_Credit_Based_Connection_request = 0x14,
  L2CAP_SIG_LE_Credit_Based_Connection_response = 0x15,
  L2CAP_SIG_LE_Flow_Control_Credit = 0x16,
} ble_l2cap_signaling_code_t;

__PACKED struct _ble_l2cap_header_t {
  ///The length of l2cap payload: AttValue's length + 3
  uint16_t Length;   //0e
  ///CID: 4 = Attribute Protocol
  uint16_t ChannelId;//10
} __PACKED_GCC;
typedef struct _ble_l2cap_header_t ble_l2cap_header_t;

/* 4.1   COMMAND REJECT (CODE 0x01) */
__PACKED struct _ble_l2cap_signaling_command_reject_t {
  ///0x0000: Command not understood
  ///0x0001: Signalling MTU exceeded
  ///0x0002: Invalid CID in request
  uint16_t Reason;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_command_reject_t ble_l2cap_signaling_command_reject_t;

/* 4.2   CONNECTION REQUEST (CODE 0x02) */
__PACKED struct _ble_l2cap_signaling_connection_request_t {
  ///Protocol/Service Multiplexer.
  uint16_t PSM;
  ///It represents a channel endpoint on the device sending the request.
  uint16_t SourceCID;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_connection_request_t ble_l2cap_signaling_connection_request_t;

/* 4.3   CONNECTION RESPONSE (CODE 0x03) */
__PACKED struct _ble_l2cap_signaling_connection_response_t {
  ///It represents a channel endpoint on the device sending this response.
  uint16_t DestinationCID;
  ///It represents a channel endpoint on the device receiving this response.
  uint16_t SourceCID;
  ///It indicates the outcome of the connection request.
  uint16_t Result;
  ///It indicates the stauts of the Pending connection.
  uint16_t Status;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_connection_response_t ble_l2cap_signaling_connection_response_t;

/* 4.6   DISCONNECTION REQUEST (CODE 0x06) */
__PACKED struct _ble_l2cap_signaling_disconnection_request_t {
  ///It represents a channel endpoint on the device receiving this request.
  uint16_t DestinationCID;
  ///It represents a channel endpoint on the device sending this request.
  uint16_t SourceCID;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_disconnection_request_t ble_l2cap_signaling_disconnection_request_t;

/* 4.7   DISCONNECTION RESPONSE (CODE 0x07) */
__PACKED struct _ble_l2cap_signaling_disconnection_response_t {
  ///It represents a channel endpoint on the device sending this response.
  uint16_t DestinationCID;
  ///It represents a channel endpoint on the device receiving this response.
  uint16_t SourceCID;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_disconnection_response_t ble_l2cap_signaling_disconnection_response_t;

/* 4.10   INFORMATION REQUEST (CODE 0x0A) */
__PACKED struct _ble_l2cap_signaling_information_request_t {
  ///0x0001: Connectionless MTU
  ///0x0002: Extended Features supported
  uint16_t InfoType;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_information_request_t ble_l2cap_signaling_information_request_t;

/* 4.20   CONNECTION PARAMETER UPDATE REQUEST (CODE 0x12) */
__PACKED struct _ble_l2cap_signaling_connection_parameters_update_request_t {
  ///Defines minimum value for the connection interval: connIntervalMin = IntervalMin * 1.25ms, [6, 3200] frames.
  uint16_t IntervalMin;
  ///Defines maximum value for the connection interval: [6, 3200] frames.
  uint16_t IntervalMax;
  ///Defines the slave latency parameter (as number of LL connection  events): [0, min(500,((connSupervisionTimeout / (connIntervalMax*2)) -1))].
  uint16_t SlaveLatency;
  ///Defines connection timeout parameter: connSupervisionTimeout = Timeout Multiplier * 10 ms, [10, 32000]
  uint16_t TimeoutMultiplier;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_connection_parameters_update_request_t ble_l2cap_signaling_connection_parameters_update_request_t;

/* 4.21   CONNECTION PARAMETER UPDATE RESPONSE (CODE 0x13) */
__PACKED struct _ble_l2cap_signaling_connection_parameters_update_response_t {
  ///Connection Parameters accepted 0x0000 or Connection Parameters rejected 0x0001.
  uint16_t Result;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_connection_parameters_update_response_t ble_l2cap_signaling_connection_parameters_update_response_t;

/* 4.22   LE CREDIT BASED CONNECTION REQUEST (CODE 0x14) */
__PACKED struct _ble_l2cap_signaling_le_connection_request_t {
  ///LE Protocol/Service Multiplexer.
  uint16_t LE_PSM;
  ///It represents a channel endpoint on the device sending the request.
  uint16_t SourceCID;
  ///It specifies the maximum SDU (Service Data Unit) size.
  uint16_t MTU;
  ///It specifies the maximum payload size in L2CAP.
  uint16_t MPS;
  ///It indicate the number of LE-frames that the peer device can send to the requestor.
  uint16_t InitialCredits;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_le_connection_request_t ble_l2cap_signaling_le_connection_request_t;

/* 4.23   LE CREDIT BASED CONNECTION RESPONSE (CODE 0x15) */
__PACKED struct _ble_l2cap_signaling_le_connection_response_t {
  ///It represents a channel endpoint on the device sending this response.
  uint16_t DestinationCID;
  ///It specifies the maximum SDU size.
  uint16_t MTU;
  ///It specifies the maximum payload size.
  uint16_t MPS;
  ///It indicate the number of LE-frames that the peer device can send to the requestor.
  uint16_t InitialCredits;
  ///It indicates the outcome of the connection request.
  uint16_t Result;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_le_connection_response_t ble_l2cap_signaling_le_connection_response_t;

/* Figure 4.2:  Command format */
__PACKED struct _ble_l2cap_signaling_pdu_t {
  ///It identifies the type of com mand.
  uint8_t Code;
  ///It matches responses with requests.
  uint8_t Identifier;
  ///It indicates the size in octets of the data field, i.e., it does not cover the Code, Id entifier, and Length fields. 
  uint16_t Length;
  __PACKED union {
    //uint8_t data[23-4];
    ble_l2cap_signaling_command_reject_t cmd_rej;
    ble_l2cap_signaling_connection_request_t conn_req;
    ble_l2cap_signaling_connection_response_t conn_rsp;
    ble_l2cap_signaling_disconnection_request_t disconn_req;
    ble_l2cap_signaling_disconnection_response_t disconn_rsp;
    ble_l2cap_signaling_information_request_t info_req;
    ble_l2cap_signaling_connection_parameters_update_request_t conn_update_req;
    ble_l2cap_signaling_connection_parameters_update_response_t conn_update_rsp;
    ble_l2cap_signaling_le_connection_request_t le_conn_req;
    ble_l2cap_signaling_le_connection_response_t le_conn_rsp;
  } payload;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_pdu_t ble_l2cap_signaling_pdu_t;

__PACKED struct _ble_l2cap_signaling_pkt_t {
  ble_data_header_t         header;   //0c
  ble_l2cap_header_t        l2cap;    //0e
  ble_l2cap_signaling_pdu_t signal;
} __PACKED_GCC;
typedef struct _ble_l2cap_signaling_pkt_t ble_l2cap_signaling_pkt_t;

#endif /* #ifndef __BLE_L2CAP_H__ */

