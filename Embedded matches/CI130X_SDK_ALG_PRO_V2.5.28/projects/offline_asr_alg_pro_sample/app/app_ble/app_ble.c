/**
 * @file main.c
 * @brief 示例程序
 * @version 1.0.0
 * @date 2021-03-19
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */
#include <stdio.h> 
#include <malloc.h>
#include "FreeRTOS.h" 
#include "task.h"
#include "sdk_default_config.h"
#include "ci130x_core_eclic.h"
#include "ci130x_spiflash.h"
#include "ci130x_gpio.h"
#include "board.h"
#include "ci_log.h"
#include "user_config.h"
#include "sdk_default_config.h"
#if CIAS_BLE_CONNECT_MODE_ENABLE||CIAS_BLE_ADV_GROUP_MODE_ENABEL
#include "exe_app.h"
#include "exe_api.h"
#include "exe_adv_init.h"
#include "ci_ble_spi.h"
#include "rf_msg_deal.h"
#include "ble_user_config.h"
#endif
#if RGB_DRIVER_EN
#include "cias_rgb_driver.h"
#endif

extern TaskHandle_t ble_task_handle;
void ble_model_init()
{

#if CIAS_BLE_CONNECT_MODE_ENABLE                                   //BLE连接方案使能
#if CIAS_BLE_USE_DEFAULT_ADV_DATA
    user_dev_init(DEV_TYPE_ID, DEV_NUMBER_ID, CONFIG_TYPE);       //设置服务信息
    user_ble_name_init();                                         //初始蓝牙广播名称
#else
    uint8_t adv_data[28] = {0};
    if(!ble_adv_data_init(adv_data, 28))
        mprintf("ble_adv_data_init error!!!!!!!!!!!!!\r\n");;
#endif  
    /*蓝牙BLE任务*/
    //注册射频接收函数，射频发送函数默认已经注册，调用rf_cb_funcs.rf_send(), \
    用户不使用启英物联小程序，请注册custom_rf_recv_data_handle函数自行处理接收消息*/
    dev_state_init(); //初始化设备功能状态
    xTaskCreate(ci_ble_recv_task,  "ci_ble_recv_task",480,NULL,4,NULL); 
    register_rf_callback(ci_rf_recv_data_handle); 
    ble_uuid_init(BLE_UUID_CIAS_SERVICE, BLE_UUID_CIAS_WRITE, BLE_UUID_CIAS_NOTIFY);
    xTaskCreate(ble_exec_loop,"ble_exec_loop",1024*2, NULL, 5, &ble_task_handle);
    init_connect_timer();
    //xTaskCreate(get_codec_mode_task,"get_codec_mode_task",100,NULL,4,NULL);
#endif //CIAS_BLE_CONNECT_MODE_ENABLE

#if  CIAS_BLE_ADV_GROUP_MODE_ENABEL
    dev_state_init(); //初始化设备功能状态
    user_dev_adv_init(DEV_TYPE_ID, DEV_NUMBER_ID, CONFIG_TYPE);   //设置对外广播信息

    xTaskCreate(ci_ble_recv_task,  "ci_ble_recv_task",480,NULL,4,NULL); 
    xTaskCreate(ble_adv_scan,"ble_adv_scan",1024, NULL, 3, &ble_task_handle);
#endif

#if CIAS_BLE_AT_ENABLE   
    xTaskCreate(product_recv_task, "product_recv_task", 480, NULL, 4, NULL);
    xTaskCreate(at_msg_task, "at_msg_task",100,NULL,3,NULL);
#endif
    
#if RGB_DRIVER_EN
    xTaskCreate(rgb_music_mode_task,"rgb_music_mode_task",256,NULL,4,NULL);
#endif//RGB_DRIVER_EN

}

void ble_xtal_init()
{
    #if EXTERNAL_CRYSTAL_OSC_FROM_RF
    if(ble_port_init())
    {
        //vTaskDelay(pdMS_TO_TICKS(100));
        extern void board_clk_source_set(void);
        board_clk_source_set();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    else
    {
        hal_dump_rf_register();
        vTaskDelay(pdMS_TO_TICKS(100));
        dpmu_software_reset_system_config();
    }
    #endif
}