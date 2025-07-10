/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-03-08 09:58:20
 * @LastEditTime: 2023-05-17 13:52:33
 * @Description: #
 */
#ifndef __BLE_IOT_MSG_DEAL_H__
#define __BLE_IOT_MSG_DEAL_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include "FreeRTOS.h"
#include "user_config.h"
#include "sdk_default_config.h"

typedef enum{
    IR_DEV = 1,          
    AIRCONDITION_DEV,
    RGB_DEV,
    AUDIO_DEV,
    TEABAR_DEV,
    FAN_DEV,
    HEATTABLE_DEV,
    WARMER_DEV,
    WATERHEATED_DEV,
}dev_type_t;

typedef enum{
    ATTRIBUTE_SETUP = 1,
    EVENT_REPORT,
    STATE_QUERY,
    STATE_RECOVERY,
}data_type_t;

typedef enum{
    FUN_TURN_OFF = 1,
    FUN_TURN_ON,
}dev_function_t;

#pragma pack(1) 
typedef struct
{
    uint8_t pkg_header[2];      //包头
    uint8_t  protocol_ver;    //协议版本
    uint8_t manufacturer_id;  //厂商ID
    uint8_t dev_type;         //设备类型
    uint8_t dev_number;       //设备编号
    uint8_t data_type:4;      //数据类型
    uint8_t function_type:4;  //功能类型
    uint8_t function_id;      //功能ID
    uint16_t data_len;        //小端传输
    uint8_t  data[8];         //数据部分
}ble_msg_V1_t;  
#pragma pack()

#pragma pack(1) 	
typedef struct
{
    uint8_t pkg_header[2];      //包头
    uint8_t ble_cmd;            //蓝牙命令
}ble_cmd_t; 
#pragma pack()

void fan_init(void);
void fan_callback(ble_msg_V1_t msg);
void fan_query(ble_msg_V1_t msg);
uint8_t fan_report(uint16_t cmd_id);

void warmer_init(void);
void warmer_callback(ble_msg_V1_t msg);
void warmer_query(ble_msg_V1_t msg);
uint8_t warmer_report(uint16_t cmd_id);

void aircondition_init(void);
void aircondition_callback(ble_msg_V1_t msg);
void aircondition_query(ble_msg_V1_t msg);
uint8_t aircondition_report(uint16_t cmd_id);

void heattable_init(void);
void heattable_callback(ble_msg_V1_t msg);
void heattable_query(ble_msg_V1_t msg);
uint8_t heattable_report(uint16_t cmd_id);

void tbm_init(void);
void tbm_callback(ble_msg_V1_t msg);
void tbm_query(ble_msg_V1_t msg);
uint8_t tbm_report(uint16_t cmd_id);

void rgb_init(void);
void rgb_callback(ble_msg_V1_t msg);
void rgb_query(ble_msg_V1_t msg);
uint8_t rgb_report(uint16_t cmd_id);

void waterheated_init(void);
void waterheated_callback(ble_msg_V1_t msg);
void waterheated_query(ble_msg_V1_t msg);
uint8_t waterheated_report(uint16_t cmd_id);

void dev_state_init(void);
void uart1_send_str(char *str);
void ci_ble_recv_task(void);
void deal_ble_send_msg(uint16_t cmd_id);

void ci_rf_recv_data_handle(uint8_t* recv_data, uint8_t len);
void custom_rf_recv_data_handle(uint8_t* recv_data, uint8_t len);
void uart_send_asr(uint16_t cmd_id);
void usr_send_asr_result(uint16_t cmd_id);
void ble_xtal_init(void);
void ble_model_init(void);
#endif