/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-02-24 09:36:43
 * @LastEditTime: 2023-04-06 18:27:08
 * @Description: 
 */
/*============================================================================*/
/* @file exe_app.h
 * @brief App configurations defined by user.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_APP_H__
#define __EXE_APP_H__

#include "ble_gap.h"

/**
 * @brief 产品的品类Appearance.
 *        拍照器，键盘  KEYBOARD
 *        鼠标          MOUSE
 *        串口          METER
 */
#if !defined(__EC616)
//#define APP_GAP_APPEARANCE              BLE_APPEARANCE_HID_KEYBOARD
#define APP_GAP_APPEARANCE              BLE_APPEARANCE_HID_KEYBOARD//BLE_APPEARANCE_HID_MOUSE
#else
#define APP_GAP_APPEARANCE              BLE_APPEARANCE_GENERIC_THERMOMETER
#endif

#if defined(HM1001_M0)
///真实扫描鼠标
#define APP_MOUSE_AUTO_CIRCLE           0
#include "hal_mouse_bsp.h"

#else
///模拟自动画圆
#define APP_MOUSE_AUTO_CIRCLE           1
#endif

///用户定义的设备状态。
typedef enum {
  POWER_ON_DEVICE_STATUS,
  DEEP_SLEEP_DEVICE_STATUS,
  HIGH_ADV_DEVICE_STATUS,
  LOW_ADV_DEVICE_STATUS,
  CONNECTED_DEVICE_STATUS,
  AFTER_CONNECTED_DEVICE_STATUS,
  KEY_PRESS_DEVICE_STATUS,
} app_device_status_t;

/**
 * @brief 扫描输入硬件，得到HID数据。将作为notification数据被发送到master。
 * @note  为拍照器方案实现该扫描函数。
 */
void app_scan_hwinput(void);

/**
 * @breif 生成鼠标报告数据，将作为notification数据被发送到master。
 *        要么是真实扫描鼠标button/sensor/wheel得到的数据(APP_MOUSE_AUTO_CIRCLE=0);
 *        要么是模拟自动画圆得到的数据(APP_MOUSE_AUTO_CIRCLE=1).
 *        
 * @note  在主循环中轮询调用。
 */
void app_mouse_scan(void);

/**
 * @brief 尝试把各种要发送的HID notification数据，放入发送队列。
 * @note 在主循环中轮询调用。
 */

#endif /* #ifndef __EXE_APP_H__ */
