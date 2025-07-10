/**
  ******************************************************************************
  * @文件    light_device.h
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
#ifndef _LIGHT_DEVICE_H
#define _LIGHT_DEVICE_H

#include "device.h"
#ifdef DEVICE_SUPPORT_LIGHT

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LIGHT_ASR_MATCH_LIGHT = 4000,
    LIGHT_ASR_CLEAN_CODE,
    LIGHT_ASR_OPEN_LIGHT,
    LIGHT_ASR_CLOSE_LIGHT,
    LIGHT_ASR_BRIGHT_LIGHT,
    LIGHT_ASR_DARK_LIGHT,
    LIGHT_ASR_SWITCH_LIGHT,

    LIGHT_VOICE_POWER_ON = LIGHT_ASR_MATCH_LIGHT + 200,
    LIGHT_VOICE_POWER_OFF,
    LIGHT_VOICE_BRIGHT,
    LIGHT_VOICE_DARK,
    LIGHT_VOICE_SWITCH_LIGHT,

    LIGHT_VOICE_MATCH_FAN_FIRST = LIGHT_ASR_MATCH_LIGHT + 250,
    LIGHT_VOICE_OPEN_FAN_FIRST,
    LIGHT_ASR_MAX,
}eLightASRIndex;

typedef struct
{
    stKey PowerOnKey;
    stKey PowerOffKey;
    stKey BrightKey;
    stKey DarkKey;
    stKey SwitchKey;
}stLightKey;

stDevice* get_light_device(void);

#ifdef __cplusplus
}
#endif

#endif // DEVICE_SUPPORT_LIGHT

#endif /*__CI100X_ASRCMD_H*/
