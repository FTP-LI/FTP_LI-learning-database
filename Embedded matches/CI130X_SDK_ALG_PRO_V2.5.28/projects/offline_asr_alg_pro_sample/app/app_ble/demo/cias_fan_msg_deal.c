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
#include "cias_fan_msg_deal.h"

uint32_t fan_timer_counter = 0;//定时器秒数
xTimerHandle fan_timer = NULL; //定时器
fan_dev_t fan_dev;
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数

static fan_timer_callback()
{
    if (fan_timer_counter)
    {
        fan_timer_counter --;
        mprintf("定时剩余：%d S\r\n",fan_timer_counter);
    }
    else
    {
        xTimerStop(fan_timer,0);
        fan_dev.timing = 0; 
        mprintf("定时时间到\r\n");
    }
}


void set_fan_timing(uint8_t timing)
{
    if ((timing >= 0xA1)&&(timing <= 0xAC))
    {
        prompt_play_by_cmd_id(timing-0xA1+TIMMING_1H, -1, default_play_done_callback,true);
        uart_send_asr(timing-0xA1+TIMMING_1H);
        fan_dev.timing = timing; 
        
        fan_timer_counter = (fan_dev.timing-0xA0)*3600;
        xTimerStop(fan_timer, 0);
        xTimerStart(fan_timer, 0);
    }
    else if (timing == 0xAD)
    {
        prompt_play_by_cmd_id(TIMMING_HARF, -1, default_play_done_callback,true);
        uart_send_asr(TIMMING_HARF);
        fan_dev.timing = timing ; 
        
        fan_timer_counter = 1800;
        xTimerStop(fan_timer, 0);
        xTimerStart(fan_timer, 0);
    }
}


void fan_init()
{
    fan_dev.power = FUN_TURN_OFF;
    fan_dev.speed = DEV_SPEED_MIN;
    fan_dev.shake = FUN_TURN_OFF;
    fan_dev.mode = FAN_MODE_NORMAL;
    fan_dev.anion = FUN_TURN_OFF;
    fan_dev.timing = FUN_TURN_OFF;
    fan_timer = xTimerCreate("fan_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, fan_timer_callback);
}
uint8_t fan_report(uint16_t cmd_id)
{
    bool send_flag = true;
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = FAN_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    switch (cmd_id)
    {
        case TURN_OFF:
        {
            send_data[7] = FAN_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.power = FUN_TURN_OFF;
            break;
        }
        case TURN_ON:
        {
            send_data[7] = FAN_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            fan_dev.power = FUN_TURN_ON;
            break;
        }
         case SPEED_LOW://一档风
        {
            send_data[7] =  FAN_SPEED;
            send_data[10] = 0x01;//数据
            fan_dev.speed = 0x01;
            break;
        }
        case SPEED_MIDDLE://二挡风
        {
            send_data[7] = FAN_SPEED;
            send_data[10] = 0x02;//数据
            fan_dev.speed = 0x02;
            break;
        }
        case SPEED_HIGH://三挡风
        {
            send_data[7] = FAN_SPEED;
            send_data[10] = 0x03;//数据
            fan_dev.speed = 0x03;
            break;
        }
        case SPEED_RAISE://风速增大
        {
            send_data[7] = FAN_SPEED;
            if (fan_dev.speed < DEV_SPEED_MAX)
            {
                fan_dev.speed ++;
            }
            send_data[10] = fan_dev.speed;//数据
            break;
        }
        case SPEED_REDUCE://风速减小
        {
            send_data[7] = FAN_SPEED;
             if (fan_dev.speed > DEV_SPEED_MIN)
            {
                fan_dev.speed --;
            }
            send_data[10] = fan_dev.speed;//数据
            break;
        }
         case SPEED_MAX://风速最大
        {
            send_data[7] = FAN_SPEED;           
            send_data[10] = DEV_SPEED_MAX;//数据
            fan_dev.speed = DEV_SPEED_MAX;
            break;
        }
        case SPEED_MIN://风速最小
        {
            send_data[7] = FAN_SPEED;                
            send_data[10] = DEV_SPEED_MIN;//数据
            fan_dev.speed = DEV_SPEED_MIN;
            break;
        }
        case SHAKE_OFF://关闭摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.shake = FUN_TURN_OFF;
            break;
        }
        case SHAKE_ON://打开摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = FUN_TURN_ON;//数据
            fan_dev.shake = FUN_TURN_ON;
            break;
        }
        case SHAKE_LR_OFF://关闭左右摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = 0x11;//数据
            fan_dev.shake = 0x11;
            break;
        }
        case SHAKE_LR_ON://打开左右摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = 0x12;//数据
            fan_dev.shake = 0x12;
            break;
        }
        case SHAKE_HD_OFF://关闭上下摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = 0x21;//数据
            fan_dev.shake = 0x21;
            break;
        }
        case SHAKE_HD_ON://打开上下摇头
        {
            send_data[7] = FAN_SHAKE;
            send_data[10] = 0x22;//数据
            fan_dev.shake = 0x22;
            break;
        }
        case NORMAL_ON://打开正常风
        {
            send_data[7] = FAN_MODE;
            send_data[10] = FAN_MODE_NORMAL;//数据
            fan_dev.mode = FAN_MODE_NORMAL;
            break;
        }
        case SLEEP_ON://打开睡眠
        {
            send_data[7] = FAN_MODE;
            send_data[10] = FAN_MODE_SLEEP;//数据
            fan_dev.mode = FAN_MODE_SLEEP;
            break;
        }
        case NATURAL_ON://打开自然风
        {
            send_data[7] = FAN_MODE;
            send_data[10] = FAN_MODE_NATURAL;//数据
            fan_dev.mode = FAN_MODE_NATURAL;
            break;
        }
        case ANION_OFF://关闭负离子
        {
            send_data[7] = FAN_ANION;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.anion = FUN_TURN_OFF;
            break;
        }
        case ANION_ON://打开负离子
        {
            send_data[7] = FAN_ANION;
            send_data[10] = FUN_TURN_ON;//数据
            fan_dev.anion = FUN_TURN_ON;
            break;
        }
        case TIMING_OFF://关闭定时
        {
            send_data[7] = FAN_TIMING;
            send_data[10] = FUN_TURN_OFF;//数据
            fan_dev.timing = FUN_TURN_OFF;
            xTimerStop(fan_timer, 0);
            break;
        }
        case TIMMING_1H ... TIMMING_12H://定时X小时
        {
            send_data[7] = FAN_TIMING;
            send_data[10] = (cmd_id-TIMMING_1H) + 0xA1;//数据
            fan_dev.timing = send_data[10];
            fan_timer_counter = (fan_dev.timing - 0xA0)*3600;
            xTimerStop(fan_timer, 0);
            xTimerStart(fan_timer, 0);
            break;
        }
        case TIMMING_HARF://定时半小时
        {
            send_data[7] = FAN_TIMING;
            send_data[10] = 0xAD;//数据
            fan_dev.timing = 0xAD;
            fan_timer_counter = 1800;
            xTimerStop(fan_timer, 0);
            xTimerStart(fan_timer, 0);
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

void fan_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;

    switch (msg.function_id)
    {
        case FAN_POWER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关风扇\r\n");       
                prompt_play_by_cmd_id(TURN_OFF, -1, default_play_done_callback,true);      
                uart_send_asr(TURN_OFF);  
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("开风扇\r\n");
                prompt_play_by_cmd_id(TURN_ON, -1, default_play_done_callback,true);  
                uart_send_asr(TURN_ON);     
            }
            fan_dev.power =  msg.data[0];
            break;
        }
        case FAN_SPEED:
        {
            prompt_play_by_cmd_id(SPEED_LOW+msg.data[0]-1, -1, default_play_done_callback,true);  
            uart_send_asr(SPEED_LOW + msg.data[0] -1); 
            fan_dev.speed = msg.data[0];
            break;
        }
        case FAN_SHAKE:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(SHAKE_OFF);        
            }
            if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_ON, -1, default_play_done_callback,true);   
                uart_send_asr(SHAKE_ON);      
            }
            else if (msg.data[0] == 0x11)
            {
                mprintf("关闭左右摇头\r\n");       
                prompt_play_by_cmd_id(SHAKE_LR_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(SHAKE_LR_OFF);          
            }
            else if (msg.data[0] == 0x12)
            {
                mprintf("打开左右摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_LR_ON, -1, default_play_done_callback,true); 
                uart_send_asr(SHAKE_LR_ON);        
            }
            else if (msg.data[0] == 0x21)
            {
                mprintf("关闭上下摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_HD_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(SHAKE_HD_OFF);        
            }
            else if (msg.data[0] == 0x22)
            {
                mprintf("打开上下摇头\r\n");
                prompt_play_by_cmd_id(SHAKE_HD_ON, -1, default_play_done_callback,true);   
                uart_send_asr(SHAKE_HD_ON);      
            }
            fan_dev.shake = msg.data[0];
            break;
        }
        case FAN_MODE:
        {
            if (msg.data[0] == FAN_MODE_NORMAL)
            {
                mprintf("打开正常风\r\n");       
                prompt_play_by_cmd_id(NORMAL_ON, -1, default_play_done_callback,true); 
                uart_send_asr(NORMAL_ON);             
            }
            else if (msg.data[0] == FAN_MODE_SLEEP)
            {
                mprintf("打开睡眠风\r\n");       
                prompt_play_by_cmd_id(SLEEP_ON, -1, default_play_done_callback,true); 
                uart_send_asr(SLEEP_ON);   
            }
            else if (msg.data[0] == FAN_MODE_NATURAL)
            {
                mprintf("打开自然风\r\n");       
                prompt_play_by_cmd_id(NATURAL_ON, -1, default_play_done_callback,true); 
                uart_send_asr(NATURAL_ON);   
            }
            fan_dev.mode = msg.data[0];
            break;
        }
        case FAN_ANION:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭健康\r\n");
                prompt_play_by_cmd_id(ANION_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(ANION_OFF);        
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开健康\r\n");       
                prompt_play_by_cmd_id(ANION_ON, -1, default_play_done_callback,true); 
                uart_send_asr(ANION_ON);           
            }
            fan_dev.anion = msg.data[0];
            break;
        }
        case FAN_TIMING:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭定时\r\n");
                prompt_play_by_cmd_id(TIMING_OFF, -1, default_play_done_callback,true);   
                uart_send_asr(TIMING_OFF);   
                fan_dev.timing = 0;   
                xTimerStop(fan_timer,0);
            }
            else
            {
                set_fan_timing(msg.data[0]);              
            }
            break;
        }
        default:
        {
            mprintf("other cmd\r\n");
            break;
        }
    }

}

void fan_query(ble_msg_V1_t msg)
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
    send_data[4] = FAN_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x41;
    send_data[7] = msg.function_id;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    
    uint8_t *data = (uint8_t *)&fan_dev;
/*     for (size_t i = 0; i < 14; i++)
    {
        mprintf("%x ",data[i]);
    } */
    if(msg.function_id == FAN_TIMING_SET)
    {
        send_data[9] = 0x02;
        send_data[10] = DEV_TIMING_MIN;
        send_data[11] = DEV_TIMING_MAX;
    }
    else if(msg.function_id == FAN_SPEED_SET)
    {
        send_data[9] = 0x02;
        send_data[10] = DEV_SPEED_MIN;
        send_data[11] = DEV_SPEED_MAX;
    }
    else if(msg.function_id == FAN_MODE_SET)  //填入需要的风类对应的数据(如'正常风'填入0x03)
    {
        send_data[9] = 0x03;
        send_data[10] = FAN_MODE_NORMAL;
        send_data[11] = FAN_MODE_SLEEP;
        send_data[12] = FAN_MODE_NATURAL;
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

