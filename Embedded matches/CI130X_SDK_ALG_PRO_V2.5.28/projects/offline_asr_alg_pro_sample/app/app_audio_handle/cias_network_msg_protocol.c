/**
 ******************************************************************************
 * @文件    cias_network_msg_protocol.c
 * @版本    V1.0.0
 * @日期    2019-5-9
 * @概要    接收网络端的数据，并处理
 ******************************************************************************
 * @注意
 *
 * 版权归chipintelli公司所有，未经允许不得使用或修改
 *
 ******************************************************************************
 */
#include <string.h>
#include "ci_log.h"
#include "cias_network_msg_protocol.h"
#include "cias_network_msg_send_task.h"
#include "prompt_player.h"
#include "cias_uart_protocol.h"
#include "audio_play_api.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "cias_audio_data_handle.h"
#include "codec_manager.h"
// #include "cias_voice_plyer_handle.h"

#define MAX_SEND_NUM 5
#define MAX_ACK_TIME 500 // MS
#define REQ_PLAY_DATA_SIZE (10 * 1024)


pro_wait_acktypedef wait_ackstruct;
audio_play_os_stream_t cur_play_stream = PLAY_NULL;


bool recv_music_end_sem = false;
bool no_audio_data_flag = false;

// int8_t package_data[1050] = {0};

uint8_t gwork_mode = WORKING_APPLICATION_MODE;
int16_t ligth_test_interval_time = 3000; // 3S默认3S



static uint32_t response_timer_cnt = 0;
static uint8_t *login_info = NULL;



int32_t wifi_msg_deal(uint8_t *msg_buf, int32_t msg_len);



uint32_t get_response_timer_cnt(void)
{
    response_timer_cnt = xTaskGetTickCount() - response_timer_cnt;
    ci_loginfo(LOG_MEDIA, "response_timer %d ms\n", response_timer_cnt);
    return response_timer_cnt;
}

uint32_t set_response_timer_cnt_start(void)
{
    return response_timer_cnt = xTaskGetTickCount();
}


void resp_file_config(void)
{
#if 0
    uint16_t login_info_len = 500;
    if(login_info == NULL)
    {
        login_info = pvPortMalloc(login_info_len);
        if(login_info == NULL)
        {
            ci_loginfo(LOG_MEDIA,"login info malloc error\n");
            return;
        }
    }
    requset_flash_ctl();
    flash_read(FLASH_SPI_PORT,(uint32_t)login_info,PRODUCT_INFO_SPIFLASH_START_ADDR,login_info_len);
    release_flash_ctl();
    cias_send_cmd_and_data(CI_PROFILE_MSG, login_info, login_info_len, DEF_FILL);
    ci_loginfo(LOG_MEDIA,"send login info\n");
#endif
}

static int32_t current_network_state_t = WIFI_DISCONNECT_STATE;
int32_t wifi_current_state_set(int32_t arg)
{
    if(arg > NETWORK_DISCONNECT_STATE||arg < WIFI_CONNECTED_STATE)
        return -1;

    current_network_state_t = arg;
    return 0;
}
int32_t wifi_current_state_get(void)
{
    return current_network_state_t;
}

/**
 * @brief 发送退出唤醒消息
 *
 */
void send_exit_wakeup_msg(char debug)
{
    ci_logdebug(LOG_USER, "send_exit_wakeup_msg = %d\n", debug);
    sys_msg_t send_msg;
    send_msg.msg_type = SYS_MSG_TYPE_CMD_INFO;
    send_msg.msg_data.cmd_info_data.cmd_info_status = MSG_CMD_INFO_STATUS_EXIT_WAKEUP;
    if (pdPASS != send_msg_to_sys_task(&send_msg, NULL))
    {
        ci_logdebug(LOG_USER, "send_exit_wakeup_msg err\n");
    }
}


// typedef enum{

//     NET_MSG_HEAD = 1, //
//     NET_MSG_DATE = 2,
//     NET_MSG_ERR  = 0,
//     NET_MSG_IDE  = 3,
// }wifi_communicate_state_t;

volatile bool start_recv_flag = false;//向wifi请求数据

/**
 * @brief WIFI 消息处理
 *
 * @param msg_buf 数据信息
 * @param msg_len 数据长度
 * @return int32_t RETURN_OK 处理成功，RETURN_ERR 数据异常
 */
// extern int32_t aux_aiot_aircondition_callback(uint8_t *msg_buf);
int32_t wifi_msg_deal(uint8_t *msg_buf, int32_t msg_len)
{
    if ((msg_buf == NULL) || (msg_len <= 0))
    {
        return RETURN_ERR;
    }
#if TUYA_IR_CTRL_DEMO_ENABLE | CI_IR_CTRL_DEMO_ENABLE   //涂鸦红外遥控器demo
    tuya_ir_wifi_msg_callback(msg_buf);
#endif

#if TENCENT_IOT_LIGHT_DEMO_ENABLE | TENCENT_IOT_IR_DEMO_ENABLE
    tencent_iot_network_callback(msg_buf);
#endif
#if CIAS_DOUBLE_MIC_DEMO_ENABLE | CIAS_SIGNAL_MIC_DEMO_ENABLE
    aux_aiot_aircondition_callback(msg_buf);
#endif

    return RETURN_OK;
}
