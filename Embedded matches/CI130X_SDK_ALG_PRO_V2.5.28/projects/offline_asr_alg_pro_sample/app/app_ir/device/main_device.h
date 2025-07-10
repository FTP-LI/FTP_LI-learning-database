/**
  ******************************************************************************
  * @文件    main_device.h
  * @作者    chipintelli软件团队
  * @版本    V1.0.0
  * @日期    2016-4-9
  * @概要    这个文件是chipintelli公司的CI100X芯片程序的命令词处理头文件。
  ******************************************************************************
  * @注意
  *
  * 版权归chipintelli公司所有，未经允许不得使用或修改
  *
  ******************************************************************************
  */
#ifndef __MAIN_DEVICE_H
#define __MAIN_DEVICE_H
#include "user_config.h"
#include "common_api.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MAIN_ASR_START = 60000,
    MAIN_ASR_WAKEUP_1,
    MAIN_ASR_WAKEUP_2,
    MAIN_ASR_WAKEUP_3,
    MAIN_ASR_WAKEUP_4,
    MAIN_ASR_WAKEUP_5,
    MAIN_ASR_BIGER_VOL,
    MAIN_ASR_SMALLER_VOL,
    MAIN_ASR_MAX_VOL,
    MAIN_ASR_MIN_VOL,
    MAIN_ASR_OPEN_LIGHT, // 60010
    MAIN_ASR_CLOSE_LIGHT,
    MAIN_ASR_LIGHT_UP,
    MAIN_ASR_LIGHT_DOWN,
    MAIN_ASR_LOWST_LIGHT,
    MAIN_ASR_HIGHT_LIGHT,
    MAIN_ASR_VOICE_GUIDE,//60016
    MAIN_ASR_REASE_ALL_CODE,
    MAIN_ASR_CANCEL_CLEAN_CODE,
    MAIN_ASR_ENTER_CLEAN_CODE,
    MAIN_ASR_BEEP_MODE, // 60020
    MAIN_ASR_VOICE_MODE, // 60021
	MAIN_ASR_ONESHORT_OPEN_AIR = MAIN_ASR_START + 200,
	MAIN_ASR_ONESHORT_CLOSE_AIR,

    MAIN_ASR_EXIT_MATCH = MAIN_ASR_START + 500,
    MAIN_ASR_LEARN_NEXT_KEY,
    MAIN_ASR_LEARN_PREV_KEY,

    MAIN_VOICE_POWER_ON = MAIN_ASR_START + 600,
    MAIN_VOICE_NO_IR_DATA,
    MAIN_VOICE_BEEP,
    MAIN_VOICE_MAX_VOICE,
    MAIN_VOICE_MIN_VOICE,
    MAIN_ASR_MAX,
}eCommonASRIndex;

typedef enum
{
    DEVICE_MODE_TV,
    DEVICE_MODE_STB,
    DEVICE_MODE_MAX,
}eDeviceModeCtl;

typedef struct
{
    unsigned char inited;
    unsigned char bFirstUsed;
    unsigned char LightPower;
    int night_light_level;
    int night_light_onoff;/*1:on,0:off*/
    int iWakeUpIndex;
    eDeviceModeCtl eTvStbSwitch;
}stMainStatus;


eDeviceModeCtl main_GetDeviceMode(void);

int main_device_overLearnIR(int iNeedPlay, int iPlayId);
int getBrightLevelValue(int level);
int getLevelCnt(void);

stDevice* get_main_device(void);

#ifdef __cplusplus
}
#endif

#endif
