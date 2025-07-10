#include "ci_key.h"
#include "ci_log.h"
#include "ci130x_gpio.h"
#include "ci130x_dpmu.h"
#include "system_msg_deal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"



/**
 * @brief 初始化按键，主要初始化ADC
 * 
 */
void ci_key_io_init(void)
{    
    ////////////////CI1302
    scu_set_device_gate((unsigned int)RTC_UPLINK_SWITCH_GPIO_BASE,ENABLE);       //开启时钟
    dpmu_set_io_reuse(RTC_UPLINK_SWITCH_GPIO,FIRST_FUNCTION);              //设置引脚功能复用为GPIO
    dpmu_set_io_direction(RTC_UPLINK_SWITCH_GPIO,DPMU_IO_DIRECTION_INPUT); //设置引脚功能为输入模式
    dpmu_set_io_pull(RTC_UPLINK_SWITCH_GPIO,DPMU_IO_PULL_UP);         //设置关闭上下拉
    gpio_set_input_mode(RTC_UPLINK_SWITCH_GPIO_BASE,RTC_UPLINK_SWITCH_GPIO_PIN); 

    scu_set_device_gate((unsigned int)RTC_DENOISE_FUNC_GPIO_BASE,ENABLE);       //开启时钟
    dpmu_set_io_reuse(RTC_DENOISE_FUNC_GPIO,FIRST_FUNCTION);              //设置引脚功能复用为GPIO
    dpmu_set_io_direction(RTC_DENOISE_FUNC_GPIO,DPMU_IO_DIRECTION_INPUT); //设置引脚功能为输入模式
    dpmu_set_io_pull(RTC_DENOISE_FUNC_GPIO,DPMU_IO_PULL_UP);         //设置关闭上下拉
    gpio_set_input_mode(RTC_DENOISE_FUNC_GPIO_BASE,RTC_DENOISE_FUNC_GPIO_PIN); 
}


/**
 * @brief a sample code key infomation printf
 * 
 * @param key_msg 
 */
void key_info_show(sys_msg_key_data_t *key_msg)
{    
    mprintf("HEMY-%s %d\n",__FUNCTION__,__LINE__);
    if(key_msg->key_index != KEY_NULL)
    {
        ci_loginfo(LOG_USER,"key_value is 0x%x ",key_msg->key_index);
        if(MSG_KEY_STATUS_PRESS == key_msg->key_status)
        {
            ci_loginfo(LOG_USER,"status : press down\n");
        }
        else if(MSG_KEY_STATUS_PRESS_LONG == key_msg->key_status)
        {
            ci_loginfo(LOG_USER,"status : long press\n");
        }
        else if(MSG_KEY_STATUS_RELEASE == key_msg->key_status)
        {
            ci_loginfo(LOG_USER,"status : release\n");
        }
        if(key_msg->key_status == MSG_KEY_STATUS_RELEASE)
        {
            ci_loginfo(LOG_USER,"按键时间为：%d ms\n",key_msg->key_time_ms);
        }
        /*user code*/                
    }
}


TimerHandle_t   AutoReloadTimer_Handle;//hemy-定时器   先创建定时器名
void HEMY_Timer_Create()
{
    AutoReloadTimer_Handle = xTimerCreate
    ((const char *)"AutoReloadTimer",//软件定时器名字，字符串，用于调试使用
    (TickType_t )200, //MS  节拍数，ms单位
    (UBaseType_t    )pdTRUE, //pdTRUE(周期模式)，pdFALSE（单次模式）
    (void*     )1,//定时器ID号，在回调函数中根据定时器ID号来处理不同的定时器，。如果不需要使用则传递NULL即可//具体的也没怎么用过。
    (TimerCallbackFunction_t)AutoReloadCallback);// hemy 定时器回调函数，当时间到了后就会调用函数
}


//回调函数
void AutoReloadCallback( TimerHandle_t xTimer )
{
    if(0 == gpio_get_input_level_single(RTC_UPLINK_SWITCH_GPIO_BASE,RTC_UPLINK_SWITCH_GPIO_PIN))      //获取IO状态
    {
        //INFO:输入为低电平
        mprintf("左声道:  %s %d\n",__FUNCTION__,__LINE__);
        set_src_from_right_mic(false);
    }
    else
    {
        //INFO:输入为高电平
        mprintf("右声道: %s %d\n",__FUNCTION__,__LINE__);
        set_src_from_right_mic(true);
    }

    if(0 == gpio_get_input_level_single(RTC_DENOISE_FUNC_GPIO_BASE,RTC_DENOISE_FUNC_GPIO_PIN))      //获取IO状态
    {
        //INFO:输入为低电平
        mprintf("降噪开:  %s %d\n",__FUNCTION__,__LINE__);
        convert_left_and_right(false);
    }
    else
    {
        //INFO:输入为高电平
        mprintf("降噪关: %s %d\n",__FUNCTION__,__LINE__);
        convert_left_and_right(true);
    }
}


void HEMY_Timer_Create_START(void)
{
    BaseType_t errerr=xTimerStart(AutoReloadTimer_Handle,0);//HEMY开始调用定时器 有返回值 没成功返回pdFAIL
}


void HEMY_Timer_Create_STOP(void)
{
    mprintf("照明：%s   %d\n", __func__, __LINE__ ); 
    gpio_set_output_level_single(PB, pin_0, 0);
    BaseType_t errerr=xTimerStop(AutoReloadTimer_Handle,0);//HEMY 开始关闭定时器
}


void ci_key_init(void)
{
    /*adc 按键初始化*/
    ci_key_io_init();
    HEMY_Timer_Create();//定时器初始化
    HEMY_Timer_Create_START();//定时器开始
}
