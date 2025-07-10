/**
  ******************************************************************************
  * @文件    tv_device.h
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
#ifndef _TV_DEVICE_H
#define _TV_DEVICE_H
#include "device.h"
#ifdef DEVICE_SUPPORT_TV
#include "main_device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TV_ASR_MATCH_TV = 2000,
    TV_ASR_CLEAN_CODE,
    TV_ASR_OPEN_TV,
    TV_ASR_CLOSE_TV,
    TV_ASR_OPEN_STB,
    TV_ASR_CLOSE_STB,
    TV_ASR_SWITCH_SOURCE,
    TV_ASR_STB_MENU,
    TV_ASR_STB_MENU_CONFIRME,
    TV_ASR_STB_BACK,
    TV_ASR_UP_C,
    TV_ASR_DOWN_C,
    TV_ASR_LEFT_C,
    TV_ASR_RIGHT_C,
    TV_ASR_PREV_PROG,
    TV_ASR_NEXT_PROG,
    TV_ASR_UP_TV_VOL,
    TV_ASR_DOWN_TV_VOL,
    TV_ASR_MUTE_VOL,
    TV_ASR_STOP_AUTO_SWITCH_CHANNEL,
    TV_ASR_AUTO_SWITCH_CHANNEL,

    TV_VOICE_TV_POWER_ON = 2200,
    TV_VOICE_TV_POWER_OFF,
    TV_VOICE_STB_POWER_ON,
    TV_VOICE_STB_POWER_OFF,
    TV_VOICE_SORUCE,
    TV_VOICE_STB_MENU,
    TV_VOICE_STB_MENU_CONFIRME,
    TV_VOICE_STB_MENU_BACK,
    TV_VOICE_UP_C,
    TV_VOICE_DOWN_C,
    TV_VOICE_LEFT_C,
    TV_VOICE_RIGHT_C,
    TV_VOICE_PREV_PROG,
    TV_VOICE_NEXT_PROG,
    TV_VOICE_TV_VOL_UP,
    TV_VOICE_TV_VOL_DOWN,
    TV_VOICE_MUTE,

    TV_VOICE_MATCH_FAN_FIRST = TV_ASR_MATCH_TV + 250,
    TV_VOICE_OPEN_FAN_FIRST,
    TV_ASR_MAX,
}eTvASRIndex;

typedef struct
{
    stKey TvPowerOnKey;
    stKey TvPowerOffKey;
    stKey StbPowerOnKey;
    stKey StbPowerOffKey;
    stKey TvSoureKey;
    stKey StbMenuKey;
    stKey StbMenuConfirmeKey;
    stKey StbMenuBackKey;
    stKey upKey;
    stKey downKey;
    stKey leftKey;
    stKey rightKey;
    stKey PrevProgKey;
    stKey NextProgKey;
    stKey TvVolUpKey;
    stKey TvVolDownKey;
    stKey MuteKey;
}stTvKey;

stDevice* get_tv_device(void);

#ifdef __cplusplus
}
#endif

#endif //DEVICE_SUPPORT_TV
#endif /*__CI100X_ASRCMD_H*/
