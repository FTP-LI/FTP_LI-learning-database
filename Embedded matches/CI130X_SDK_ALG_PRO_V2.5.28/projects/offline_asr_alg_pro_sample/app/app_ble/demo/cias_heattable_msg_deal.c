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
#include "cias_heattable_msg_deal.h"

uint32_t heattable_timer_counter = 0;//定时器秒数
xTimerHandle heattable_timer = NULL; //定时器
heattable_dev_t heattable_dev;
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数

static heattable_timer_callback()
{
    if (heattable_timer_counter)
    {
        heattable_timer_counter --;
        mprintf("定时剩余：%d S\r\n",heattable_timer_counter);
    }
    else
    {
        xTimerStop(heattable_timer,0);
        heattable_dev.timing = 0; 
        mprintf("定时时间到\r\n");
    }
}


void heattable_init()
{
    heattable_dev.power = FUN_TURN_OFF;
    heattable_dev.front_gear = FUN_TURN_OFF;
    heattable_dev.back_gear = FUN_TURN_OFF;
    heattable_dev.left_gear = FUN_TURN_OFF;
    heattable_dev.right_gear = FUN_TURN_OFF;
    heattable_dev.down_gear = FUN_TURN_OFF;
    heattable_dev.warmer_gear = FUN_TURN_OFF;
    heattable_dev.tempreature = DEV_TEMPERATURE_MIN;
    heattable_dev.desktop = FUN_TURN_OFF;
    heattable_timer = xTimerCreate("heattable_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, heattable_timer_callback);
}

uint8_t heattable_report(uint16_t cmd_id)
{
    bool send_flag = true;
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = HEATTABLE_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    switch (cmd_id)
    {
        case TURN_OFF:
        {
            send_data[7] = HEATTABLE_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            heattable_dev.power = FUN_TURN_OFF;
            break;
        }
        case TURN_ON:
        {
            send_data[7] = HEATTABLE_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            heattable_dev.power = FUN_TURN_ON;
            break;
        }
        case FRONT_GEAR_OFF://
        {
            send_data[7] =  HEATTABLE_FRONT;
            send_data[10] = FUN_TURN_OFF;//数据
            heattable_dev.front_gear = FUN_TURN_OFF;
            break;
        }
        case FRONT_GEAR_ON://
        {
            send_data[7] = HEATTABLE_FRONT;
            send_data[10] = FUN_TURN_ON;//数据
            heattable_dev.front_gear = FUN_TURN_ON;
            break;
        }
        case FRONT_GEAR_1 ... FRONT_GEAR_5://
        {
            send_data[7] = HEATTABLE_FRONT;
            send_data[10] = cmd_id-FRONT_GEAR_1+DEV_GEAR_MIN;//数据
            heattable_dev.front_gear = send_data[10];
            break;
        }
        case BACK_GEAR_OFF://
        {
            send_data[7] =  HEATTABLE_BACK;
            send_data[10] = FUN_TURN_OFF;//数据
            heattable_dev.back_gear = FUN_TURN_OFF;
            break;
        }
        case BACK_GEAR_ON://
        {
            send_data[7] = HEATTABLE_BACK;
            send_data[10] = FUN_TURN_ON;//数据
            heattable_dev.back_gear = FUN_TURN_ON;
            break;
        }
        case BACK_GEAR_1 ... BACK_GEAR_5://
        {
            send_data[7] = HEATTABLE_BACK;
            send_data[10] = cmd_id-BACK_GEAR_1+DEV_GEAR_MIN;//数据
            heattable_dev.back_gear = send_data[10];
            break;
        }
        case LEFT_GEAR_OFF://
        {
            send_data[7] =  HEATTABLE_LEFT;
            send_data[10] = FUN_TURN_OFF;//数据
            heattable_dev.left_gear = FUN_TURN_OFF;
            break;
        }
        case LEFT_GEAR_ON://
        {
            send_data[7] = HEATTABLE_LEFT;
            send_data[10] = FUN_TURN_ON;//数据
            heattable_dev.left_gear = FUN_TURN_ON;
            break;
        }
        case LEFT_GEAR_1 ... LEFT_GEAR_5://
        {
            send_data[7] = HEATTABLE_LEFT;
            send_data[10] = cmd_id-LEFT_GEAR_1+DEV_GEAR_MIN;//数据
            heattable_dev.left_gear = send_data[10];
            break;
        }
        case RIGHT_GEAR_OFF://
        {
            send_data[7] =  HEATTABLE_RIGHT;
            send_data[10] = FUN_TURN_OFF;//数据
            heattable_dev.right_gear = FUN_TURN_OFF;
            break;
        }
        case RIGHT_GEAR_ON://
        {
            send_data[7] = HEATTABLE_RIGHT;
            send_data[10] = FUN_TURN_ON;//数据
            heattable_dev.right_gear = FUN_TURN_ON;
            break;
        }
        case RIGHT_GEAR_1 ... RIGHT_GEAR_5://
        {
            send_data[7] = HEATTABLE_RIGHT;
            send_data[10] = cmd_id-RIGHT_GEAR_1+DEV_GEAR_MIN;//数据
            heattable_dev.right_gear = send_data[10];
            break;
        }
        case DOWN_GEAR_OFF://
        {
            send_data[7] =  HEATTABLE_DOWN;
            send_data[10] = FUN_TURN_OFF;//数据
            heattable_dev.down_gear = FUN_TURN_OFF;
            break;
        }
        case DOWN_GEAR_ON://
        {
            send_data[7] = HEATTABLE_DOWN;
            send_data[10] = FUN_TURN_ON;//数据
            heattable_dev.down_gear = FUN_TURN_ON;
            break;
        }
        case DOWN_GEAR_1 ... DOWN_GEAR_5://
        {
            send_data[7] = HEATTABLE_DOWN;
            send_data[10] = cmd_id-DOWN_GEAR_1+DEV_GEAR_MIN;//数据
            heattable_dev.down_gear = send_data[10];
            break;
        }
        case WARMER_GEAR_OFF://
        {
            send_data[7] =  HEATTABLE_WARMER;
            send_data[10] = FUN_TURN_OFF;//数据
            heattable_dev.warmer_gear = FUN_TURN_OFF;
            break;
        }
        case WARMER_GEAR_ON://
        {
            send_data[7] = HEATTABLE_WARMER;
            send_data[10] = FUN_TURN_ON;//数据
            heattable_dev.warmer_gear = FUN_TURN_ON;
            break;
        }
        case WARMER_GEAR_1 ... WARMER_GEAR_3://
        {
            send_data[7] = HEATTABLE_WARMER;
            send_data[10] = cmd_id-WARMER_GEAR_1+DEV_GEAR_MIN;//数据
            heattable_dev.warmer_gear = send_data[10];
            break;
        }

        case TEMPERATURE_40 ... TEMPERATURE_70://温度
        {
            send_data[7] = HEATTABLE_TEMPERATURE;
            send_data[10] = (cmd_id-TEMPERATURE_40)*5+DEV_TEMPERATURE_MIN;//数据
            heattable_dev.tempreature = send_data[10];
            break;
        }
        case TEMPERATURE_RAISE://升高温度
        {
            send_data[7] = HEATTABLE_TEMPERATURE;
            if (heattable_dev.tempreature < DEV_TEMPERATURE_MAX)
            {
                heattable_dev.tempreature +=5;
            }
            send_data[10] = heattable_dev.tempreature;//数据
            break;
        }
        case TEMPERATURE_REDUCE://降低温度
        {
            send_data[7] = HEATTABLE_TEMPERATURE;
            if (heattable_dev.tempreature > DEV_TEMPERATURE_MIN)
            {
                heattable_dev.tempreature -=5;
            }
            send_data[10] = heattable_dev.tempreature;//数据
            break;
        }
        case TEMPERATURE_MAX://最高温度
        {
            send_data[7] = HEATTABLE_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MAX;//数据
            heattable_dev.tempreature = DEV_TEMPERATURE_MAX;
            break;
        }
        case TEMPERATURE_MIN://最低温度
        {
            send_data[7] = HEATTABLE_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MIN;//数据
            heattable_dev.tempreature = DEV_TEMPERATURE_MIN;
            break;
        }
        
        case DESKTOP_PAUSED ... DOWN_DESKTOP_REDUCE://
        {
            send_data[7] = HEATTABLE_DESKTOP;
            uint16_t desktop_status = cmd_id - DESKTOP_PAUSED;
            send_data[10] = (desktop_status/3)*0x10 + desktop_status%3+1;
            heattable_dev.desktop = send_data[10];
            break;
        }
        case ALL_GEAR_1 ... ALL_GEAR_5://
        {
            send_data[7] = HEATTABLE_ALL;
            send_data[10] = cmd_id-ALL_GEAR_1+DEV_GEAR_MIN;//数据
            heattable_dev.front_gear = send_data[10];
            heattable_dev.back_gear = send_data[10];
            heattable_dev.left_gear = send_data[10];
            heattable_dev.right_gear = send_data[10];
            heattable_dev.down_gear = send_data[10];
            heattable_dev.warmer_gear = send_data[10];
            if(send_data[10]>DEV_GEAR_MIN+2)
            heattable_dev.warmer_gear = DEV_GEAR_MIN+2;
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
        //send_data[send_data[7]+8] = crc_cal >> 8;
        send_data[11] = crc_cal>>8;
        send_data[12] = crc_cal&0xFF;

        cias_crypto_data(send_data, 13);
        rf_cb_funcs.rf_send(send_data, 13);
    }
    return 0;
}

void heattable_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;

    switch (msg.function_id)
    {
        case HEATTABLE_POWER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {     
                prompt_play_by_cmd_id(TURN_OFF, -1, default_play_done_callback,true);      
                uart_send_asr(TURN_OFF);  
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                prompt_play_by_cmd_id(TURN_ON, -1, default_play_done_callback,true);  
                uart_send_asr(TURN_ON);     
            }
            heattable_dev.power =  msg.data[0];
            break;
        }
        case HEATTABLE_FRONT ... HEATTABLE_DOWN:
        {
            uint16_t index = msg.function_id - 2;
            if ((msg.data[0] == FUN_TURN_OFF)||(msg.data[0] == FUN_TURN_ON))
            {
                uint16_t cmd_id = FRONT_GEAR_OFF + index*7 + msg.data[0] -1;
                prompt_play_by_cmd_id(cmd_id, -1, default_play_done_callback,true); 
                uart_send_asr(cmd_id);        
            }
            else if ((msg.data[0] >= 0x11)&&(msg.data[0] <= 0x15))
            {      
                uint16_t cmd_id = FRONT_GEAR_1 + index*7 + msg.data[0] - DEV_GEAR_MIN;
                prompt_play_by_cmd_id(cmd_id, -1, default_play_done_callback,true);  
                uart_send_asr(cmd_id);          
            }
            uint8_t *data =(uint8_t *)&heattable_dev;
            data[index] = msg.data[0];
            break;
        }
        
        case HEATTABLE_WARMER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                prompt_play_by_cmd_id(WARMER_GEAR_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(WARMER_GEAR_OFF);        
            }
            if (msg.data[0] == FUN_TURN_ON)
            {
                prompt_play_by_cmd_id(WARMER_GEAR_ON, -1, default_play_done_callback,true);   
                uart_send_asr(WARMER_GEAR_ON);      
            }
            else if ((msg.data[0] >= 0x11)&&(msg.data[0] <= 0x13))
            {      
                uint16_t cmd_id = WARMER_GEAR_1 + msg.data[0] - DEV_GEAR_MIN;
                prompt_play_by_cmd_id(cmd_id, -1, default_play_done_callback,true);  
                uart_send_asr(cmd_id);          
            }
            heattable_dev.warmer_gear = msg.data[0];
            break;
        }
        case HEATTABLE_TEMPERATURE:
        {
            if (msg.data[0] == 0xF1)
            {
                mprintf("升高温度\r\n");
                prompt_play_by_cmd_id(TEMPERATURE_RAISE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_RAISE);   
                if (heattable_dev.tempreature < DEV_TEMPERATURE_MAX)
                {
                    heattable_dev.tempreature += 5;
                }
            }
            else if (msg.data[0] == 0xF2)
            {
                mprintf("降低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_REDUCE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_REDUCE);    
                if (heattable_dev.tempreature > DEV_TEMPERATURE_MAX)
                {
                    heattable_dev.tempreature -= 5;
                }       
            }
            else if (msg.data[0] == 0xF3)
            {
                mprintf("最高温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MAX, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MAX);   
                heattable_dev.tempreature = DEV_TEMPERATURE_MAX;      
            }
            else if (msg.data[0] == 0xF4)
            {
                mprintf("最低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MIN, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MIN);    
                heattable_dev.tempreature = DEV_TEMPERATURE_MIN;     
            }  
            else if ((msg.data[0] >= DEV_GEAR_MIN)&&(msg.data[0] <= DEV_GEAR_MAX))  
            {
                uint16_t cmd_id = TEMPERATURE_40 + (msg.data[0]-DEV_GEAR_MIN)/5;
                prompt_play_by_cmd_id(cmd_id, -1, default_play_done_callback,true);  
                uart_send_asr(cmd_id);    
                heattable_dev.tempreature = msg.data[0];         
            }       
            break;
        }
        case HEATTABLE_DESKTOP:
        {
            uint16_t cmd_id = DESKTOP_PAUSED + (msg.data[0]/16)*3 + msg.data[0]%16-1;
            prompt_play_by_cmd_id(cmd_id, -1, default_play_done_callback,true);  
            uart_send_asr(cmd_id);          
            heattable_dev.desktop = msg.data[0];
            break;
        }
        case HEATTABLE_ALL:
        {
            uint16_t cmd_id = ALL_GEAR_1 + msg.data[0] - DEV_GEAR_MIN;
            prompt_play_by_cmd_id(cmd_id, -1, default_play_done_callback,true);  
            uart_send_asr(cmd_id);
            heattable_dev.front_gear = msg.data[0];
            heattable_dev.back_gear = msg.data[0];
            heattable_dev.left_gear = msg.data[0];
            heattable_dev.right_gear = msg.data[0];
            heattable_dev.down_gear = msg.data[0];
            heattable_dev.warmer_gear = msg.data[0];
            if(msg.data[0]>DEV_GEAR_MIN+2)
            heattable_dev.warmer_gear = DEV_GEAR_MIN+2;
            break;
        } 

        default:
        {
            mprintf("other cmd\r\n");
            break;
        }
    }

}

void heattable_query(ble_msg_V1_t msg)
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
    send_data[4] = HEATTABLE_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x41;
    send_data[7] = msg.function_id;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    
    
/*     for (size_t i = 0; i < 14; i++)
    {
        mprintf("%x ",data[i]);
    } */
    uint8_t *data = (uint8_t *)&heattable_dev;
    send_data[10] = data[msg.function_id-1];
    uint16_t crc_cal = crc16_ccitt(0, send_data, 11);
    //mprintf("crc = 0x%x data10 = 0x%x\r\n",crc_cal, send_data[10]);
    //send_data[send_data[7]+8] = crc_cal >> 8;
    send_data[11] = crc_cal>>8;
    send_data[12] = crc_cal&0xFF;

    cias_crypto_data(send_data, 13);
    rf_cb_funcs.rf_send(send_data, 13);
}

