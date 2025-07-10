#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "ci130x_dpmu.h"
#include "rf_msg_deal.h"
#include "system_msg_deal.h"
#include "user_config.h"
#include "sdk_default_config.h"
#include "rf_msg_deal.h"
#include "crc.h"
#include "exe_api.h"
#include "cias_aircondition_msg_deal.h"

uint32_t aircondition_timer_counter = 0;//定时器秒数
xTimerHandle aircondition_timer = NULL; //定时器
aircondition_dev_t aircondition_dev;
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数

static aircondition_timer_callback()
{
    if (aircondition_timer_counter)
    {
        aircondition_timer_counter --;
        mprintf("定时剩余：%d S\r\n",aircondition_timer_counter);
    }
    else
    {
        xTimerStop(aircondition_timer,0);
        aircondition_dev.timing = 0; 
        mprintf("定时时间到\r\n");
    }
}

void set_aircondition_timing(uint8_t timing)
{
    aircondition_timer_counter = timing*3600;
    xTimerStop(aircondition_timer, 0);
    xTimerStart(aircondition_timer, 0);
}

void aircondition_init()
{
    aircondition_dev.power = FUN_TURN_OFF;
    aircondition_dev.mode = FUN_TURN_OFF;
    aircondition_dev.swing = FUN_TURN_OFF;
    aircondition_dev.tempreature = DEV_TEMPERATURE_MIN;
    aircondition_dev.light = FUN_TURN_OFF;
    aircondition_dev.health = FUN_TURN_OFF;
    aircondition_dev.sleep = FUN_TURN_OFF;
    aircondition_dev.speed = FUN_TURN_OFF;
    aircondition_dev.strong = FUN_TURN_OFF;
    aircondition_dev.swing = FUN_TURN_OFF;
    aircondition_timer = xTimerCreate("aircondition_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, aircondition_timer_callback);
}
uint8_t aircondition_report(uint16_t cmd_id)
{
    bool send_flag = true;
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = AIRCONDITION_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    switch (cmd_id)
    {
        case TURN_OFF:
        {
            send_data[7] = AIRCONDITION_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            aircondition_dev.power = FUN_TURN_OFF;
            break;
        }
        case TURN_ON:
        {
            send_data[7] = AIRCONDITION_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            aircondition_dev.power = FUN_TURN_ON;
            break;
        }
        case MODE_COOL ... MODE_AUTO://模式
        {
            send_data[7] = AIRCONDITION_MODE;
            send_data[10] = cmd_id-MODE_COOL+1;//数据
            aircondition_dev.mode = send_data[10];
            break;
        }
        case SWING_LR ... SWING_HD://风速
        {
            send_data[7] = AIRCONDITION_SWING;
            send_data[10] = (cmd_id-SWING_LR+1)*2;//数据
            aircondition_dev.swing = send_data[10];
            break;
        }
        case SWING_AUTO ... SWING_HIGH://风速
        {
            send_data[7] = AIRCONDITION_SPEED;
            send_data[10] = cmd_id-SWING_AUTO+1;//数据
            aircondition_dev.speed = send_data[10];
            break;
        }
        case SWING_VERY_HIGH://强劲风
        {
            send_data[7] = AIRCONDITION_STRONG;
            send_data[10] = FUN_TURN_ON;//数据
            aircondition_dev.strong = send_data[10];
            break;
        }
        case TEMPERATURE_16 ... TEMPERATURE_30://温度
        {
            send_data[7] = AIRCONDITION_TEMPERATURE;
            send_data[10] = cmd_id-TEMPERATURE_16+16;//数据
            aircondition_dev.tempreature = send_data[10];
            break;
        }
        
        case TEMPERATURE_RAISE://升高温度
        {
            send_data[7] = AIRCONDITION_TEMPERATURE;
            if (aircondition_dev.tempreature < DEV_TEMPERATURE_MAX)
            {
                aircondition_dev.tempreature ++;
            }
            send_data[10] = aircondition_dev.tempreature;//数据
            break;
        }
        case TEMPERATURE_REDUCE://降低温度
        {
            send_data[7] = AIRCONDITION_TEMPERATURE;
            if (aircondition_dev.tempreature > DEV_TEMPERATURE_MIN)
            {
                aircondition_dev.tempreature --;
            }
            send_data[10] = aircondition_dev.tempreature;//数据
            break;
        }
        case TEMPERATURE_MAX://最高温度
        {
            send_data[7] = AIRCONDITION_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MAX;//数据
            aircondition_dev.tempreature = DEV_TEMPERATURE_MAX;
            break;
        }
        case TEMPERATURE_MIN://最低温度
        {
            send_data[7] = AIRCONDITION_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MIN;//数据
            aircondition_dev.tempreature = DEV_TEMPERATURE_MIN;
            break;
        }
        case LIGHT_OFF://关闭氛围灯
        {
            send_data[7] = AIRCONDITION_LIGHT;
            send_data[10] = FUN_TURN_OFF;//数据
            aircondition_dev.light = FUN_TURN_OFF;
            break;
        }
        case LIGHT_ON://打开氛围灯
        {
            send_data[7] = AIRCONDITION_LIGHT;
            send_data[10] = FUN_TURN_ON;//数据
            aircondition_dev.light = FUN_TURN_ON;
            break;
        }
        case HEALTH_OFF://关闭健康
        {
            send_data[7] = AIRCONDITION_HEALTH;
            send_data[10] = FUN_TURN_OFF;//数据
            aircondition_dev.health = FUN_TURN_OFF;
            break;
        }
        case HEALTH_ON://打开健康
        {
            send_data[7] = AIRCONDITION_HEALTH;
            send_data[10] = FUN_TURN_ON;//数据
            aircondition_dev.health = FUN_TURN_ON;
            break;
        }
        case SLEEP_OFF://关闭睡眠
        {
            send_data[7] = AIRCONDITION_SLEEP;
            send_data[10] = FUN_TURN_OFF;//数据
            aircondition_dev.sleep = FUN_TURN_OFF;
            break;
        }
        case SLEEP_ON://打开睡眠
        {
            send_data[7] = AIRCONDITION_SLEEP;
            send_data[10] = FUN_TURN_ON;//数据
            aircondition_dev.sleep = FUN_TURN_ON;
            break;
        }
        default:
        {
            send_flag = false;
            break;
        }
    }
    if (send_flag)
    {
        uint16_t crc_cal = crc16_ccitt(0, send_data, 11);
        mprintf("crc = 0x%x\r\n",crc_cal);
        //send_data[send_data[7]+8] = crc_cal >> 8;
        send_data[11] = crc_cal>>8;
        send_data[12] = crc_cal&0xFF;
        
        cias_crypto_data(send_data, 13);
        rf_cb_funcs.rf_send(send_data, 13);
    }
    return 0;
}

void aircondition_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;

    switch (msg.function_id)
    {
        case AIRCONDITION_POWER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关空调\r\n");       
                prompt_play_by_cmd_id(TURN_OFF, -1, default_play_done_callback,true);      
                uart_send_asr(TURN_OFF);  
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("开空调\r\n");
                prompt_play_by_cmd_id(TURN_ON, -1, default_play_done_callback,true);  
                uart_send_asr(TURN_ON);     
            }
            aircondition_dev.power =  msg.data[0];
            break;
        }
        case AIRCONDITION_MODE:
        {
            prompt_play_by_cmd_id(MODE_COOL+msg.data[0]-1, -1, default_play_done_callback,true);  
            uart_send_asr(MODE_COOL + msg.data[0] -1); 
            aircondition_dev.mode = msg.data[0];
            break;
        }
        case AIRCONDITION_SWING:
        {
            prompt_play_by_cmd_id(SWING_LR + (msg.data[0]-2)/2, -1, default_play_done_callback,true);  //关闭扫风播报错误
            uart_send_asr(SWING_LR+(msg.data[0]-2)/2); 
            aircondition_dev.swing = msg.data[0];
            break;
        }
       case AIRCONDITION_TEMPERATURE:
        {
            if (msg.data[0] == 0xF1)
            {
                mprintf("升高温度\r\n");
                prompt_play_by_cmd_id(TEMPERATURE_RAISE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_RAISE);   
                if (aircondition_dev.tempreature < DEV_TEMPERATURE_MAX)
                {
                    aircondition_dev.tempreature ++;
                }
            }
            else if (msg.data[0] == 0xF2)
            {
                mprintf("降低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_REDUCE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_REDUCE);    
                if (aircondition_dev.tempreature > DEV_TEMPERATURE_MAX)
                {
                    aircondition_dev.tempreature --;
                }       
            }
            else if (msg.data[0] == 0xF3)
            {
                mprintf("最高温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MAX, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MAX);   
                aircondition_dev.tempreature = DEV_TEMPERATURE_MAX;      
            }
            else if (msg.data[0] == 0xF4)
            {
                mprintf("最低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MIN, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MIN);    
                aircondition_dev.tempreature = DEV_TEMPERATURE_MIN;     
            }  
            else if ((msg.data[0] >= 16)&&(msg.data[0] <= 30))  
            {
                prompt_play_by_cmd_id(TEMPERATURE_16+msg.data[0]-16, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_16+msg.data[0]-16);    
                aircondition_dev.tempreature = msg.data[0];         
            }       
            break;
        }
        case AIRCONDITION_LIGHT:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭氛围灯\r\n");
                prompt_play_by_cmd_id(LIGHT_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(LIGHT_OFF);        
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开氛围灯\r\n");       
                prompt_play_by_cmd_id(LIGHT_ON, -1, default_play_done_callback,true); 
                uart_send_asr(LIGHT_ON);           
            }
            aircondition_dev.light = msg.data[0];
            break;
        }
        case AIRCONDITION_HEALTH:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭健康\r\n");
                prompt_play_by_cmd_id(HEALTH_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(HEALTH_OFF);        
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开健康\r\n");       
                prompt_play_by_cmd_id(HEALTH_ON, -1, default_play_done_callback,true); 
                uart_send_asr(HEALTH_ON);           
            }
            aircondition_dev.health = msg.data[0];
            break;
        }
        case AIRCONDITION_SLEEP:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭睡眠\r\n");
                prompt_play_by_cmd_id(SLEEP_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(SLEEP_OFF);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("睡眠模式\r\n");       
                prompt_play_by_cmd_id(SLEEP_ON, -1, default_play_done_callback,true);   
                uart_send_asr(SLEEP_ON);         
            }
            aircondition_dev.sleep = msg.data[0];
            break;
        }
        case AIRCONDITION_SPEED:
        {
            prompt_play_by_cmd_id(SWING_AUTO + msg.data[0]-1, -1, default_play_done_callback,true);  
            uart_send_asr(SWING_AUTO+msg.data[0]-1); 
            aircondition_dev.swing = msg.data[0];
            break;
        }
        case AIRCONDITION_STRONG:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("强劲风关\r\n");    
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("强劲风开\r\n");       
                prompt_play_by_cmd_id(SWING_VERY_HIGH, -1, default_play_done_callback,true);   
                uart_send_asr(SWING_VERY_HIGH);         
            }
            aircondition_dev.strong = msg.data[0];
            break;
        }
        default:
        {
            mprintf("other cmd\r\n");
            break;
        }
    }

}

void aircondition_query(ble_msg_V1_t msg)
{
    if ((msg.function_id < 0x01)||(msg.function_id > 0x0E))
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
    send_data[4] = AIRCONDITION_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x41;
    send_data[7] = msg.function_id;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    
    uint8_t *data = (uint8_t *)&aircondition_dev;
/*     for (size_t i = 0; i < 14; i++)
    {
        mprintf("%x ",data[i]);
    } */
    
    send_data[10] = data[msg.function_id-1];
    uint16_t crc_cal = crc16_ccitt(0, send_data, 11);
    //mprintf("crc = 0x%x data10 = 0x%x\r\n",crc_cal, send_data[10]);
    //send_data[send_data[7]+8] = crc_cal >> 8;
    send_data[11] = crc_cal>>8;
    send_data[12] = crc_cal&0xFF;

    cias_crypto_data(send_data, 13);
    rf_cb_funcs.rf_send(send_data, 13);

}

