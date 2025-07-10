/**
  ******************************************************************************
  * @文件    fan_asrcmd.h
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
#ifndef _FAN_DEVICE_H
#define _FAN_DEVICE_H

#include "device.h"
#ifdef DEVICE_SUPPORT_FAN

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    FAN_ASR_MATCH_FAN = 3000,
    FAN_ASR_CLEAN_CODE,
    FAN_ASR_OPEN_FAN,
    FAN_ASR_CLOSE_FAN,
    FAN_ASR_START_SHAKE,
    FAN_ASR_STOP_SHAKE,
    FAN_ASR_SWITCH_SPEED,
    FAN_ASR_FAN_TIMER,
    FAN_ASR_SPEED_1,
    FAN_ASR_SPEED_2,
    FAN_ASR_SPEED_3,

    FAN_VOICE_POWER_ON = FAN_ASR_MATCH_FAN + 200,
    FAN_VOICE_POWER_OFF,
    FAN_VOICE_SHAK,
    FAN_VOICE_STOP_SHAK,
    FAN_VOICE_SPEED,
    FAN_VOICE_TIME,
    FAN_VOICE_SPEED_1,
    FAN_VOICE_SPEED_2,
    FAN_VOICE_SPEED_3,

    FAN_VOICE_MATCH_FAN_FIRST = FAN_ASR_MATCH_FAN + 250,
    FAN_VOICE_OPEN_FAN_FIRST,
    FAN_ASR_MAX,
}eFanASRIndex;

typedef struct
{
    stKey PowerOnKey;
    stKey PowerOffKey;
    stKey ShakKey;
    stKey StopShakKey;
    stKey SpeedKey;
    stKey TimeKey;
    stKey SpeedKey1;
    stKey SpeedKey2;
    stKey SpeedKey3;
}stFanKey;

stDevice* get_fan_device(void);

#ifdef __cplusplus
}
#endif

#endif //DEVICE_SUPPORT_FAN

#endif /*__CI100X_ASRCMD_H*/
