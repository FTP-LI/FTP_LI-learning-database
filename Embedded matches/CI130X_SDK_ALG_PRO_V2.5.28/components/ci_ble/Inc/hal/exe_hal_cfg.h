/*============================================================================*/
/* @file exe_hal_cfg.h
 * @brief EXE HAL configuration data in storage header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_CFG_H__
#define __EXE_HAL_CFG_H__

#include <stdint.h>
#include <stdbool.h>

#if defined(HM1001_M0)
#define HAL_STORAGE_CFG_SIZE            28
#else
#define HAL_STORAGE_CFG_SIZE            96
#endif

#if defined(HS6601) || defined(HS6601c)
#define HAL_STORAGE_CFG_BASE            (0x80000000 + 0x9000)
#else
#define HAL_STORAGE_CFG_BASE            (16384 - HAL_STORAGE_CFG_SIZE)
#endif

/* the fixed data format in ADV_IND.AdvData[]. */
#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
#define EXE_ADV_DEV_NAME_OFF  (21+7-7)
#define EXE_ADV_DEV_NAME_LEN  (18-7)
#else
#define EXE_ADV_DEV_NAME_OFF  21
#define EXE_ADV_DEV_NAME_LEN  18
#endif

/* The maximum size of device name in cfg area. */
#define CFG_DEV_NAME_LEN_MAX  64

typedef struct {
#if defined(HM1001_M0)
  ///电池电量为空(如2.0V，此处值为20），用于强制停止使用，防止损坏电池和设备
  uint8_t bat_empty_value;
  ///电池电量欠压(如2.2V，此处值为22），用于欠压报警
  uint8_t bat_near_empty_value;
  ///电池电量满格电压(如锂电池4.2V，此处为42，纽扣电池3.0V，此处30)
  uint8_t bat_full_value;
  ///用户不能配置蓝牙设备名。
  char     dev_name[0];
  ///频偏配置参数，用于同一固件不同晶体时。
  uint8_t  freq_off;
  ///预留以按4-byte对齐。
  uint8_t  rsv[3];
  ///使用CP测试时间作为蓝牙设备地址，为节省空间，只用4Byte
  uint8_t  bda[4];
  ///鸿博定义的power, rc, gpadc的校准参数
  uint32_t rchf_rclf_trim;
  uint32_t pwr_trim;
  uint32_t rc_trim;
  uint32_t adc_int_bias;
  uint32_t adc_ext_bias;

#else
  ///用户配置的蓝牙设备名，从后往前找第一个非0就是名称开始。允许多次更改，前面的覆盖写成0即可。
  char     dev_name[CFG_DEV_NAME_LEN_MAX]; 
  uint8_t  freq_off;
  uint8_t  bda[6];
  uint8_t  rsv[HAL_STORAGE_CFG_SIZE - 7 - CFG_DEV_NAME_LEN_MAX - 16];
#endif
} hal_storage_cfg_t;

void hal_cfg_set_bat_value(void);
void hal_cfg_set_freq_offset(uint8_t freq_off_cw);
void hal_rf_init(void);
void hal_cfg_set_rx_gain(void);
void hal_cfg_set_bda(uint8_t *p_pda);
void hal_cfg_set_devname(uint8_t *p_devname);

#endif /* #ifndef __EXE_HAL_CFG_H__ */

