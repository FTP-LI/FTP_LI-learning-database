#ifndef __COMMON_API_H
#define __COMMON_API_H

#include "ci_log.h"
#include "string.h"
#include "device.h"
#include "main_device.h"
#include "air_device.h"
#include "tv_device.h"
#include "fan_device.h"
#include "light_device.h"
#include "command_info.h"
#include "prompt_player.h"

#ifdef __cplusplus
extern "C" {
#endif

//define in ir_data.a ,do not use it
//#define NVDATA_ID_AIRSTATUS                 (0x70000001) /*'AIRSTATUS'*/
//#define NVDATA_ID_AIR_SEARCH_STATUS         (0x70000000) /*'空调搜索存储的数据'*/

#define NVDATA_ID_AIRINFO                   0x70000003 /*'AIRINFO'*/
#define NVDATA_ID_TVINFO                    0x70000004 /*'TVINFO'*/
#define NVDATA_ID_FANINFO                   0x70000006 /*'FANINFO'*/
#define NVDATA_ID_LIGHTINFO                 0x70000007 /*'LIGHTINFO'*/
#define NVDATA_ID_MAININFO                  0x70000008 /*'MAININFO'*/
#define NVDATA_ID_WAKE_NUMBER               0x70000009 /*'LIGHTINFO'*/
#define NVDATA_ID_NIGHT_STATE               0x70000005 //夜灯状态

/*公共数据接口*/
typedef struct _stDeviceInfo_S
{
    int current_deal_index;
    eDevice_Mode device_mode;
    stDevice* CommonDevice;
}stCommonInfo;

typedef struct
{
    stDevice* pDevice;
    stKey* pKey;
    int asr_id;
}device_asr_msg;

typedef struct
{
    stDevice* pDevice;
    stKey* pKey;
    ir_data_t* pir_data;
}device_interrput_msg;

typedef struct
{
    int ir_code;
}device_ir_msg;

typedef struct
{
    int callback_msg;
}device_ir_send_callback_msg;

typedef struct _stDevice433
{
    unsigned char heade;
    unsigned char cmd;
    unsigned char door;
    unsigned char type;
    unsigned char ctl;
}device_uart_msg;

typedef struct
{
    unsigned int uiFirstTimeTick;
    int iOldAsr;
    int iMaxDoubleAsrTime;
}stDoubleAsr;

typedef struct
{
    eDeviceMsgType type;

    /*here will be modify use union*/
    union
    {
        device_asr_msg userasr_data;
        device_interrput_msg timer_data;
        device_ir_msg ir_data;
        device_uart_msg uart_data;
        stUartLearnData uart_learn_data;
        device_ir_send_callback_msg ir_callback_data;
    }msg_data;
}device_msg;

typedef struct
{
    unsigned char bFlag;
    unsigned int totle_cnt;
    unsigned int cnt;
}stSendCnt;

typedef struct
{
    unsigned char bFlag;
    unsigned int totle_timer;
    unsigned int timer;
}stSendTimer;

typedef struct _stContinueSendKey
{
    stKey* pKey;
    unsigned int send_key_TimeMS;
    unsigned int offset_TimeMS;
    stSendCnt send_cnt;
    stSendTimer send_timer;

    int (*ContinueKeySendDown)(stKey* pKey); //发送IR按键完成后回调

    struct _stContinueSendKey* Next;
}stContinueSendKey;

#define RETURN_EXE_NULL (-1) //未执行
#define RETURN_EXE_OK (0)    //执行成功
#define RETURN_EXE_OVER (1)  //执行所有完成



int main_CheckGetLearnStatus(stKey* key, ir_data_t *ir_data);
int AsrCmdPlayIdByCmdHandle(unsigned int cmd_handle, int play_id);
int AsrCmdPlayIdByMsg(stAsrInfo* asrinfo, int play_id);
int AsrCmdPlayIdByCmdIDEx(stAsrInfo* pAsrInfo, uint32_t semantic_id, int play_id);
int AsrCmdPlayIdByCmdID(uint32_t semantic_id, int play_id);
int common_device_InitNightLevel(void);
int common_PlayVoiceAsrIndex(uint32_t semantic_id, int play_index);

int CheckKeyJumpNext(stKeyJump* keyjump, int len, stKey* key, int *plen);

stKey* GetCmdKey(uint32_t CmdIndex, AsrCmd_Key_TypeDef* CmdKey, int len);

int ContinueSendKey_list_init(void);
int check_and_send_continue_key(void);
int delete_all_ContinueSendKey_list(void);
int stop_key_in_ContinueSendKey_list(stKey* pKey);
int check_key_in_ContinueSendKey_list(stKey* pKey);
int check_send_continue_key(stContinueSendKey *pContinueSendKey);
int check_send_continue_key(stContinueSendKey *pContinueSendKey);
void add_to_ContinueSendKey_list(stContinueSendKey* pContinueSendKey);
stContinueSendKey* delete_form_ContinueSendKey_list(stContinueSendKey* pContinueSendKey);

extern void pause_voice_in(void);
extern void default_play_done_callback(cmd_handle_t cmd_handle);
extern void learn_done_play_callback(cmd_handle_t cmd_handle);

#ifdef __cplusplus
}
#endif

//ohther API in SDK
void set_state_enter_wakeup(uint32_t exit_wakup_ms);

void night_light_init(void);
void night_light_set_brightness(uint32_t onoff,uint32_t val);

#endif
