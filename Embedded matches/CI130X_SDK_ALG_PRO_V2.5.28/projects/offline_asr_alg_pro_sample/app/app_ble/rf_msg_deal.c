#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "system_msg_deal.h"
#include "prompt_player.h"
#include "voice_module_uart_protocol.h"
#include "ci_nvdata_manage.h"
#include "ci_log.h"
#include "ci130x_gpio.h"
#include "ci130x_uart.h"
#include "ci130x_dma.h"
#include "ci130x_it.h"

#include "user_config.h"
#include "sdk_default_config.h"
#include "ble_user_config.h"
#include "exe_api.h"
#include "crc.h"
#include "exe_app.h"
#include "rf_msg_deal.h"

void *ble_msg_queue = NULL;
extern rf_cb_funcs_t rf_cb_funcs; // 射频发送和接收的回调函数
static bool send_flag;            // 蓝牙数据发送标志

void dev_state_init(void)
{
#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)
    aircondition_init();
#elif (DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)
    rgb_init();
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)
    tbm_init();
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)
    fan_init();
#elif (DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
    heattable_init();
#elif (DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
    warmer_init();
#elif (DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
    waterheated_init();
#endif
}

void printf_ble_msg_V1(ble_msg_V1_t ble_msg)
{
    mprintf("ble_msg dev_type %x\r\n", ble_msg.dev_type);
    mprintf("ble_msg dev_number %x\r\n", ble_msg.dev_number);
    mprintf("ble_msg function_type %x\r\n", ble_msg.function_type);
    mprintf("ble_msg data_type %x\r\n", ble_msg.data_type);
    mprintf("ble_msg function_id %x\r\n", ble_msg.function_id);
    //mprintf("ble_msg data_len %x\r\n",ble_msg.data_len);
    mprintf("ble_msg msg data 0: %x\r\n", ble_msg.data[0]);
}
/**
 * @brief 接收并处理蓝牙消息任务，按照蓝牙小程序协议或自定义蓝牙协议，解析设备端收到的手机APP消息(该函数只适用于和启英物联APP交互使用)
 *
 */
volatile bool ble_msg_send_erro = false;
volatile bool ble_msg_send_flag = false;
void ci_ble_recv_task()
{
    BaseType_t err = pdPASS;
    ble_msg_V1_t recv_ble_msg;
    uint16_t recv_ble_type;
    ble_msg_queue = xQueueCreate(10, sizeof(ble_msg_V1_t));
    while (1)
    {
        /* 阻塞接收系统消息 */
        if (xQueueReceive(ble_msg_queue, &recv_ble_msg, portMAX_DELAY) != pdPASS)
        {
            mprintf("ble_msg_queue rcv error ...\r\n");
        }
        else
        {
            recv_ble_type = recv_ble_msg.dev_type;
            ble_msg_send_flag = false;
            mprintf("ble_recv_msg type = 0x%x\r\n", recv_ble_type);
            switch (recv_ble_type)
            {
#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)
            case AIRCONDITION_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    aircondition_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    aircondition_query(recv_ble_msg);
                }
            }
            break;
#elif (DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)
            case RGB_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    rgb_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    rgb_query(recv_ble_msg);
                }
            }
            break;
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)
            case TEABAR_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    tbm_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    tbm_query(recv_ble_msg);
                }
            }
            break;
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)
            case FAN_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    fan_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    fan_query(recv_ble_msg);
                }
            }
            break;

#elif (DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
            case HEATTABLE_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    heattable_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    heattable_query(recv_ble_msg);
                }
            }
            break;

#elif (DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
            case WARMER_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    warmer_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    warmer_query(recv_ble_msg);
                }
            }
            break;

#elif (DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
            case WATERHEATED_DEV:
            {
                if (recv_ble_msg.function_type == ATTRIBUTE_SETUP)
                {
                    waterheated_callback(recv_ble_msg);
                }
                else if (recv_ble_msg.function_type == STATE_QUERY)
                {
                    waterheated_query(recv_ble_msg);
                }
            }
            break;
#endif
            default:
                break;
            }
        }
    }
}

extern bool send_msg;
/**
 * @brief 组装蓝牙设备发送的消息，按照蓝牙小程序协议或自定义协议，上报本地IOT事件到手机APP端
 *
 */
void deal_ble_send_msg(uint16_t cmd_id)
{
    uint16_t crc_cal = 0;
    switch (cmd_id)
    {
    default:
    {

#if (DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)
        aircondition_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)
        rgb_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)
        tbm_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)
        fan_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
        heattable_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
        warmer_report(cmd_id);
#elif (DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
        waterheated_report(cmd_id);
#endif
        break;
    }
    }
}

/**
 * @brief 处理蓝牙/2.4G接收到的消息
 * @param send_data 芯片接收到的数据；
 * @param len       接收数据长度；
 */
void custom_rf_recv_data_handle(uint8_t *recv_data, uint8_t len)
{
    if (len > RF_RX_TX_MAX_LEN)
    {
        mprintf("ci_rf_recv_data_handle len error\r\n");
        return;
    }
    else
    {
        mprintf("recv (%d):%s\r\n", len, recv_data);

        rf_cb_funcs.rf_send(recv_data, len);
        // 用户根据自己协议实现
    }
}

/**
 * @brief 处理蓝牙/2.4G接收到的消息-只使用于启英物联加密交互，客户使用自己的私有协议，请使用custom_rf_recv_data_handle函数
 * @param send_data 芯片接收到的数据；
 * @param len       接收数据长度；
 */
void ci_rf_recv_data_handle(uint8_t *recv_data, uint8_t len)
{
    if (len > RF_RX_TX_MAX_LEN)
    {
        mprintf("ci_rf_recv_data_handle len error\r\n");
        return;
    }
#if CIAS_BLE_CONNECT_MODE_ENABLE
    cias_crypto_data(recv_data, len);
    uint16_t crc_cal = crc16_ccitt(0, recv_data, len - 2);
    if (recv_data[len - 1] != (crc_cal & 0xff))
    {
        return;
    }
#endif
    for (size_t i = 0; i < len; i++)
    {
        mprintf("%x ", recv_data[i]);
    }
#if CIAS_BLE_APP_CMD
    ble_cmd_t *recv_ble_cmd = (ble_cmd_t *)recv_data;
    if (recv_ble_cmd->pkg_header[0] == 0xA5 && recv_ble_cmd->pkg_header[1] == 0x5B) // 校验帧头
    {
        if (recv_ble_cmd->ble_cmd == 0x01) // ble连接断开
        {
            app_cb_disconnected();
        }
        memset(recv_data, 0, len);
    }
#endif

#ifdef CIAS_PROTOCOL_VER
    BaseType_t err = pdPASS;
    ble_msg_V1_t *recv_ble_msg = (ble_msg_V1_t *)recv_data;
    if (recv_ble_msg->pkg_header[0] == 0xA5 && recv_ble_msg->pkg_header[1] == 0x5A) // 校验帧头
    {
        if (recv_ble_msg->protocol_ver == 0x01) // V1.0协议版本
        {
            err = xQueueSend(ble_msg_queue, recv_ble_msg, 0); // 开启ble模式时，将收到数据发送到ble处理消息队列，按照蓝牙小程序协议处理消息
            ble_msg_send_flag = true;
            if (err != pdPASS)
            {
                mprintf("deal_ble_recv_msg send fail:%d,%s\n", __LINE__, __FUNCTION__);
            }
        }
    }
    memset(recv_data, 0, len);
#endif
}

void uart_send_asr(uint16_t cmd_id)
{
#if (UART_PROTOCOL_VER == 1)
    uart_send_AsrResult(cmd_id, 0);
#elif (UART_PROTOCOL_VER == 2)
    vmup_send_asr_result_cmd(cmd_id, 0);
#elif (UART_PROTOCOL_VER == 255)
    usr_send_asr_result(cmd_id);
#endif // 0
}

// 按串口协议发送数据
void usr_send_asr_result(uint16_t cmd_id)
{
    int ret;
    switch (cmd_id)
    {
    default:
        ret = 0;
        break;
    }
}