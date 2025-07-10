/*============================================================================*/
/* @file ble_att.h
 * @brief  Host volume, Attribute Protocol, Vol 3, Part F.
 * @author luwei
 * @date 2020/02
 */

#ifndef __BLE_ATT_H__
#define __BLE_ATT_H__

/* Table 3.37:  Attribute protocol summary */
typedef enum {
  ATT_Error_Response = 0x01,
  ATT_Exchange_MTU_Request = 0x02,
  ATT_Exchange_MTU_Response = 0x03,
  ATT_Find_Information_Request = 0x04,
  ATT_Find_Information_Response = 0x05,
  ATT_Find_By_Type_Value_Request = 0x06,
  ATT_Find_By_Type_Value_Response = 0x07,
  ATT_Read_By_Type_Request = 0x08,
  ATT_Read_By_Type_Response = 0x09,
  ATT_Read_Request = 0x0A,
  ATT_Read_Response = 0x0B,
  ATT_Read_Blob_Request = 0x0C,
  ATT_Read_Blob_Response = 0x0D,
  ATT_Read_Multiple_Request = 0x0E,
  ATT_Read_Multiple_Response = 0x0F,
  ATT_Read_by_Group_Type_Request = 0x10,
  ATT_Read_by_Group_Type_Response = 0x11,
  ATT_Write_Request = 0x12,
  ATT_Write_Response = 0x13,
  ATT_Write_command = 0x52,
  ATT_Prepare_Write_Request = 0x16,
  ATT_Prepare_Write_Response = 0x17,
  ATT_Execute_Write_Request = 0x18,
  ATT_Handle_Value_Notification = 0x1B,
  ATT_Handle_Value_Indication = 0x1D,
  ATT_Handle_Value_Confirmation = 0x1E,
  ATT_Signed_Write_command = 0xD2,
} att_opcode_t;

/* Table 3.3: Error codes */
typedef enum {
  ATT_ERR_Invalid_Handle = 0x01, // The attribute handle given was not valid on this server.
  ATT_ERR_Read_Not_Permitted = 0x02, // The attribute cannot be read.
  ATT_ERR_Write_Not_Permitted = 0x03, // The attribute cannot be written.
  ATT_ERR_Invalid_PDU = 0x04, // The attribute PDU was invalid.
  ATT_ERR_Insufficient_Authentication = 0x05, // The attribute requires authentication before it can be read or written.
  ATT_ERR_Request_Not_Supported = 0x06, // Attribute server does not support the request received f rom the client.
  ATT_ERR_Invalid_Offset = 0x07, // Offset specified was past the end of the attribute.
  ATT_ERR_Insufficient_Authorization = 0x08, // The attribute requires authorization before it can be read or written.
  ATT_ERR_Prepare_Queue_Full = 0x09, // Too many prepare writes have been queued.
  ATT_ERR_Attribute_Not_Found = 0x0A, // No attribute found within the given attri-bute handle range.
  ATT_ERR_Attribute_Not_Long = 0x0B, // The attribute cannot be read using the Read Blob Request.
  ATT_ERR_Insufficient_Encryption_Key_Size = 0x0C, // The Encryption Key Size used for encrypting this link is insufficient.
  ATT_ERR_Invalid_Attribute_Value_Length = 0x0D, // The attribute value length is invalid for the operation.
  ATT_ERR_Unlikely_Error = 0x0E, // The attribute request that was requested has encountered an error that was unlikely, and therefore could not be completed as requested.
  ATT_ERR_Insufficient_Encryption = 0x0F, // The attribute requires encryption before it can be read or written.
  ATT_ERR_Unsupported_Group_Type = 0x10, // The attribute type is not a supported grouping attribute as defined by a higher layer specification.
  ATT_ERR_Insufficient_Resources = 0x11, // Insufficient Resources to complete the request.
  ATT_ERR_Database_Out_Of_Sync = 0x12, // The server requests the client to redis-cover the database.
  ATT_ERR_Value_Not_Allowed = 0x13, // The attribute parameter value was not allowed.
  ATT_ERR_Application_Error_Base = 0x80, //  Application error code defined by a higher layer specification.
  ATT_ERR_Common_Profile_and_Service_Error_Base = 0xE0, // Common profile and service error codes defined in [ Core Specification Supplement], Part B.
} att_error_code_t;

/* 3.4   ATTRIBUTE PROTOCOL PDUS */
/* 3.4.1.1  Error Response */
__PACKED struct _ble_att_error_response_t {
  ///The request that generated this error response.
  uint8_t RequestOpcode; //13
  ///The attribute handle that generated this error response.
  uint16_t Handle;       //14
  ///The reason why the request has generated an error response.
  uint8_t ErrorCode;     //16
} __PACKED_GCC;
typedef struct _ble_att_error_response_t ble_att_error_response_t;

/* 3.4.2  MTU exchange */
__PACKED struct _ble_att_exchange_mtu_request_t {
  ///Client receive MTU size.
  uint16_t ClientRxMTU; //13
} __PACKED_GCC;
typedef struct _ble_att_exchange_mtu_request_t ble_att_exchange_mtu_request_t;

__PACKED struct _ble_att_exchange_mtu_response_t {
  ///Attribute server receive MTU size.
  uint16_t ServerRxMTU; //13
} __PACKED_GCC;
typedef struct _ble_att_exchange_mtu_response_t ble_att_exchange_mtu_response_t;

/* 3.4.3.1  Find Information Request */
__PACKED struct _ble_att_find_information_request_t {
  ///First requested handle number.
  uint16_t StartingHandle;   //13
  ///Last requested handle number.
  uint16_t EndingHandle;     //15
} __PACKED_GCC;
typedef struct _ble_att_find_information_request_t ble_att_find_information_request_t;

/* 3.4.3.2  Find Information Response */
__PACKED struct _ble_att_find_information_response_t {
  ///The format of the information data: 1=16-bit UUID; 2=128-bit UUID.
  uint8_t Format; //13
  ///The information data whose format is determined by the Format field: (Handle, UUID) pair.
  uint8_t InformationList[23-2]; //14
} __PACKED_GCC;
typedef struct _ble_att_find_information_response_t ble_att_find_information_response_t;

/* 3.4.3.3  Find By Type Value Request */
__PACKED struct _ble_att_find_by_type_request_t {
  ///First requested handle number.
  uint16_t StartingHandle;   //13
  ///Last requested handle number.
  uint16_t EndingHandle;     //15
  ///2 octet UUID to find.
  uint8_t Type[2];           //17
  ///Attribute value to find.
  uint8_t Value[23-7];       //19
} __PACKED_GCC;
typedef struct _ble_att_find_by_type_request_t ble_att_find_by_type_request_t;

/* 3.4.3.4  Find By Type Value Response */
__PACKED struct _ble_att_find_by_type_response_t {
  ///A list of 1 or more Handle Informations: (Found Attribute Handle, Group End Handle) pair.
  uint16_t HandlesInformationList[(23-1)/2]; //13
} __PACKED_GCC;
typedef struct _ble_att_find_by_type_response_t ble_att_find_by_type_response_t;

/* 3.4.4.1  Read By Type Request */
__PACKED struct _ble_att_read_by_type_request_t {
  ///First requested handle number.
  uint16_t StartingHandle;   //13
  ///Last requested handle number.
  uint16_t EndingHandle;     //15
  ///2 or 16 octet UUID.
  uint8_t Type[16];          //17
} __PACKED_GCC;
typedef struct _ble_att_read_by_type_request_t ble_att_read_by_type_request_t;

/* 3.4.4.2  Read By  Type Response */
__PACKED struct _ble_att_read_by_type_response_t {
  ///The size of each attribute handle-value pair.
  uint8_t Length;         //13
  ///A list of Attribute Data.
  uint8_t DataList[23-2]; //14
} __PACKED_GCC;
typedef struct _ble_att_read_by_type_response_t ble_att_read_by_type_response_t;

/* 3.4.4.3  Read Request */
__PACKED struct _ble_att_read_request_t {
  ///The handle of the attribute to be read.
  uint16_t Handle;   //13
} __PACKED_GCC;
typedef struct _ble_att_read_request_t ble_att_read_request_t;

/* 3.4.4.4  Read Response */
__PACKED struct _ble_att_read_response_t {
  ///The value of the attribute with the handle given.
  uint8_t Value[23-1]; //13
} __PACKED_GCC;
typedef struct _ble_att_read_response_t ble_att_read_response_t;

/* 3.4.4.5  Read Blob Request */
__PACKED struct _ble_att_read_blob_request_t {
  ///The handle of the attribute to be read.
  uint16_t Handle;   //13
  ///The offset of the first octet to be read.
  uint16_t Offset;   //15
} __PACKED_GCC;
typedef struct _ble_att_read_blob_request_t ble_att_read_blob_request_t;

/* 3.4.4.6  Read Blob Response */
__PACKED struct _ble_att_read_blob_response_t {
  ///Part of the value of the attribute with the handle given.
  uint8_t PartValue[23-1]; //13
} __PACKED_GCC;
typedef struct _ble_att_read_blob_response_t ble_att_read_blob_response_t;

/* 3.4.4.9  Read By Group Type Request */
__PACKED struct _ble_att_read_by_group_type_request_t {
  ///First requested handle number.
  uint16_t StartingHandle;   //13
  ///Last requested handle number.
  uint16_t EndingHandle;     //15
  ///2 or 16 octet UUID.
  uint8_t Type[16];          //17
} __PACKED_GCC;
typedef struct _ble_att_read_by_group_type_request_t ble_att_read_by_group_type_request_t;

/* 3.4.4.10  Read By Group Type Response */
__PACKED struct _ble_att_read_by_group_type_response_t {
  ///The size of each attribute handle-value pair.
  uint8_t Length;         //13
  ///A list of Attribute Data.
  uint8_t DataList[23-2]; //14
} __PACKED_GCC;
typedef struct _ble_att_read_by_group_type_response_t ble_att_read_by_group_type_response_t;

/* 3.4.5.1  Write Request */
__PACKED struct _ble_att_write_request_t {
  ///The handle of the attribute to be written.
  uint16_t Handle;        //13
  ///The value to be written to the attribute.
  uint8_t Value[23-3];    //15
} __PACKED_GCC;
typedef struct _ble_att_write_request_t ble_att_write_request_t;

/* 3.4.5.2  Write Response */
//__PACKED struct _ble_att_write_response_t {
//} __PACKED_GCC;
//typedef struct _ble_att_write_response_t ble_att_write_response_t;

/* 3.4.5.3  Write command */
__PACKED struct _ble_att_write_command_t {
  ///The handle of the attribute to be set.
  uint16_t Handle;        //13
  ///The value to be written to the attribute.
  uint8_t Value[23-3];    //15
} __PACKED_GCC;
typedef struct _ble_att_write_command_t ble_att_write_command_t;

/* 3.4.7.1  Handle Value Notification */
__PACKED struct _ble_att_notification_t {
  ///The handle of the attribute.
  uint16_t Handle;     //13
  ///The current value of the attribute.
  uint8_t Value[23-3]; //15
} __PACKED_GCC;
typedef struct _ble_att_notification_t ble_att_notification_t;

__PACKED struct _ble_att_pdu_t {
  ///Attribute Opcode
  uint8_t Opcode;    //12
  __PACKED union {
    ///ATT_MTU=23
    //uint8_t                               data[23-1];
    ble_att_exchange_mtu_request_t        mtu_req;
    ble_att_exchange_mtu_response_t       mtu_rsp;
    ble_att_error_response_t              err_rsp;
    ble_att_find_information_request_t    find_info_req;
    ble_att_find_information_response_t   find_info_rsp;
    ble_att_find_by_type_request_t        find_type_req;
    ble_att_find_by_type_response_t       find_type_rsp;
    ble_att_read_by_type_request_t        read_type_req;
    ble_att_read_by_type_response_t       read_type_rsp;
    ble_att_read_request_t                read_req;
    ble_att_read_response_t               read_rsp;
    ble_att_read_blob_request_t           read_blob_req;
    ble_att_read_blob_response_t          read_blob_rsp;
    ble_att_read_by_group_type_request_t  read_group_req;
    ble_att_read_by_group_type_response_t read_group_rsp;
    ble_att_write_request_t               write_req;
    //ble_att_write_response_t              write_rsp;
    ble_att_write_command_t               write_cmd;
    ble_att_notification_t                notify;
  } payload;
} __PACKED_GCC;
typedef struct _ble_att_pdu_t ble_att_pdu_t;

__PACKED struct _ble_att_pkt_t {
  ble_data_header_t   header;   //0c
  ble_l2cap_header_t  l2cap;    //0e
  ble_att_pdu_t       att;      //12
} __PACKED_GCC;
typedef struct _ble_att_pkt_t ble_att_pkt_t;

#endif /* #ifndef __BLE_ATT_H__ */

