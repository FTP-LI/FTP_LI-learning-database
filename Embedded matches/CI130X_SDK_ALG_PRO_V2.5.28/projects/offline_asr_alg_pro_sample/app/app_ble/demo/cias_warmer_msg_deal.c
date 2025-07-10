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
#include "cias_warmer_msg_deal.h"

uint32_t warmer_timer_counter = 0;//定时器秒数
xTimerHandle warmer_timer = NULL; //定时器
warmer_dev_t warmer_dev;
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数

static warmer_timer_callback()
{
    if (warmer_timer_counter)
    {
        warmer_timer_counter --;
        mprintf("定时剩余：%d S\r\n",warmer_timer_counter);
    }
    else
    {
        xTimerStop(warmer_timer,0);
        warmer_dev.timing = 0; 
        mprintf("定时时间到\r\n");
        prompt_play_by_cmd_id(TIME_ON, -1, default_play_done_callback,true);
    }
}

void warmer_init()
{
    warmer_dev.power = FUN_TURN_OFF;
    warmer_dev.shake = FUN_TURN_OFF;
    warmer_dev.anion = FUN_TURN_OFF;
    warmer_dev.energy = FUN_TURN_OFF;
    warmer_dev.sleep = FUN_TURN_OFF;
    warmer_dev.screen = FUN_TURN_OFF;
    warmer_dev.light = FUN_TURN_OFF;
    warmer_dev.gears = FUN_TURN_OFF;
    warmer_dev.dry = FUN_TURN_OFF;
    warmer_dev.disinfection = FUN_TURN_OFF;
    warmer_dev.timing = FUN_TURN_OFF;
    warmer_dev.tempreature = DEV_TEMPERATURE_MIN;
    warmer_dev.humidity = DEV_HUMIDITY_MIN;
    warmer_dev.flame_screen = FUN_TURN_OFF;
    warmer_timer = xTimerCreate("warmer_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, warmer_timer_callback);
}
uint8_t warmer_report(uint16_t cmd_id)
{
    bool send_flag = true;
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = WARMER_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    switch (cmd_id)
    {
        case TURN_OFF://关取暖器
        {
            send_data[7] = WARMER_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.power = FUN_TURN_OFF;
            break;
        }
        case TURN_ON://打开取暖器
        {
            send_data[7] = WARMER_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.power = FUN_TURN_ON;
            break;
        }
        case SHAKE_OFF://关闭摇头
        {
            send_data[7] = WARMER_SHAKE;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.shake = FUN_TURN_OFF;
            break;
        }
        case SHAKE_ON://打开摇头
        {
            send_data[7] = WARMER_SHAKE;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.shake = FUN_TURN_ON;
            break;
        }
        case SHAKE_LR_OFF://关闭左右摇头
        {
            send_data[7] = WARMER_SHAKE;
            send_data[10] = 0x11;//数据
            warmer_dev.shake = 0x11;
            break;
        }
        case SHAKE_LR_ON://打开左右摇头
        {
            send_data[7] = WARMER_SHAKE;
            send_data[10] = 0x12;//数据
            warmer_dev.shake = 0x12;
            break;
        }
        case SHAKE_HD_OFF://关闭上下摇头
        {
            send_data[7] = WARMER_SHAKE;
            send_data[10] = 0x21;//数据
            warmer_dev.shake = 0x21;
            break;
        }
        case SHAKE_HD_ON://打开上下摇头
        {
            send_data[7] = WARMER_SHAKE;
            send_data[10] = 0x22;//数据
            warmer_dev.shake = 0x22;
            break;
        }
        case ANION_OFF://关闭负离子
        {
            send_data[7] = WARMER_ANION;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.anion = FUN_TURN_OFF;
            break;
        }
        case ANION_ON://打开负离子
        {
            send_data[7] = WARMER_ANION;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.anion = FUN_TURN_ON;
            break;
        }
        case ENERGY_OFF://关闭节能
        {
            send_data[7] = WARMER_ENERGY;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.energy = FUN_TURN_OFF;
            break;
        }
        case ENERGY_ON://打开节能
        {
            send_data[7] = WARMER_ENERGY;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.energy = FUN_TURN_ON;
            warmer_dev.sleep = FUN_TURN_OFF;
            warmer_dev.dry = FUN_TURN_OFF;   
            break;
        }
        case SLEEP_OFF://关闭睡眠
        {
            send_data[7] = WARMER_SLEEP;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.sleep = FUN_TURN_OFF;
            break;
        }
        case SLEEP_ON://打开睡眠
        {
            send_data[7] = WARMER_SLEEP;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.sleep = FUN_TURN_ON;
            warmer_dev.energy = FUN_TURN_OFF;
            warmer_dev.dry = FUN_TURN_OFF; 
            break;
        }
        case SCREEN_OFF://关闭显示
        {
            send_data[7] = WARMER_SCREEN;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.screen = FUN_TURN_OFF;
            break;
        }
        case SCREEN_ON://打开显示
        {
            send_data[7] = WARMER_SCREEN;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.screen = FUN_TURN_ON;
            break;
        }
         case LIGHT_OFF://关闭氛围灯
        {
            send_data[7] = WARMER_LIGHT;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.light = FUN_TURN_OFF;
            break;
        }
        case LIGHT_ON://打开氛围灯
        {
            send_data[7] = WARMER_LIGHT;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.light = FUN_TURN_ON;
            break;
        }
        case LOW_ON://打开低档
        {
            send_data[7] =  WARMER_GEARS;
            send_data[10] = 0x01;//数据
            warmer_dev.gears = 0x01;
            break;
        }
        case MIDDLE_ON://打开中档
        {
            send_data[7] = WARMER_GEARS;
            send_data[10] = 0x02;//数据
            warmer_dev.gears = 0x02;
            break;
        }
        case HIGH_ON://打开高档
        {
            send_data[7] = WARMER_GEARS;
            send_data[10] = 0x03;//数据
            warmer_dev.gears = 0x03;
            break;
        }
        case DRY_OFF://关闭干衣模式
        {
            send_data[7] = WARMER_DRY;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.dry = FUN_TURN_OFF;
            break;
        }
        case DRY_ON://打开干衣模式
        {
            send_data[7] = WARMER_DRY;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.dry = FUN_TURN_ON;
            warmer_dev.energy = FUN_TURN_OFF;
            warmer_dev.sleep = FUN_TURN_OFF;
            break;
        }
        case DISINFECTION_OFF://关闭消毒
        {
            send_data[7] = WARMER_DISINFECTION;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.disinfection = FUN_TURN_OFF;
            break;
        }
        case DISINFECTION_ON://打开消毒
        {
            send_data[7] = WARMER_DISINFECTION;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.disinfection = FUN_TURN_ON;
            break;
        }

        case TIMING_OFF://关闭定时
        {
            send_data[7] = WARMER_TIMING;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.timing = FUN_TURN_OFF;
            xTimerStop(warmer_timer, 0);
            break;
        }
        case TIMMING_1H ... TIMMING_12H://定时X小时
        {
            send_data[7] = WARMER_TIMING;
            send_data[10] = (cmd_id-TIMMING_1H) + 0xA1;//数据
            warmer_dev.timing = send_data[10];
            warmer_timer_counter = (warmer_dev.timing - 0xA0)*3600;
            xTimerStop(warmer_timer, 0);
            xTimerStart(warmer_timer, 0);
            break;
        }
        case TIMMING_HARF://定时半小时
        {
            send_data[7] = WARMER_TIMING;
            send_data[10] = 0xAD;//数据
            warmer_dev.timing = 0xAD;
            warmer_timer_counter = 1800;
            xTimerStop(warmer_timer, 0);
            xTimerStart(warmer_timer, 0);
            break;
        }

        case TEMPERATURE_15 ... TEMPERATURE_45://
        {
            send_data[7] = WARMER_TEMPERATURE;
            send_data[10] = cmd_id-TEMPERATURE_15+0x0F;//数据
            warmer_dev.tempreature = send_data[10];
            break;
        }
        case TEMPERATURE_RAISE://升高温度
        {
            send_data[7] = WARMER_TEMPERATURE;
            if (warmer_dev.tempreature < DEV_TEMPERATURE_MAX)
            {
                warmer_dev.tempreature ++;
            }
            send_data[10] = warmer_dev.tempreature;//数据
            break;
        }
        case TEMPERATURE_REDUCE://降低温度
        {
            send_data[7] = WARMER_TEMPERATURE;
            if (warmer_dev.tempreature > DEV_TEMPERATURE_MIN)
            {
                warmer_dev.tempreature --;
            }
            send_data[10] = warmer_dev.tempreature;//数据
            break;
        }
        case TEMPERATURE_MAX://最高温度
        {
            send_data[7] = WARMER_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MAX;//数据
            warmer_dev.tempreature = DEV_TEMPERATURE_MAX;
            break;
        }
        case TEMPERATURE_MIN://最低温度
        {
            send_data[7] = WARMER_TEMPERATURE;
            send_data[10] = DEV_TEMPERATURE_MIN;//数据
            warmer_dev.tempreature = DEV_TEMPERATURE_MIN;
            break;
        }
        case HUMIDITY_OFF://关闭加湿
        {
            send_data[7] = WARMER_HUMIDITY;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.humidity = FUN_TURN_OFF;
            break;
        }
        case HUMIDITY_ON://打开加湿
        {
            send_data[7] = WARMER_HUMIDITY;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.humidity = FUN_TURN_ON;
            break;
        }
        case HUMIDITY_40 ... HUMIDITY_80://湿度
        {
            send_data[7] = WARMER_HUMIDITY;
            send_data[10] = (cmd_id-HUMIDITY_40)*5 + 0x28;//数据
            warmer_dev.humidity = send_data[10];
            break;
        }
        case HUMIDITY_RAISE://升高湿度
        {
            send_data[7] = WARMER_HUMIDITY;
            if (warmer_dev.humidity < DEV_HUMIDITY_MAX)
            {
                warmer_dev.humidity ++;
            }
            send_data[10] = warmer_dev.humidity;//数据
            break;
        }
        case HUMIDITY_REDUCE://降低湿度
        {
            send_data[7] = WARMER_HUMIDITY;
            if (warmer_dev.humidity > DEV_HUMIDITY_MIN)
            {
                warmer_dev.humidity --;
            }
            send_data[10] = warmer_dev.humidity;//数据
            break;
        }
        case HUMIDITY_MAX://最高湿度
        {
            send_data[7] = WARMER_HUMIDITY;
            send_data[10] = DEV_HUMIDITY_MAX;//数据
            warmer_dev.humidity = DEV_HUMIDITY_MAX;
            break;
        }
        case HUMIDITY_MIN://最低湿度
        {
            send_data[7] = WARMER_HUMIDITY;
            send_data[10] = DEV_HUMIDITY_MIN;//数据
            warmer_dev.humidity = DEV_HUMIDITY_MIN;
            break;
        }
        case FLAME_OFF://关闭火焰
        {
            send_data[7] = WARMER_FLAME_SCREEN;
            send_data[10] = FUN_TURN_OFF;//数据
            warmer_dev.flame_screen = FUN_TURN_OFF;
            break;
        }
        case FLAME_ON://打开火焰
        {
            send_data[7] = WARMER_FLAME_SCREEN;
            send_data[10] = FUN_TURN_ON;//数据
            warmer_dev.flame_screen = FUN_TURN_ON;
            break;
        }
        case FLAME_COLORFUL ... FLAME_PURPLE:
        {
            send_data[7] = WARMER_FLAME_SCREEN;
            send_data[10] = cmd_id-FLAME_COLORFUL+0x11;//数据
            warmer_dev.flame_screen = send_data[10];
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


void set_warmer_timing(uint8_t timing)
{
    if ((timing >= 0xA1)&&(timing <= 0xAC))
    {
        prompt_play_by_cmd_id(timing-0xA1+TIMMING_1H, -1, default_play_done_callback,true);
        uart_send_asr(timing-0xA1+TIMMING_1H);
        warmer_dev.timing = timing ; 
        
        warmer_timer_counter = (warmer_dev.timing-0xA0)*3600;
        xTimerStop(warmer_timer, 0);
        xTimerStart(warmer_timer, 0);
    }
    else if (timing == 0xAD)
    {
        prompt_play_by_cmd_id(TIMMING_HARF, -1, default_play_done_callback,true);
        uart_send_asr(TIMMING_HARF);
        warmer_dev.timing = timing ; 
        
        warmer_timer_counter = 1800;
        xTimerStop(warmer_timer, 0);
        xTimerStart(warmer_timer, 0);
    }
}

void set_warmer_temperature(uint8_t temperature)
{
    if((temperature >= DEV_TEMPERATURE_MIN)&&(temperature <= DEV_TEMPERATURE_MAX)) 
    {
        prompt_play_by_cmd_id(temperature-0x0F+TEMPERATURE_15, -1, default_play_done_callback,true); 
        uart_send_asr(temperature-0x0F+TEMPERATURE_15);
        warmer_dev.tempreature = temperature;
    }
}

void set_warmer_humidity(uint8_t humidity)
{
    mprintf("set_warmer_humidity %d\r\n",humidity);
    prompt_play_by_cmd_id((humidity-40)/5+HUMIDITY_40, -1, default_play_done_callback,true); 
    uart_send_asr((humidity-40)/5+HUMIDITY_40);
    warmer_dev.humidity = humidity;
}

void set_warmer_flame_screen(uint8_t flame_screen)
{
    if ((flame_screen >= 0x11)&&(flame_screen <= 0x18))
    {
        prompt_play_by_cmd_id(flame_screen-0x11+FLAME_COLORFUL, -1, default_play_done_callback,true); 
        uart_send_asr(flame_screen-0x11+FLAME_COLORFUL);
        warmer_dev.flame_screen = flame_screen;
    }
}

void warmer_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;

    switch (msg.function_id)
    {
        case WARMER_POWER:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关取暖器\r\n");       
                prompt_play_by_cmd_id(TURN_OFF, -1, default_play_done_callback,true);      
                uart_send_asr(TURN_OFF);  
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("开取暖器\r\n");
                prompt_play_by_cmd_id(TURN_ON, -1, default_play_done_callback,true);  
                uart_send_asr(TURN_ON);     
            }
            warmer_dev.power =  msg.data[0];
            break;
        }
        case WARMER_SHAKE:
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
            warmer_dev.shake = msg.data[0];
            break;
        }
        case WARMER_ANION:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭负离子\r\n");
                prompt_play_by_cmd_id(ANION_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(ANION_OFF);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开负离子\r\n");       
                prompt_play_by_cmd_id(ANION_ON, -1, default_play_done_callback,true);   
                uart_send_asr(ANION_ON);         
            }
            warmer_dev.anion = msg.data[0];
            break;
        }
        case WARMER_ENERGY:
        {
           if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭节能\r\n");
                prompt_play_by_cmd_id(ENERGY_OFF, -1, default_play_done_callback,true);
                uart_send_asr(ENERGY_OFF);         
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开节能\r\n");       
                prompt_play_by_cmd_id(ENERGY_ON, -1, default_play_done_callback,true);  
                uart_send_asr(ENERGY_ON);
                warmer_dev.sleep = FUN_TURN_OFF;
                warmer_dev.dry = FUN_TURN_OFF;        
            }
            warmer_dev.energy = msg.data[0];
            break;
        }
        case WARMER_SLEEP:
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
                warmer_dev.energy = FUN_TURN_OFF;
                warmer_dev.dry = FUN_TURN_OFF;    
            }
            warmer_dev.sleep = msg.data[0];
            break;
        }
        case WARMER_SCREEN:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭屏显\r\n");
                prompt_play_by_cmd_id(SCREEN_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(SCREEN_OFF);        
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开屏显\r\n");       
                prompt_play_by_cmd_id(SCREEN_ON, -1, default_play_done_callback,true); 
                uart_send_asr(SCREEN_ON);       
            }
            warmer_dev.screen = msg.data[0];
            break;
        }
        case WARMER_LIGHT:
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
            warmer_dev.light = msg.data[0];
            break;
        }
        case WARMER_GEARS:
        {
            if (msg.data[0] == 0x01)
            {
                mprintf("打开低档\r\n");
                prompt_play_by_cmd_id(LOW_ON, -1, default_play_done_callback,true);  
                uart_send_asr(LOW_ON);       
            }
            else if (msg.data[0] == 0x02)
            {
                mprintf("打开中档\r\n");       
                prompt_play_by_cmd_id(MIDDLE_ON, -1, default_play_done_callback,true); 
                uart_send_asr(MIDDLE_ON);           
            }
            else if (msg.data[0] == 0x03)
            {
                mprintf("打开高档\r\n");       
                prompt_play_by_cmd_id(HIGH_ON, -1, default_play_done_callback,true);   
                uart_send_asr(HIGH_ON);         
            }
            warmer_dev.gears = msg.data[0];
            break;
        }
        case WARMER_DRY:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭干衣模式 ：%d\r\n",msg.data[0]);
                prompt_play_by_cmd_id(DRY_OFF, -1, default_play_done_callback,true);   
                uart_send_asr(DRY_OFF);     
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开干衣模式:%d\r\n", msg.data[0]);       
                prompt_play_by_cmd_id(DRY_ON, -1, default_play_done_callback,true);    
                uart_send_asr(DRY_ON);
                warmer_dev.energy = FUN_TURN_OFF;
                warmer_dev.sleep = FUN_TURN_OFF;       
            }
            warmer_dev.dry = msg.data[0];
            break;
        }
        case WARMER_DISINFECTION:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭消毒\r\n");
                prompt_play_by_cmd_id(DISINFECTION_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(DISINFECTION_OFF);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开消毒\r\n");       
                prompt_play_by_cmd_id(DISINFECTION_ON, -1, default_play_done_callback,true);   
                uart_send_asr(DISINFECTION_ON);         
            }
            warmer_dev.disinfection = msg.data[0];
            break;
        }
        case WARMER_TIMING:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭定时\r\n");
                prompt_play_by_cmd_id(TIMING_OFF, -1, default_play_done_callback,true);   
                uart_send_asr(TIMING_OFF);   
                warmer_dev.timing = 0;   
                xTimerStop(warmer_timer,0);
            }
            else
            {
                set_warmer_timing(msg.data[0]);              
            }
            break;
        }
        case WARMER_TEMPERATURE:
        {
            if (msg.data[0] == 0xF1)
            {
                mprintf("升高温度\r\n");
                prompt_play_by_cmd_id(TEMPERATURE_RAISE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_RAISE);   
                if (warmer_dev.tempreature < DEV_TEMPERATURE_MAX)
                {
                    warmer_dev.tempreature ++;
                }
            }
            else if (msg.data[0] == 0xF2)
            {
                mprintf("降低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_REDUCE, -1, default_play_done_callback,true); 
                uart_send_asr(TEMPERATURE_REDUCE);    
                if (warmer_dev.tempreature > DEV_TEMPERATURE_MIN)
                {
                    warmer_dev.tempreature --;
                }       
            }
            else if (msg.data[0] == 0xF3)
            {
                mprintf("最高温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MAX, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MAX);   
                warmer_dev.tempreature = DEV_TEMPERATURE_MAX;      
            }
            else if (msg.data[0] == 0xF4)
            {
                mprintf("最低温度\r\n");       
                prompt_play_by_cmd_id(TEMPERATURE_MIN, -1, default_play_done_callback,true);  
                uart_send_asr(TEMPERATURE_MIN);    
                warmer_dev.tempreature = DEV_TEMPERATURE_MIN;     
            }  
            else          
            {
                set_warmer_temperature(msg.data[0]);        
            }
            break;
        }
        case WARMER_HUMIDITY:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭加湿\r\n");
                prompt_play_by_cmd_id(HUMIDITY_OFF, -1, default_play_done_callback,true);  
                uart_send_asr(HUMIDITY_OFF);       
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开加湿\r\n");       
                prompt_play_by_cmd_id(HUMIDITY_ON, -1, default_play_done_callback,true);  
                uart_send_asr(HUMIDITY_ON);          
            }
            else if ((msg.data[0] >= DEV_HUMIDITY_MIN)&&(msg.data[0] <= DEV_HUMIDITY_MAX))
            { 
                set_warmer_humidity(msg.data[0]);  
            }
            else if (msg.data[0] == 0xF1)
            {
                mprintf("升高湿度\r\n");
                prompt_play_by_cmd_id(HUMIDITY_RAISE, -1, default_play_done_callback,true); 
                uart_send_asr(HUMIDITY_RAISE);   
                if (warmer_dev.humidity < DEV_HUMIDITY_MAX)
                {
                    warmer_dev.humidity+=5;
                }
            }
            else if (msg.data[0] == 0xF2)
            {
                mprintf("降低湿度\r\n");       
                prompt_play_by_cmd_id(HUMIDITY_REDUCE, -1, default_play_done_callback,true); 
                uart_send_asr(HUMIDITY_REDUCE);    
                if (warmer_dev.humidity > DEV_HUMIDITY_MIN)
                {
                    warmer_dev.humidity-=5;
                }       
            }
            else if (msg.data[0] == 0xF3)
            {
                mprintf("最高湿度\r\n");       
                prompt_play_by_cmd_id(HUMIDITY_MAX, -1, default_play_done_callback,true);  
                uart_send_asr(HUMIDITY_MAX);   
                warmer_dev.humidity = DEV_HUMIDITY_MAX;      
            }
            else if (msg.data[0] == 0xF4)
            {
                mprintf("最低湿度\r\n");       
                prompt_play_by_cmd_id(HUMIDITY_MIN, -1, default_play_done_callback,true);  
                uart_send_asr(HUMIDITY_MIN);    
                warmer_dev.humidity = DEV_HUMIDITY_MIN;     
            }  
            break;
        }
        case WARMER_FLAME_SCREEN:
        {
            if (msg.data[0] == FUN_TURN_OFF)
            {
                mprintf("关闭焰火\r\n");
                prompt_play_by_cmd_id(FLAME_OFF, -1, default_play_done_callback,true); 
                uart_send_asr(FLAME_OFF);   
                warmer_dev.flame_screen = msg.data[0];     
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                mprintf("打开焰火\r\n");       
                prompt_play_by_cmd_id(FLAME_ON, -1, default_play_done_callback,true);  
                uart_send_asr(FLAME_ON);    
                warmer_dev.flame_screen = msg.data[0];
            }
            else 
            {      
                set_warmer_flame_screen(msg.data[0]);   
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

void warmer_query(ble_msg_V1_t msg)
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
    send_data[4] = WARMER_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x41;
    send_data[7] = msg.function_id;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    
    uint8_t *data = (uint8_t *)&warmer_dev;
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

