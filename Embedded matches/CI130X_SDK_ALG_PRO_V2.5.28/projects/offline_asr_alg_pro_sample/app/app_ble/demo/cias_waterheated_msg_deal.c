#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "ci130x_dpmu.h"
#include "rf_msg_deal.h"
#include "system_msg_deal.h"
#include "sdk_default_config.h"
#include "user_config.h"
#include "rf_msg_deal.h"
#include "crc.h"
#include "exe_api.h"
#include "prompt_player.h"
#include "cias_waterheated_msg_deal.h"

uint32_t waterheated_timer_counter = 0;//定时器秒数
xTimerHandle waterheated_timer = NULL; //定时器
waterheated_dev_t waterheated_dev;
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数

static waterheated_timer_callback()
{
    if (waterheated_timer_counter)
    {
        waterheated_timer_counter --;
        mprintf("定时剩余：%d S\r\n",waterheated_timer_counter);
    }
    else
    {
        xTimerStop(waterheated_timer,0);
        waterheated_dev.timing = 0; 
        mprintf("定时时间到\r\n");
        prompt_play_by_cmd_string("<time_on>", -1, default_play_done_callback,true);
    }
}

void waterheated_init()
{
    waterheated_dev.power = FUN_TURN_OFF;
    waterheated_dev.childlock = FUN_TURN_OFF;
    waterheated_dev.temperature = DEV_TEMPERATURE_NORMALLY;
    waterheated_dev.temperature_set = FUN_TURN_OFF;
    waterheated_dev.timing = FUN_TURN_OFF;
    waterheated_dev.sleep = FUN_TURN_OFF;
    waterheated_dev.quick_heat = FUN_TURN_OFF;
    waterheated_dev.remove_mity = FUN_TURN_OFF;
    waterheated_dev.warm_area = FORBID;
    waterheated_dev.anion = FUN_TURN_OFF;
    waterheated_dev.lamp = FUN_TURN_OFF;
    waterheated_dev.lamp_set = FORBID;
    waterheated_dev.sleep_voice = FORBID;
    waterheated_dev.voice = FORBID;
    waterheated_dev.timing_set = FUN_TURN_OFF;
    waterheated_timer = xTimerCreate("waterheated_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, waterheated_timer_callback);
}
uint8_t waterheated_report(uint16_t cmd_id)
{
    bool send_flag = true;
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = WATERHEATED_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    switch (cmd_id)
    {
        case TURN_OFF://关水暖毯
        {
            send_data[7] = WATERHEATED_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.power = FUN_TURN_OFF;
            break;
        }
        case TURN_ON://打开水暖毯
        {
            send_data[7] = WATERHEATED_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            waterheated_dev.power = FUN_TURN_ON;
            break;
        }
        case CHILDLOCK_OFF://关闭童锁
        {
            send_data[7] = WATERHEATED_CHILDLOCK;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.childlock = FUN_TURN_OFF;
            break;
        }
        case CHILDLOCK_ON://打开童锁
        {
            send_data[7] = WATERHEATED_CHILDLOCK;
            send_data[10] = FUN_TURN_ON;//数据
            waterheated_dev.childlock = FUN_TURN_ON;
            break;
        }
        case SLEEP_OFF://关闭睡眠
        {
            send_data[7] = WATERHEATED_SLEEP;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.sleep = FUN_TURN_OFF;
            break;
        }
        case SLEEP_ON://打开睡眠
        {
            send_data[7] = WATERHEATED_SLEEP;
            send_data[10] = FUN_TURN_ON;//数据
            waterheated_dev.sleep = FUN_TURN_ON;
            break;
        }
        case QUICKHEAT_OFF://关闭速热
        {
            send_data[7] = WATERHEATED_QUICK_HEAT;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.quick_heat = FUN_TURN_OFF;
            break;
        }
        case QUICKHEAT_ON://打开速热
        {
            send_data[7] = WATERHEATED_QUICK_HEAT;
            send_data[10] = FUN_TURN_ON;//数据
            waterheated_dev.quick_heat = FUN_TURN_ON;
            break;
        }
        case REMOVE_MITE_OFF://关闭除螨
        {
            send_data[7] = WATERHEATED_REMOVE_MITY;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.remove_mity = FUN_TURN_OFF;
            break;
        }
        case REMOVE_MITE_ON://打开除螨
        {
            send_data[7] = WATERHEATED_REMOVE_MITY;
            send_data[10] = FUN_TURN_ON;//数据
            waterheated_dev.remove_mity = FUN_TURN_ON;
            break;
        }
        case TIMING_OFF://关闭定时
        {
            send_data[7] = WATERHEATED_TIMING;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.timing = FUN_TURN_OFF;
            xTimerStop(waterheated_timer, 0);
            break;
        }
        case TIMMING_1H ... TIMMING_12H://定时X小时
        {
            send_data[7] = WATERHEATED_TIMING;
            send_data[10] = (cmd_id-TIMMING_1H) + 0xA1;//数据
            waterheated_dev.timing = send_data[10];
            waterheated_timer_counter = (waterheated_dev.timing - 0xA0)*3600;
            xTimerStop(waterheated_timer, 0);
            xTimerStart(waterheated_timer, 0);
            break;
        }
        case TEMPERATURE_18 ... TEMPERATURE_45://
        {
            send_data[7] = WATERHEATED_TEMPERATURE;
            send_data[10] = cmd_id-TEMPERATURE_18+0x12;//数据
            waterheated_dev.temperature = send_data[10];
            break;
        }
        case TEMPERATURE_RAISE://升高温度
        {
            send_data[7] = WATERHEATED_TEMPERATURE;
            if (waterheated_dev.temperature < DEV_TEMPERATURE_MAX)
            {
                waterheated_dev.temperature ++;
            }
            send_data[10] = waterheated_dev.temperature;//数据
            break;
        }
        case TEMPERATURE_REDUCE://降低温度
        {
            send_data[7] = WATERHEATED_TEMPERATURE;
            if (waterheated_dev.temperature > DEV_TEMPERATURE_MIN)
            {
                waterheated_dev.temperature --;
            }
            send_data[10] = waterheated_dev.temperature;//数据
            break;
        }
        case TEMPERATURE_MAX://最高温度
        {
            send_data[7] = WATERHEATED_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MAX;//数据
            waterheated_dev.temperature = DEV_TEMPERATURE_MAX;
            break;
        }
        case TEMPERATURE_MIN://最低温度
        {
            send_data[7] = WATERHEATED_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MIN;//数据
            waterheated_dev.temperature = DEV_TEMPERATURE_MIN;
            break;
        }
        case ANION_OFF://关负离子
        {
            send_data[7] = WATERHEATED_ANION;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.anion = FUN_TURN_OFF;
            break;
        }
        case ANION_ON://开负离子
        {
            send_data[7] = WATERHEATED_ANION;
            send_data[10] = FUN_TURN_ON;//数据
            waterheated_dev.anion = FUN_TURN_ON;
            break;
        }
        case LAMP_OFF://关小夜灯
        {
            send_data[7] = WATERHEATED_LAMP;
            send_data[10] = FUN_TURN_OFF;//数据
            waterheated_dev.lamp = FUN_TURN_OFF;
            break;
        }
        case LAMP_ON://开小夜灯
        {
            send_data[7] = WATERHEATED_LAMP;
            send_data[10] = FUN_TURN_ON;//数据
            waterheated_dev.lamp = FUN_TURN_ON;
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


void set_waterheated_timing(uint8_t timing)
{
    prompt_play_by_cmd_id(timing-0xA1+TIMMING_1H, -1, default_play_done_callback,true);
    uart_send_asr(timing-0xA1+TIMMING_1H);
    waterheated_dev.timing = timing ; 
    
    waterheated_timer_counter = (waterheated_dev.timing-0xA0)*3600;
    xTimerStop(waterheated_timer, 0);
    xTimerStart(waterheated_timer, 0);
}

void set_waterheated_temperature(uint8_t temperature)
{
    if((temperature >= DEV_TEMPERATURE_MIN)&&(temperature <= DEV_TEMPERATURE_MAX)) 
    {
        prompt_play_by_cmd_id(temperature-0x12+TEMPERATURE_18, -1, default_play_done_callback,true); 
        uart_send_asr(temperature-0x12+TEMPERATURE_18);
        waterheated_dev.temperature = temperature;
    }
}

void waterheated_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;

    switch (msg.function_id)
    {
        case WATERHEATED_POWER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关水暖毯\r\n");       
                prompt_play_by_cmd_id(TURN_OFF, -1, default_play_done_callback,true);      
                uart_send_asr(TURN_OFF);  
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("开水暖毯\r\n");
                prompt_play_by_cmd_id(TURN_ON, -1, default_play_done_callback,true);  
                uart_send_asr(TURN_ON);     
            }
            waterheated_dev.power =  msg.data[0];
            break;
        }
        case WATERHEATED_CHILDLOCK:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭童锁\r\n");
                prompt_play_by_cmd_id(CHILDLOCK_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(CHILDLOCK_OFF);        
            }
            if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开童锁\r\n");
                prompt_play_by_cmd_id(CHILDLOCK_ON, -1, default_play_done_callback,true);   
                uart_send_asr(CHILDLOCK_ON);      
            }
            waterheated_dev.childlock = msg.data[0];
            break;
        }
        case WATERHEATED_SLEEP:
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
            waterheated_dev.sleep = msg.data[0];
            break;
        }
        case WATERHEATED_QUICK_HEAT:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭速热\r\n");
                prompt_play_by_cmd_id(QUICKHEAT_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(QUICKHEAT_OFF);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开速热\r\n");       
                prompt_play_by_cmd_id(QUICKHEAT_ON, -1, default_play_done_callback,true);   
                uart_send_asr(QUICKHEAT_ON); 
            }
            waterheated_dev.quick_heat = msg.data[0];
            break;
        }
        case WATERHEATED_REMOVE_MITY:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭除螨\r\n");
                prompt_play_by_cmd_id(REMOVE_MITE_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(REMOVE_MITE_OFF);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开除螨\r\n");       
                prompt_play_by_cmd_id(REMOVE_MITE_ON, -1, default_play_done_callback,true);   
                uart_send_asr(REMOVE_MITE_ON); 
            }
            waterheated_dev.quick_heat = msg.data[0];
            break;
        }
        case WATERHEATED_TIMING:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭定时\r\n");
                prompt_play_by_cmd_id(TIMING_OFF, -1, default_play_done_callback,true);   
                uart_send_asr(TIMING_OFF);   
                waterheated_dev.timing = 0;   
                xTimerStop(waterheated_timer,0);
            }
            else
            {
                set_waterheated_timing(msg.data[0]);              
            }
            break;
        }
        case WATERHEATED_TEMPERATURE:
        {
            if (msg.data[0] == 0xF1)
            {
                mprintf("升高温度\r\n");
                prompt_play_by_cmd_id(TEMPERATURE_RAISE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_RAISE);   
                if (waterheated_dev.temperature < DEV_TEMPERATURE_MAX)
                {
                    waterheated_dev.temperature ++;
                }
            }
            else if (msg.data[0] == 0xF2)
            {
                mprintf("降低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_REDUCE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_REDUCE);    
                if (waterheated_dev.temperature > DEV_TEMPERATURE_MIN)
                {
                    waterheated_dev.temperature --;
                }       
            }
            else if (msg.data[0] == 0xF3)
            {
                mprintf("最高温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MAX, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MAX);   
                waterheated_dev.temperature = DEV_TEMPERATURE_MAX;      
            }
            else if (msg.data[0] == 0xF4)
            {
                mprintf("最低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MIN, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MIN);    
                waterheated_dev.temperature = DEV_TEMPERATURE_MIN;     
            }  
            else          
            {
                set_waterheated_temperature(msg.data[0]);        
            }
            break;
        }
        case WATERHEATED_ANION:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关负离子\r\n");
                prompt_play_by_cmd_id(ANION_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(ANION_OFF);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("开负离子\r\n");       
                prompt_play_by_cmd_id(ANION_ON, -1, default_play_done_callback,true);   
                uart_send_asr(ANION_ON); 
            }
            waterheated_dev.quick_heat = msg.data[0];
            break;
        }
        case WATERHEATED_LAMP:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关小夜灯\r\n");
                prompt_play_by_cmd_id(LAMP_ON, -1, default_play_done_callback,true);  
                uart_send_asr(LAMP_ON);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("开小夜灯\r\n");       
                prompt_play_by_cmd_id(LAMP_OFF, -1, default_play_done_callback,true);   
                uart_send_asr(LAMP_OFF); 
            }
            waterheated_dev.quick_heat = msg.data[0];
            break;
        }
        default:
        {
            mprintf("other cmd\r\n");
            break;
        }
    }

}

void waterheated_query(ble_msg_V1_t msg)
{
    if ((msg.function_id < 0x01)||(msg.function_id > 0x0F))
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
    send_data[4] = WATERHEATED_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x41;
    send_data[7] = msg.function_id;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    
    uint8_t *data = (uint8_t *)&waterheated_dev;
/*     for (size_t i = 0; i < 14; i++)
    {
        mprintf("%x ",data[i]);
    } */
    if(msg.function_id == WATERHEATED_TEMPERATURE_SET)
    {
        send_data[9] = 0x02;
        send_data[10] = DEV_TEMPERATURE_MIN;
        send_data[11] = DEV_TEMPERATURE_MAX;
    }
    else if(msg.function_id == WATERHEATED_LAMP_SET)         //数据长度为1，设置功能使能，否知设置夜灯颜色
    {
        //功能使能
        // send_data[10] = DEV_LAMP_COLOR_FORBID;

        //夜灯颜色
        send_data[9] = 0x03;
        send_data[10] = DEV_LAMP_COLOR_YELLOW;
        send_data[11] = DEV_LAMP_COLOR_WHITE;
        send_data[12] = DEV_LAMP_COLOR_PURPLE;
    }
    else if(msg.function_id == WATERHEATED_TIME_SET)
    {
        send_data[9] = 0x02;
        send_data[10] = DEV_TIMMING_MIN;
        send_data[11] = DEV_TIMMING_MAX;
    }
    else
    {
        send_data[10] = data[msg.function_id-1];
    }

    uint16_t crc_cal = crc16_ccitt(0, send_data, send_data[8]*16+send_data[9]+10);
    // mprintf("crc = 0x%x\r\n",crc_cal);
    send_data[send_data[8]*16+send_data[9]+10] = crc_cal>>8;
    send_data[send_data[8]*16+send_data[9]+11] = crc_cal&0xFF;
    len = send_data[8]*16+send_data[9]+12;
    cias_crypto_data(send_data, len); 
    rf_cb_funcs.rf_send(send_data, len);



}

