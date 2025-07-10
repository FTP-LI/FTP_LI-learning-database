/*============================================================================*/
/* @file ble_smp.h
 * @spec   SECURITY MANAGER SPECIFICATION, Vol 3, Part H.
 * @author luwei
 * @date 2020/02
 */

#ifndef __BLE_SMP_H__
#define __BLE_SMP_H__

/* Vol 3, Part H, Table 3.3:  SMP command codes */
typedef enum {
  SMP_Pairing_Request = 0x01,
  SMP_Pairing_Response,
  SMP_Pairing_Confirm,
  SMP_Pairing_Random,
  SMP_Pairing_Failed,
  SMP_Encryption_Information,
  SMP_Master_Identification,
  SMP_Identity_Information,         //08
  SMP_Identity_Address_Information,
  SMP_Signing_Information,
  SMP_Security_Request,
  /* 4.2: Secure Connection. */
  SMP_Pairing_Public_Key,
  SMP_Pairing_DHKey_Check,
  SMP_Pairing_Keypress_Notification,
} smp_command_codes_t;

/* 3.6.1  Key distribut ion and generation */
__PACKED struct _ble_key_distribution_t {
  uint8_t EncKey  :1;
  uint8_t IdKey   :1;
  uint8_t SignKey :1;
  uint8_t LinkKey :1;
  uint8_t RFU     :4;
} __PACKED_GCC;
typedef struct _ble_key_distribution_t ble_key_distribution_t;

/* 3.5.1  Pairing Request
   3.5.2  Pairing Response */
__PACKED struct _ble_smp_pairing_request_response_t {
  uint8_t IOCapability;             //13
  uint8_t OOBdataFlag;              //14
  uint8_t AuthReq;                  //15
  uint8_t MaximumEncryptionKeySize; //16
  ///Initiator Key Distribution / Generation.
  ble_key_distribution_t InitiatorKeyDistribution; //17
  ///Responder Key Distribution / Generation.
  ble_key_distribution_t ResponderKeyDistribution; //18
} __PACKED_GCC;
typedef struct _ble_smp_pairing_request_response_t ble_smp_pairing_request_response_t;

/* 3.5.3  Pairing Confirm
   3.5.4  Pairing Random */
__PACKED struct _ble_smp_pairing_confirm_random_t {
  uint8_t Data[16];
} __PACKED_GCC;
typedef struct _ble_smp_pairing_confirm_random_t ble_smp_pairing_confirm_random_t;

/* 3.5.5  Pairing Failed */
__PACKED struct _ble_smp_pairing_failed_t {
  uint8_t Reason;
} __PACKED_GCC;
typedef struct _ble_smp_pairing_failed_t ble_smp_pairing_failed_t;

/* 3.6.2  Encryption Information
   3.6.4  Identity Information
   3.6.6  Signing Information
   3.5.7  Pairing DHKey Check */
__PACKED struct _ble_smp_key_t {
  uint8_t Key[16];
} __PACKED_GCC;
typedef struct _ble_smp_key_t ble_smp_key_t;

/* 3.6.3  Master Identification */
__PACKED struct _ble_smp_master_identification_t {
  uint16_t EDIV;
  uint8_t  Rand[8];
} __PACKED_GCC;
typedef struct _ble_smp_master_identification_t ble_smp_master_identification_t;

/* 3.6.5  Identity Address Information */
__PACKED struct _ble_smp_identity_address_information_t {
  uint8_t AddrType;
  uint8_t BDA[6];
} __PACKED_GCC;
typedef struct _ble_smp_identity_address_information_t ble_smp_identity_address_information_t;

/* 3.6.7  Security Request */
__PACKED struct _ble_smp_security_request_t {
  uint8_t AuthReq;
} __PACKED_GCC;
typedef struct _ble_smp_security_request_t ble_smp_security_request_t;

/* 3.5.6  Pairing Public Key */
__PACKED struct _ble_smp_pairing_public_key_t {
  uint8_t PublicKeyX[32];
  uint8_t PublicKeyY[32];
} __PACKED_GCC;
typedef struct _ble_smp_pairing_public_key_t ble_smp_pairing_public_key_t;

/* 3.5.8  Keypress Notification */
__PACKED struct _ble_smp_key_notification_t {
  uint8_t NotificationType;
} __PACKED_GCC;
typedef struct _ble_smp_key_notification_t ble_smp_key_notification_t;

/* 3.3   COMMAND FORMAT */
__PACKED struct _ble_smp_pdu_t {
  ///Identifies the type of command.
  uint8_t Code;    //12
  __PACKED union {
    ///ATT_MTU=23
    //uint8_t                                data[23-1];
    ble_smp_security_request_t             sec_req;
    ble_smp_pairing_request_response_t     pair_req;
    ble_smp_pairing_request_response_t     pair_rsp;
    ble_smp_pairing_confirm_random_t       pair_cfm;
    ble_smp_pairing_confirm_random_t       pair_rnd;
    ble_smp_pairing_failed_t               pair_err;
    ble_smp_key_t                          enc_key;
    ble_smp_key_t                          id_key;
    ble_smp_key_t                          sign_key;
    ble_smp_master_identification_t        master_id;
    ble_smp_identity_address_information_t id_addr;
    /* 4.2: it cannot be put in a complete small ble packet. */
    //ble_smp_pairing_public_key_t         pair_pkey;
    ble_smp_key_t                          dhkey;
    ble_smp_key_notification_t             key_ntf;
  } payload;
} __PACKED_GCC;
typedef struct _ble_smp_pdu_t ble_smp_pdu_t;

__PACKED struct _ble_smp_pkt_t {
  ble_data_header_t   header;   //0c
  ble_l2cap_header_t  l2cap;    //0e
  ble_smp_pdu_t       smp;      //12
} __PACKED_GCC;
typedef struct _ble_smp_pkt_t ble_smp_pkt_t;

#endif /* #ifndef __BLE_SMP_H__ */


