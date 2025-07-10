/**
 * @file es7243e.h
 * @brief 
 * @version 0.1
 * @date 2019-03-28
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */


#ifndef __ES7243E_H
#define __ES7243E_H

#include <stdint.h>
#include <stdbool.h>
#include "ci130x_system.h"


/**
 * @ingroup third_device_driver
 * @defgroup es7243e es7243e
 * @brief es7243e的驱动
 */

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @addtogroup es7243e
 * @{
 */


/**
 * @brief 7243e ALC目标幅值（dB为单位）
 * 
 */
typedef enum
{
    ES7243E_ALC_LEVEL__30dB = 0,
    ES7243E_ALC_LEVEL__27dB,
    ES7243E_ALC_LEVEL__24dB,
    ES7243E_ALC_LEVEL__22dB,
    ES7243E_ALC_LEVEL__19dB,
    ES7243E_ALC_LEVEL__17dB ,
    ES7243E_ALC_LEVEL__15dB,
    ES7243E_ALC_LEVEL__13_5dB,
    ES7243E_ALC_LEVEL__12dB,
    ES7243E_ALC_LEVEL__10_5dB,
    ES7243E_ALC_LEVEL__9dB,
    ES7243E_ALC_LEVEL__7_5dB,
    ES7243E_ALC_LEVEL__6dB,
    ES7243E_ALC_LEVEL__4_5dB,
    ES7243E_ALC_LEVEL__3dB,
    ES7243E_ALC_LEVEL__1_5dB,
}es7243e_alc_level_t;//ALC调节声音的目标幅度


typedef enum
{
    ES7243E_RAMPRATE_80US = 0,
    ES7243E_RAMPRATE_160US,
    ES7243E_RAMPRATE_320US,
    ES7243E_RAMPRATE_640US,
    ES7243E_RAMPRATE_1_28MS,
    ES7243E_RAMPRATE_2_56MS,
    ES7243E_RAMPRATE_5_12MS,
    ES7243E_RAMPRATE_10_24MS,
    ES7243E_RAMPRATE_20_48MS,
    ES7243E_RAMPRATE_40_96MS,
    ES7243E_RAMPRATE_81_92MS,
    ES7243E_RAMPRATE_163_84MS,
    ES7243E_RAMPRATE_327_68MS,
    ES7243E_RAMPRATE_655_36MS,
    ES7243E_RAMPRATE_1_3S,
}es7243e_vc_ramprate_t;


typedef enum
{
    ES7243E_PGA_GAIN_0dB = 0,
    ES7243E_PGA_GAIN_3dB,
    ES7243E_PGA_GAIN_6dB,
    ES7243E_PGA_GAIN_9dB,
    ES7243E_PGA_GAIN_12dB,
    ES7243E_PGA_GAIN_15dB,
    ES7243E_PGA_GAIN_18dB,
    ES7243E_PGA_GAIN_21dB,
    ES7243E_PGA_GAIN_24dB,
    ES7243E_PGA_GAIN_27dB,
    ES7243E_PGA_GAIN_30dB,
    ES7243E_PGA_GAIN_33dB,
    ES7243E_PGA_GAIN_34_5dB,
    ES7243E_PGA_GAIN_36dB,
    ES7243E_PGA_GAIN_37_5dB,
}es7243e_pga_gain_t;


typedef enum
{
    ES7243E_ADC_GAIN_SCALE_UP_0dB = 0,
    ES7243E_ADC_GAIN_SCALE_UP_6dB,
    ES7243E_ADC_GAIN_SCALE_UP_12dB,
    ES7243E_ADC_GAIN_SCALE_UP_18dB,
    ES7243E_ADC_GAIN_SCALE_UP_24dB,
    ES7243E_ADC_GAIN_SCALE_UP_30dB,
    ES7243E_ADC_GAIN_SCALE_UP_36dB,
    ES7243E_ADC_GAIN_SCALE_UP_42dB,
}es7243e_adc_gain_scale_up_t;


/**
 * @brief 8311 ALC通道选择
 * 
 */
typedef enum
{
    //!8311 ALC关闭
    ES7243E_ALC_OFF  = (0<<6),
    //!8311 ALC 只开启右通道
    ES7243E_ALC_ON   = (1<<6),
}es7243e_alc_gate_t;


/**
 * @brief 8311 ALC配置
 * 
 */
typedef struct 
{
    float max_gain;
    es7243e_alc_level_t alc_level;           
}es7243e_alc_config_t;


typedef enum
{
    ES7243E_AUTO_MUTE_WS_40US = 0,
    ES7243E_AUTO_MUTE_WS_60US,
    ES7243E_AUTO_MUTE_WS_120US,
    ES7243E_AUTO_MUTE_WS_240US,
    ES7243E_AUTO_MUTE_WS_480US,
    ES7243E_AUTO_MUTE_WS_1MS,
    ES7243E_AUTO_MUTE_WS_3MS,
    ES7243E_AUTO_MUTE_WS_6MS,
    ES7243E_AUTO_MUTE_WS_9MS,
    ES7243E_AUTO_MUTE_WS_15MS,
    ES7243E_AUTO_MUTE_WS_30MS,
    ES7243E_AUTO_MUTE_WS_60MS,
}es7243e_auto_mute_ws_t;


typedef enum
{
    ES7243E_AUTOMUTE_NG__96dB = 0,
    ES7243E_AUTOMUTE_NG__90dB,
    ES7243E_AUTOMUTE_NG__84dB,
    ES7243E_AUTOMUTE_NG__78dB,
    ES7243E_AUTOMUTE_NG__72dB,
    ES7243E_AUTOMUTE_NG__66dB,
    ES7243E_AUTOMUTE_NG__60dB,
    ES7243E_AUTOMUTE_NG__54dB,
}es7243e_automute_ng_t;


typedef enum
{
    ES7243E_AUTO_MUTE_DISABLE = 0,
    ES7243E_AUTO_MUTE_ENABLE = 1,
}es7243E_auto_mute_en_t;


// typedef struct
// {
//     es8311_auto_mute_en_t auto_mute_en;
//     es8311_automute_ng_t ng;
//     es8311_automute_vol_t vol;
//     es8311_auto_mute_ws_t ws;
// }es8311_auto_mute_config_t;

void es7243e_init(bool is_linein);
void es7243e_pga_gain_l(es7243e_pga_gain_t gain);
void es7243e_pga_gain_r(es7243e_pga_gain_t gain);
int8_t es7243e_alc_gate_en(es7243e_alc_gate_t ALC_gate);
void es7243e_alc_maxgain_set(float gain);
void es7243e_exit_lowpower_mode(void);
void es7243e_enter_lowpower_mode(void);
int8_t es7243e_ramprate_cfg(es7243e_vc_ramprate_t ramp);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
/***************** (C) COPYRIGHT Chipintelli Technology Co., Ltd. *****END OF FILE****/

