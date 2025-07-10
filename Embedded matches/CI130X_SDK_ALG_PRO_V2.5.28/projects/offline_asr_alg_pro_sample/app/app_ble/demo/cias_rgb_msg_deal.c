#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "ci130x_dpmu.h"
#include "rf_msg_deal.h"
#include "system_msg_deal.h"
#include "user_config.h"
#include "sdk_default_config.h"
#include "crc.h"
#include "exe_api.h"
#include "cias_rgb_msg_deal.h"

rgb_dev_t rgb_dev;
uint32_t rgb_timer_counter = 0;//定时器秒数
xTimerHandle rgb_timer = NULL; //定时器
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数

static rgb_timer_callback()
{
    if (rgb_timer_counter)
    {
        rgb_timer_counter --;
        mprintf("定时剩余：%d S\r\n",rgb_timer_counter);
    }
    else
    {
        xTimerStop(rgb_timer,0);
        rgb_dev.timing = 0; 
        mprintf("定时时间到\r\n");
    }
}

void rgb_init()
{
    rgb_dev.power = FUN_TURN_OFF;
    rgb_dev.brightness = DEV_BRIGHTNESS_MID;
    rgb_dev.red_value = 0x00;
    rgb_dev.green_value = 0x00;
    rgb_dev.blue_value = 0x00;
    rgb_dev.e_value = 0x00;
    rgb_dev.s_value = 0x00;
    rgb_dev.property_mode = 0X00;
    rgb_dev.timing;
    rgb_timer = xTimerCreate("rgb_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, rgb_timer_callback);
}

void set_rgb_timing(uint8_t timing)
{
    if ((timing >= 0xA1)&&(timing <= 0xAD))
    {
        prompt_play_by_cmd_id(timing-0xA1+TIMMING_1H, -1, default_play_done_callback,true);
        uart_send_asr(timing-0xA1+TIMMING_1H);
        rgb_dev.timing = timing ; 
        
        if(rgb_dev.timing == 0xAD)      //定时半小时
        {
            rgb_timer_counter = 1800;
        }
        else    //定时一小时~十二小时
        {
            rgb_timer_counter = (rgb_dev.timing-0xA0)*3600;
        }

        xTimerStop(rgb_timer, 0);
        xTimerStart(rgb_timer, 0);
    }
}

uint8_t rgb_report(uint16_t cmd_id)
{
#if CIAS_BLE_CONNECT_MODE_ENABLE
    bool send_flag = true;
#else 
      bool send_flag = false;
#endif
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = RGB_DEV;
    send_data[5] = 0x07;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    if(TURN_ON <= cmd_id <= RGB_MUSIC_MODE)
        rgb_dev.power = FUN_TURN_ON;
    switch (cmd_id)
    {
        case TURN_OFF://关闭灯光
        {
            send_data[7] = RGB_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            rgb_dev.power = FUN_TURN_OFF;
            break;
        }
        case TURN_ON://打开灯光
        {
            send_data[7] = RGB_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            break;
        }
        case RGB_BRIGHTNESS_UP://调亮一点
        {
            rgb_dev.brightness += 20;
            if(rgb_dev.brightness > 100)
            {
                rgb_dev.brightness = 100;
                mprintf("已经是最高亮度\r\n");
            }
            send_data[7] = RGB_BRIGHTNESS;
            send_data[10] = rgb_dev.brightness;//数据
            break;
        }
        case RGB_BRIGHTNESS_DOWN://调暗一点
        {
            rgb_dev.brightness -= 20;
            if(rgb_dev.brightness < 10)
            {
                rgb_dev.brightness = 10;
                mprintf("已经是最低亮度\r\n");
            }
            send_data[7] = RGB_BRIGHTNESS;
            send_data[10] = rgb_dev.brightness;//数据
            break;
        }
        case RGB_BRIGHTNESS_MIX://最高亮度
        {
            rgb_dev.brightness = DEV_BRIGHTNESS_MAX;
            send_data[7] = RGB_BRIGHTNESS;
            send_data[10] = rgb_dev.brightness;//数据
            break;
        }
        case RGB_BRIGHTNESS_MIN://最低亮度
        {
            rgb_dev.brightness = DEV_BRIGHTNESS_MIN;
            send_data[7] = RGB_BRIGHTNESS;
            send_data[10] = rgb_dev.brightness;//数据
            break;
        }
        case RGB_BRIGHTNESS_MID://中等亮度
        {
            rgb_dev.brightness = DEV_BRIGHTNESS_MID;
            send_data[7] = RGB_BRIGHTNESS;
            send_data[10] = rgb_dev.brightness;//数据
            break;
        }

        case RGB_WHITE_COLOR ... RGB_MUSIC_MODE://
        {
            rgb_dev.property_mode = cmd_id-RGB_WHITE_COLOR+1;
            send_data[7] = RGB_PROPERTY_MODE;
            send_data[9] = 0x06;
            send_data[10] = rgb_dev.red_value;//数据
            send_data[11] = rgb_dev.green_value;//数据
            send_data[12] = rgb_dev.blue_value;//数据
            send_data[13] = rgb_dev.e_value;//数据
            send_data[14] = rgb_dev.s_value;//数据
            send_data[15] = rgb_dev.property_mode;//数据
            break;
        }
        case TIMMING_1H ... TIMMING_12H://定时X小时
        {
            send_data[7] = RGB_TIMING;
            send_data[10] = (cmd_id-TIMMING_1H) + 0xA1;//数据
            rgb_dev.timing = send_data[10];
            rgb_timer_counter = (rgb_dev.timing - 0xA0)*3600;
            xTimerStop(rgb_timer, 0);
            xTimerStart(rgb_timer, 0);
            break;
        }
        case TIMMING_HARF://定时半小时
        {
            send_data[7] = RGB_TIMING;
            send_data[10] = 0xAD;//数据
            rgb_dev.timing = 0xAD;
            rgb_timer_counter = 1800;
            xTimerStop(rgb_timer, 0);
            xTimerStart(rgb_timer, 0);
            break;
        }
        default:
            send_flag = false;
            break;
    }
    if (send_flag)
    {
        uint16_t crc_cal = crc16_ccitt(0, send_data, send_data[8]*16+send_data[9]+10);
        mprintf("crc = 0x%x\r\n",crc_cal);
        send_data[send_data[8]*16+send_data[9]+10] = crc_cal>>8;
        send_data[send_data[8]*16+send_data[9]+11] = crc_cal&0xFF;
        
        len = send_data[8]*16+send_data[9]+12;
        cias_crypto_data(send_data, len); 
        rf_cb_funcs.rf_send(send_data, len);
    }
}


void rgb_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;
    switch (msg.function_id)
    {
        case RGB_POWER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                prompt_play_by_cmd_id(TURN_OFF,-1,default_play_done_callback,true);      //开灯
                uart_send_asr(TURN_ON);
            }
            else if(msg.data[0] == FUN_TURN_ON)
            {
                prompt_play_by_cmd_id(TURN_ON,-1,default_play_done_callback,true);     //关灯
                uart_send_asr(TURN_OFF);
            }
            rgb_dev.power = msg.data[0];
            break;
        }
        case RGB_BRIGHTNESS:
        {

            if(DEV_BRIGHTNESS_MAX == msg.data[0])
            {
                prompt_play_by_cmd_id(RGB_BRIGHTNESS_MIX,-1,default_play_done_callback,true);       //最高亮度
                uart_send_asr(RGB_BRIGHTNESS_MIX);
            }
            else if(DEV_BRIGHTNESS_MIN == msg.data[0])
            {
                prompt_play_by_cmd_id(RGB_BRIGHTNESS_MIN,-1,default_play_done_callback,true);       //最低亮度
                uart_send_asr(RGB_BRIGHTNESS_MIN);
            }
            else if(DEV_BRIGHTNESS_MID == msg.data[0]){
                prompt_play_by_cmd_id(RGB_BRIGHTNESS_MID,-1,default_play_done_callback,true);       //中等亮度
                uart_send_asr(RGB_BRIGHTNESS_MID);
            }
            else if(rgb_dev.brightness < msg.data[0])
            {
                prompt_play_by_cmd_id(RGB_BRIGHTNESS_UP,-1,default_play_done_callback,true);        //调亮一点
                uart_send_asr(RGB_BRIGHTNESS_UP);
            }
            else if(rgb_dev.brightness > msg.data[0])
            {
                prompt_play_by_cmd_id(RGB_BRIGHTNESS_DOWN,-1,default_play_done_callback,true);      //调暗一点
                uart_send_asr(RGB_BRIGHTNESS_DOWN);
            }
            rgb_dev.brightness = msg.data[0];
            rgb_dev.power = FUN_TURN_ON;
            break;
        }
        case RGB_PROPERTY_MODE:
        {
            if (msg.data[5] == 1){
                prompt_play_by_cmd_id(RGB_WHITE_COLOR,-1,default_play_done_callback,true);   //白色灯光
                uart_send_asr(RGB_WHITE_COLOR);
            }
            else if(msg.data[5] == 2){
                prompt_play_by_cmd_id(RGB_ORANGE_COLOR,-1,default_play_done_callback,true);   //橙色灯光
                uart_send_asr(RGB_ORANGE_COLOR);
            }
            else if(msg.data[5] == 3){
                prompt_play_by_cmd_id(RGB_RED_COLOR,-1,default_play_done_callback,true);   //红色灯光
                uart_send_asr(RGB_RED_COLOR);
            }
            else if(msg.data[5] == 4){
                prompt_play_by_cmd_id(RGB_PURPLE_COLOR,-1,default_play_done_callback,true);   //紫色灯光
                uart_send_asr(RGB_PURPLE_COLOR);
            }
            else if(msg.data[5] == 5){
                prompt_play_by_cmd_id(RGB_BLUE_COLOR,-1,default_play_done_callback,true);   //蓝色灯光
                uart_send_asr(RGB_BLUE_COLOR);
            }
            else if(msg.data[5] == 6){
                prompt_play_by_cmd_id(RGB_GREEN_COLOR,-1,default_play_done_callback,true);   //绿色灯光
                uart_send_asr(RGB_GREEN_COLOR);
            }
            else if(msg.data[5] == 7){
                prompt_play_by_cmd_id(RGB_YELLOW_COLOR,-1,default_play_done_callback,true);   //黄色灯光
                uart_send_asr(RGB_YELLOW_COLOR);
            }
            else if(msg.data[5] == 8){
                prompt_play_by_cmd_id(RGB_MUSIC_MODE,-1,default_play_done_callback,true);   //音乐灯光
                uart_send_asr(RGB_MUSIC_MODE);
            }
            rgb_dev.property_mode = msg.data[5];
            rgb_dev.power = FUN_TURN_ON;
            break;
        }
        case RGB_TIMING:       //定时功能
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭定时\r\n");
                prompt_play_by_cmd_id(TURN_OFF_TIMING, -1, default_play_done_callback,true);   
                uart_send_asr(TURN_OFF_TIMING);   
                rgb_dev.timing = 0;   
                xTimerStop(rgb_timer,0);
            }
            else
            {
                set_rgb_timing(msg.data[0]);              
            }
            break;
        }
        default:
            break;
        
    }
}


void rgb_query(ble_msg_V1_t msg)
{
    if ((msg.function_id < 0x01)||(msg.function_id > 0x05))
    {
        mprintf("无效查询指令");
        return;
    }

    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = RGB_DEV;
    send_data[5] = 0x07;
    send_data[6] = 0x41;
    send_data[7] = msg.function_id;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    
    uint8_t *data = (uint8_t *)&rgb_dev;
/*     for (size_t i = 0; i < 14; i++)
    {
        mprintf("%x ",data[i]);
    } */
    if(msg.function_id == RGB_PROPERTY_MODE)
    {
        send_data[9] = 0x06;
        send_data[10] = rgb_dev.red_value;
        send_data[11] = rgb_dev.green_value;
        send_data[12] = rgb_dev.blue_value;
        send_data[13] = rgb_dev.e_value;
        send_data[14] = rgb_dev.s_value;
        send_data[15] = rgb_dev.property_mode;
    }
    else
    {
        send_data[10] = data[msg.function_id-1];
    }
    uint16_t crc_cal = crc16_ccitt(0, send_data, send_data[8]*16+send_data[9]+10);
    mprintf("crc = 0x%x\r\n",crc_cal);
    send_data[send_data[8]*16+send_data[9]+10] = crc_cal>>8;
    send_data[send_data[8]*16+send_data[9]+11] = crc_cal&0xFF;
    len = send_data[8]*16+send_data[9]+12;
    cias_crypto_data(send_data, len); 
    rf_cb_funcs.rf_send(send_data, len);
}