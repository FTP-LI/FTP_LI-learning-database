/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-04-25 11:06:33
 * @LastEditTime: 2023-05-12 18:10:37
 * @Description: 
 */
#ifndef __BLE_USER_CONFIG_H__
#define __BLE_USER_CONFIG_H__

#include "user_config.h"
#include "sdk_default_config.h"
//主设备ID定义
#define DEV_IR_CONTROL_MAIN_ID               0x01   //红外遥控器
#define DEV_AIRCONDITION_MAIN_ID             0X02   //空调
#define DEV_LIGHT_CONTROL_MAIN_ID            0x03   //灯控
#define DEV_SOUND_MAIN_ID                    0x04   //音响设备
#define DEV_TEA_BAR_MAIN_ID                  0x05   //茶吧机
#define DEV_FAN_MAIN_ID                      0x06   //风扇
#define DEV_HEATTABLE_MAIN_ID                0x07   //取暖桌
#define DEV_WARMER_MAIN_ID                   0x08   //取暖器
#define DEV_WATERHEATED_MAIN_ID              0x09   //水暖毯
//次设备ID定义
#define DEV_LIGHT_CONTROL_ONE_SUB_ID         0x01  //一路灯
#define DEV_LIGHT_CONTROL_THREE_SUB_ID       0x02  //三路灯
#define DEV_LIGHT_CONTROL_FOUR_SUB_ID        0x03  //四路灯
#define DEV_LIGHT_CONTROL_FIVE_SUB_ID        0x04  //五路灯
#define DEV_LIGHT_CONTROL_LAMP_SUB_ID        0x05  //灯带
#define DEV_LIGHT_CONTROL_SPOT_SUB_ID        0x06  //射灯
#define DEV_LIGHT_CONTROL_RGB_SUB_ID         0x07  //RGB灯

#if(DEV_DRIVER_EN_ID == DEV_AIRCONDITION_MAIN_ID)    
    #define DEV_TYPE_ID       DEV_AIRCONDITION_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0  
#elif(DEV_DRIVER_EN_ID == DEV_LIGHT_CONTROL_MAIN_ID)               
    #define DEV_TYPE_ID        DEV_LIGHT_CONTROL_MAIN_ID
    #define DEV_NUMBER_ID      DEV_LIGHT_CONTROL_RGB_SUB_ID
    #define CONFIG_TYPE       0
    #if CIAS_BLE_ADV_GROUP_MODE_ENABEL
    #define CONFIG_TYPE       4     //小程序区别广播设备
    #endif
#elif (DEV_DRIVER_EN_ID == DEV_TEA_BAR_MAIN_ID)  
    #define DEV_TYPE_ID       DEV_TEA_BAR_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_FAN_MAIN_ID)  
    #define DEV_TYPE_ID       DEV_FAN_MAIN_ID
    #define DEV_NUMBER_ID     1 
    #define CONFIG_TYPE       0   
#elif(DEV_DRIVER_EN_ID == DEV_HEATTABLE_MAIN_ID)
    #define DEV_TYPE_ID       DEV_HEATTABLE_MAIN_ID
    #define DEV_NUMBER_ID     1   
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_WARMER_MAIN_ID)
    #define DEV_TYPE_ID       DEV_WARMER_MAIN_ID
    #define DEV_NUMBER_ID     1 
    #define CONFIG_TYPE       0 
#elif(DEV_DRIVER_EN_ID == DEV_WATERHEATED_MAIN_ID)
    #define DEV_TYPE_ID       DEV_WATERHEATED_MAIN_ID
    #define DEV_NUMBER_ID     1 
    #define CONFIG_TYPE       0 
#endif


//服务定义-如需修改该下面三个值，小程序也要做同步修改(不可用0x2900之前的值不可用)
#define BLE_UUID_CIAS_SERVICE              0XAE3A   
#define BLE_UUID_CIAS_WRITE                0XAE3B     
#define BLE_UUID_CIAS_NOTIFY               0XAE3C 


#endif