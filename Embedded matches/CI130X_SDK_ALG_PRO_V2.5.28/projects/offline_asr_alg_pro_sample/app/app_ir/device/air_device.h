/**
  ******************************************************************************
  * @文件    air_device.c
  * @作者    chipintelli软件团队
  * @版本    V1.0.0
  * @日期    2016-4-9
  * @概要    这个文件是chipintelli公司的CI100X芯片程序的命令词处理文件.
  ******************************************************************************
  * @注意
  *
  * 版权归chipintelli公司所有，未经允许不得使用或修改
  *
  ******************************************************************************
  */
#ifndef _AIR_DEVICE_H
#define _AIR_DEVICE_H
#include "main_device.h"
#include "command_info.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    AIR_ASR_FAST_MATCH_AIR = 1000,
    AIR_ASR_LEARN_AIR,
    AIR_ASR_CLEAN_CODE,
    AIR_ASR_OPEN_AIR = AIR_ASR_FAST_MATCH_AIR + 5,
    AIR_ASR_CLOSE_AIR,
    AIR_ASR_HIGHT_FAN,
    AIR_ASR_MIND_FAN,
    AIR_ASR_LOW_FAN,
    AIR_ASR_AUTO_FAN,
    AIR_ASR_CLOSE_SCAN,
    AIR_ASR_OPEN_SCAN,
    AIR_ASR_TEMP_19,
    AIR_ASR_TEMP_20,
    AIR_ASR_TEMP_21,
    AIR_ASR_TEMP_22,
    AIR_ASR_TEMP_23,
    AIR_ASR_TEMP_24,
    AIR_ASR_TEMP_25,
    AIR_ASR_TEMP_26,
    AIR_ASR_TEMP_27,
    AIR_ASR_TEMP_28,
    AIR_ASR_TEMP_29,
    AIR_ASR_TEMP_30,
    AIR_ASR_COOL_MODE,
    AIR_ASR_HOT_MODE,
    AIR_ASR_FAN_MODE,
    AIR_ASR_WET_MODE,
    AIR_ASR_AUTO_MODE,
    AIR_ASR_TEMP_16 = AIR_ASR_FAST_MATCH_AIR + 30,
    AIR_ASR_TEMP_17,
    AIR_ASR_TEMP_18,
    AIR_ASR_UP_DOWN_SCAN = AIR_ASR_FAST_MATCH_AIR + 68,
    AIR_ASR_LEFT_DOWN_SCAN,
    AIR_ASR_UP_DOWN_SCAN_STOP,
    AIR_ASR_LEFT_DOWN_SCAN_STOP,
    AIR_ASR_TEMP_UP,
    AIR_ASR_TEMP_DOWN,
    AIR_ASR_FAN_UP,
    AIR_ASR_FAN_DOWN,
    AIR_ASR_SLEEP_CLOSE,
    AIR_ASR_SLEEP_OPEN,
    AIR_ASR_ECO_CLOSE,
    AIR_ASR_ECO_OPEN,
    AIR_ASR_OSD_CLOSE,
    AIR_ASR_OSD_OPEN,
    AIR_ASR_ALL_AUTO = AIR_ASR_FAST_MATCH_AIR + 100,
    AIR_ASR_FAST_COOL,
    AIR_ASR_FAST_HOT,
    AIR_ASR_LOWN_POWER_MODE,
    //定时功能
    AIR_ASR_TIME_CANCEL,
    AIR_ASR_TIME_1H,
    AIR_ASR_TIME_2H,
    AIR_ASR_TIME_3H,
    AIR_ASR_TIME_4H,
    AIR_ASR_TIME_5H,
    AIR_ASR_TIME_6H,
    AIR_ASR_TIME_7H,
    AIR_ASR_TIME_8H,
    AIR_ASR_TIME_9H,
    AIR_ASR_TIME_10H,

    AIR_VOICE_MATCH_SUCCESS = AIR_ASR_FAST_MATCH_AIR + 200,
    AIR_VOICE_MATCH_FAIL1,
    AIR_VOICE_MATCH_FAIL2,
    AIR_VOICE_MATCH_FAIL3,
    AIR_VOICE_MATCH_FAIL,
    AIR_VOICE_SEARCH_COMPLETE,
    AIR_VOICE_POWER_ON,
    AIR_VOICE_POWER_OFF,
    AIR_VOICE_AUTO_MODE,
    AIR_VOICE_MOVE_WET,
    AIR_VOICE_COOL_MODE,
    AIR_VOICE_HOTE_MODE,
    AIR_VOICE_SAVE_POWER,
    AIR_VOICE_TEMP_UP,
    AIR_VOICE_TEMP_DOWN,
    AIR_VOICE_HORIZONTAL_FAN,
    AIR_VOICE_VERTICAL_FAN,
    AIR_VOICE_SWITCH_FAN_SPEED,
    AIR_VOICE_OPEN_AIR_FIRST = AIR_ASR_FAST_MATCH_AIR + 250,

    AIR_ASR_SET_LG = 1500,
    AIR_ASR_SET_TCL,
    AIR_ASR_SET_AUX,
    AIR_ASR_SET_CHIGO,
    AIR_ASR_SET_PANASONIC,
    AIR_ASR_SET_GREE,
    AIR_ASR_SET_HISENSE,
    AIR_ASR_SET_HAIER,
    AIR_ASR_SET_KELON,
    AIR_ASR_SET_MEDIA,
    AIR_ASR_SET_CHANGHONG,
    AIR_ASR_SET_HITACHI,
    AIR_ASR_SET_DAIKIN,
    AIR_ASR_SET_MITSUBISHI,
    AIR_ASR_SET_MI,
    AIR_ASR_SET_WHIRLPOOL,

    AIR_ASR_SET_GALANZ,       //格兰仕
    AIR_ASR_SET_FUJITSU,      //富士通
    AIR_ASR_SET_SANSHUI,      //山水
    AIR_ASR_SET_YORK,         //约克
    AIR_ASR_SET_SKYWORTH,     //创维
    AIR_ASR_SET_SHINCO,       //新科
    AIR_ASR_SET_CHUNLAN,      //春兰
    AIR_ASR_SET_CHEBLO,       //樱花
    AIR_ASR_SET_SAMSUNG,      //三星
    AIR_ASR_SET_AUCMA,        //澳柯玛
    AIR_ASR_SET_XINFEI,       //新飞
    AIR_ASR_SET_TOSHIBA,      //东芝
    AIR_ASR_SET_SAMPO,        //声宝
    AIR_ASR_SET_YUETU,        //月兔
    AIR_ASR_SET_YAIR,         //扬子
    AIR_ASR_SET_MOYE,         //夏新
    AIR_ASR_SET_SHAIP,        //夏普
    AIR_ASR_SET_END,

    AIR_ASR_OPEN_AUXILIARY_HEAT = 1600,
    AIR_ASR_CLOSE_AUXILIARY_HEAT,
    AIR_ASR_OPEN_FRESH_AIR,
    AIR_ASR_CLOSE_FRESH_AIR,
    AIR_ASR_OPEN_WIND_TO_MAN,
    AIR_ASR_CLOSE_WIND_TO_MAN,
    AIR_ASR_SLEEP2_OPEN,
    AIR_ASR_SLEEP3_OPEN,
    AIR_ASR_OPEN_HEALTH,
    AIR_ASR_CLOSE_HEALTH,
    AIR_ASR_OPEN_MUTE_FAN,
    AIR_ASR_CLOSE_MUTE_FAN,
    AIR_ASR_OPEN_STRONG_FAN,
    AIR_ASR_CLOSE_STRONG_FAN,
    AIR_ASR_OPEN_SENSE,
    AIR_ASR_CLOSE_SENSE,

    AIR_ASR_SERCH_AIR_INDEX = 1700,
    AIR_ASR_SERCH_AIR_BRAND,
    AIR_ASR_MAX,
}eAirASRIndex;

typedef struct
{
    int ir_code;
    unsigned char air_mode;//0:快速匹配，1.按键匹配 2、搜索空调，包括搜索品牌
}stAirInfo;

typedef struct
{
    eAirASRIndex semantic_id;//语义ID
    int air_cmd;//空调对应操作命令词
}stAirAsrCmdTable;


typedef struct
{
    stKey PowerOnKey;
    stKey PowerOffKey;
    stKey TempUpKey;
    stKey TempDownKey;
    stKey AutoModeKey;
    stKey MoveWetKey;
    stKey CoolKey;
    stKey HotKey;
    stKey HoriScanKey;
    stKey VertScanKey;
    stKey WindSpeedKey;
    stKey SavePowerKey;
}stAirKey;

stDevice* get_air_device(void);
int air_device_send_callback(eDeviceMsgType, int callback_msg);
int air_CheckTimer(void);
void air_ctl(int bOpen, cmd_handle_t cmd_handle);

#ifdef __cplusplus
}
#endif

#endif /*__CI100X_ASRCMD_H*/
