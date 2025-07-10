/**
 * @file ir_remote_driver.c
 * @brief  红外驱动 need one timer,one pwm, pwm 38khz
 * @version 1.0
 * @date 2017-08-22
 *
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 *
 */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "ci_log.h"
#include "ci130x_pwm.h"
#include "ci130x_dpmu.h"
#include "ci130x_gpio.h"
#include "ci130x_timer.h"
#include "ci130x_spiflash.h"
#include "ci130x_core_eclic.h"
#include "platform_config.h"
#include "sdk_default_config.h"
#include "ir_remote_driver.h"
/**************************************************************************
                    type define
****************************************************************************/

#define IR_CODE_BUSY_STATE 1
#define IR_CODE_IDLE_STATE 0
#define IR_CARRIER_FREQUENCY (38000)    //载波频率
#define IR_CARRIER_DUTY (300)           //占空比
#define IR_CARRIER_DUTY_MAX (1000)           //最大占空比
#define IR_MAX_DATA_COUNT (1024)        //最大接收长度

#define TIMER0_ONEUS_COUNT (get_apb_clk() / 1000000)/*FPCLK/FEQDIV*/

#define IR_RECEIVE_DATA_TIMEOUT         (100000L)
#define IR_RECEIVE_DATA_TIMEOUT_USED    (100000L)
#define IR_RECEIVE_FIRST_LEVEL          (0)

typedef enum
{
    IR_RECEIVE_STATE_IDLE = 0,
    IR_RECEIVE_STATE_INIT,
    IR_RECEIVE_STATE_NOINPUT,/*timeout 5s*/
    IR_RECEIVE_STATE_START,
    IR_RECEIVE_STATE_DATA,
    IR_RECEIVE_STATE_END,/*timeout 20000us, ir code datebase used this value*/
    IR_RECEIVE_STATE_ERR,/*now if receive too short will error*/
}ir_receive_state_t;



/**************************************************************************
                    global
****************************************************************************/
static uint32_t ir_time_function = 1;/*1:send , 0:receive*/

static uint32_t ir_code_total_count = 0;
static uint32_t ir_code_send_count = 0;

static uint16_t *ir_level_code = NULL;
static uint32_t ir_level_code_size = 0;
static uint8_t odd_even_carry_pwm_wave = 1;

static uint32_t ir_receive_state = IR_RECEIVE_STATE_IDLE;
static uint32_t ir_receive_level_flag = IR_RECEIVE_FIRST_LEVEL;/*1 high ,0 low*/
uint32_t ir_receive_level_count = 0;

static void ir_receive_process(void);
static gpio_irq_callback_list_t ir_gpio_callback = {ir_receive_process,NULL};
static bool hw_Init = false;

static IrRemoteState ir_state =
{
    .is_busy = false,
    .event = IR_IDEL
};



/*红外事件回调处理*/
static ir_remote_event_callback_t g_ir_callback = NULL;

stIrPinInfo ir_driver_info =
{
    .outPin =
    {
        .PinName = IR_OUT_PWM_PIN_NAME,
        .GpioBase = IR_OUT_GPIO_PIN_BASE,
        .PinNum = IR_OUT_PWM_PIN_NUMBER,
        .PwmFun = IR_OUT_PWM_FUNCTION,
        .IoFun = IR_OUT_GPIO_FUNCTION,
        .PwmBase = IR_OUT_PWM_NAME,
    },
    .revPin =
    {
        .PinName = IR_REV_IO_PIN_NAME,
        .GpioBase = IR_REV_IO_PIN_BASE,
        .PinNum = IR_REV_IO_PIN_NUMBER,
        .IoFun = IR_REV_IO_FUNCTION,
        .GpioIRQ = IR_REV_IO_IRQ,
    },

    .irTimer =
    {
        .ir_use_timer = IR_USED_TIMER_NAME,
        .ir_use_timer_IRQ = IR_USED_TIMER_IRQ,
    }
};


/**
 * @brief 注册IR事件回调函数
 *
 * @param ir_callback 回调函数
 */
void registe_ir_remote_callback(ir_remote_event_callback_t ir_callback)
{
    g_ir_callback = ir_callback;
}


/**
 * @brief 注销IR事件回调函数
 *
 */
void unregiste_ir_remote_callback(void)
{
    g_ir_callback = NULL;
}




/**
 * @brief 回调函数是否有效
 *
 * @return true 有效
 * @return false 无效
 */
static bool is_callback_vaild(void)
{
    if(g_ir_callback != NULL)
    {
        return true;
    }
    return false;
}



/**
 * @brief pwm管脚输出使能
 *
 */
static void ir_pwm_out_pad_enable(void)
{
    pwm_start(ir_driver_info.outPin.PwmBase);
    dpmu_set_io_reuse(ir_driver_info.outPin.PinName,ir_driver_info.outPin.PwmFun);
}


/**
 * @brief pwm管脚输出关闭
 *
 */
static void ir_pwm_out_pad_disable(void)
{
    pwm_stop(ir_driver_info.outPin.PwmBase);
    dpmu_set_io_direction(ir_driver_info.outPin.PinName, DPMU_IO_DIRECTION_OUTPUT);
    dpmu_set_io_reuse(ir_driver_info.outPin.PinName,ir_driver_info.outPin.IoFun);/*gpio function*/
    gpio_set_output_level_single(ir_driver_info.outPin.GpioBase, ir_driver_info.outPin.PinNum, 0);
}


/**
 * @brief 数据发送底半部
 *
 */
static void send_ir_code_continue(void)
{
    bool send_end_flag = false;

    /*发送结束*/
    if(ir_code_send_count >= ir_code_total_count)
    {
        send_end_flag = true;
        ir_state.event = IR_SEND_END;
        goto callback;
    }

    /*数据错误*/
    if(100 > ir_level_code[ir_code_send_count])
    {
        send_end_flag = true;
        ir_state.event = IR_SEND_DATA_ERR;
        ci_logerr(LOG_IR,"ir data error,wave value is 0\n");
        goto callback;
    }

    timer_stop(ir_driver_info.irTimer.ir_use_timer);
    timer_set_count(ir_driver_info.irTimer.ir_use_timer, (uint32_t)ir_level_code[ir_code_send_count]*TIMER0_ONEUS_COUNT*IR_DATA_DIV_COEFFICIENT);
    timer_start(ir_driver_info.irTimer.ir_use_timer);
    if(0 == (ir_code_send_count & odd_even_carry_pwm_wave))/*high level*/
    {
        ir_pwm_out_pad_enable();
    }
    else
    {
        ir_pwm_out_pad_disable();
    }
    ir_code_send_count++;

callback:
    /*结束处理*/
    if(send_end_flag)
    {
        timer_stop(ir_driver_info.irTimer.ir_use_timer);
        ir_pwm_out_pad_disable();
        ir_state.is_busy = false;
        /*调用回调*/
        if(is_callback_vaild())
        {
            g_ir_callback(&ir_state);
        }
        return;
    }
}


/**
 * @brief 数据接收结束
 *
 */
void ir_receive_end(void)
{
    uint32_t i;

    ir_state.is_busy = false;
    ir_state.event = IR_RECEIVE_END;

    ir_time_function = 1;
    gpio_irq_mask(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);
    //gpio_set_output_mode(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);

    #if 0
    for(i=0;i<ir_receive_level_count;i++)
    {
        if(0 == (i%16))
        {
            ci_loginfo(LOG_IR,"\n");
        }
        ci_loginfo(LOG_IR,"%d ",ir_level_code[i]*IR_DATA_DIV_COEFFICIENT);
    }
    #endif

    if(16 > ir_receive_level_count)
    {
        ci_loginfo(LOG_IR,"rev error, too short IR_RECEIVE_STATE_ERR\n");
        ir_receive_state = IR_RECEIVE_STATE_ERR;
        ir_state.event = IR_RECEIVE_SHORT_ERR;
    }

    /*调用回调*/
    if(is_callback_vaild())
    {
        g_ir_callback(&ir_state);
    }

}

/**
 * @brief 数据接收超时处理
 *
 */
static void ir_receive_timeout_deal(void)
{
//    int msg = 0;
    switch(ir_receive_state)
    {
        case IR_RECEIVE_STATE_INIT:
            ci_logerr(LOG_IR,"no input signal\n");
            ir_receive_state = IR_RECEIVE_STATE_NOINPUT;
            ir_receive_end();
            /*send a meassge,play error voice*/
            break;
        case IR_RECEIVE_STATE_DATA:
            ir_receive_state = IR_RECEIVE_STATE_END;
            /*ir receive done*/
            ir_level_code[ir_receive_level_count] = IR_RECEIVE_DATA_TIMEOUT/IR_DATA_DIV_COEFFICIENT;
            ir_receive_level_count++;
            ir_receive_end();
            break;
        default:
            break;
    }
}


/**
 * @brief timer中断处理函数
 *
 */
static void ir_timer_timeout_deal(void)
{
    timer_clear_irq(ir_driver_info.irTimer.ir_use_timer);

    if(1 == ir_time_function)
    {
        send_ir_code_continue();
    }
    else
    {
        ir_receive_timeout_deal();
    }
}


/**
 * @brief ir硬件初始化
 *
 * @retval RETURN_OK 初始化成功
 * @retval RETURN_ERR 初始化失败
 */
int32_t ir_hw_init(void)
{
    pwm_init_t pwm_38k_init;
    timer_init_t timer0_init;


    scu_set_device_gate(ir_driver_info.outPin.PwmBase, ENABLE);
    dpmu_set_io_reuse(ir_driver_info.outPin.PinName, ir_driver_info.outPin.PwmFun);
    pwm_38k_init.clk_sel    = 0;    //apb时钟
    pwm_38k_init.freq       = IR_CARRIER_FREQUENCY;
    pwm_38k_init.duty       = IR_CARRIER_DUTY;
    pwm_38k_init.duty_max   = IR_CARRIER_DUTY_MAX;
    pwm_init(ir_driver_info.outPin.PwmBase, pwm_38k_init);
    pwm_set_restart_md(ir_driver_info.outPin.PwmBase, ENABLE);   //处理毛刺
    ir_pwm_out_pad_disable();

    //配置TIMER0的中断
    __eclic_irq_set_vector(ir_driver_info.irTimer.ir_use_timer_IRQ, (int)ir_timer_timeout_deal);
    eclic_irq_enable(ir_driver_info.irTimer.ir_use_timer_IRQ);
    scu_set_device_gate(ir_driver_info.irTimer.ir_use_timer, ENABLE);
    timer0_init.mode  = timer_count_mode_single; /*one shot mode*/
    timer0_init.div   = timer_clk_div_0;/*1us = 50/2 period*/
    timer0_init.width = timer_iqr_width_f;
    timer0_init.count = TIMER0_ONEUS_COUNT * IR_RECEIVE_DATA_TIMEOUT;
    timer_init(ir_driver_info.irTimer.ir_use_timer, timer0_init);

    /* IR receive IO */

    gpio_irq_mask(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);
    scu_set_device_gate(ir_driver_info.revPin.GpioBase, ENABLE);
    dpmu_set_io_pull(ir_driver_info.revPin.PinName, DPMU_IO_PULL_DISABLE);
    dpmu_set_io_reuse(ir_driver_info.revPin.PinName, ir_driver_info.revPin.IoFun);/*gpio function*/
    gpio_set_input_mode(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);
    gpio_irq_unmask(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);
    gpio_irq_trigger_config(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum, both_edges_trigger);
    gpio_clear_irq_single(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);
    registe_gpio_callback(ir_driver_info.revPin.GpioBase, &ir_gpio_callback);
    eclic_irq_enable(ir_driver_info.revPin.GpioIRQ);
    gpio_irq_mask(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);

    hw_Init = true;

    return RETURN_OK;
}



/**
 * @brief gpio中断处理函数
 *
 */
static void ir_receive_process(void)
{
    uint32_t count = 0;

    if(gpio_get_irq_mask_status_single(ir_driver_info.revPin.GpioBase,ir_driver_info.revPin.PinNum))
    {
        timer_get_count(ir_driver_info.irTimer.ir_use_timer,(unsigned int*)(&count));

        timer_stop(ir_driver_info.irTimer.ir_use_timer);
        timer_set_count(ir_driver_info.irTimer.ir_use_timer,TIMER0_ONEUS_COUNT*IR_RECEIVE_DATA_TIMEOUT);
        timer_start(ir_driver_info.irTimer.ir_use_timer);

        if(IR_RECEIVE_STATE_INIT == ir_receive_state)
        {
            ir_receive_state = IR_RECEIVE_STATE_DATA;
            ir_receive_level_flag = IR_RECEIVE_FIRST_LEVEL;
        }
        else
        {
            if (ir_receive_level_count >= (IR_MAX_DATA_COUNT - 1))
            {
                ir_receive_state = IR_RECEIVE_STATE_END;
                ir_receive_end();
            }
            else
            {

                ir_receive_level_flag ^= 1;

                count = TIMER0_ONEUS_COUNT*IR_RECEIVE_DATA_TIMEOUT - count;
                ir_level_code[ir_receive_level_count] = count/TIMER0_ONEUS_COUNT/IR_DATA_DIV_COEFFICIENT;
                ir_receive_level_count ++;
            }
        }

        if(ir_receive_level_flag != gpio_get_input_level_single(ir_driver_info.revPin.GpioBase,ir_driver_info.revPin.PinNum))
        {
            ci_logerr(LOG_IR,"receive_level error,need retry\n");
            ir_receive_state = IR_RECEIVE_STATE_ERR;
            ir_receive_level_count = 0;
            ir_receive_end();
        }

    }

}


/**
 * @brief 红外收发硬件管脚配置，定时器配置
 *
 * @param pIrPinInfo
 * @retval RETURN_OK 配置成功
 * @retval RETURN_ERR 配置失败
 */
int32_t ir_setPinInfo(stIrPinInfo* pIrPinInfo)
{
    if (NULL == pIrPinInfo)
    {
        return RETURN_ERR;
    }

    memcpy((void*)&ir_driver_info,(void*)pIrPinInfo,sizeof(stIrPinInfo));
    return RETURN_OK;
}


/**
 * @brief 配置数据buf地址
 *
 * @param addr buf地址
 * @param size 大小
 * @retval RETURN_OK 配置成功
 * @retval RETURN_ERR 配置失败
 */
int32_t set_ir_level_code_addr(uint32_t addr, uint32_t size)
{
    if (size < (1024*2))
    {
        return RETURN_ERR;
    }

    ir_level_code_size = size;
    ir_level_code = (uint16_t *)addr;

    return RETURN_OK;
}

/**
 * @brief 配置数据buf地址
 *
 * @param addr buf地址
 * @param size 大小
 * @retval RETURN_OK 配置成功
 * @retval RETURN_ERR 配置失败
 */
int32_t set_ir_level_code_addr_for_test(uint32_t addr, uint32_t size)
{
   /* if (size < (1024*2))
    {
        return RETURN_ERR;
    }*/

    ir_level_code_size = size;
    ir_level_code = (uint16_t *)addr;

    return RETURN_OK;
}

/**
 * @brief 获取数据buf地址
 *
 * @return uint16_t* buf地址
 */
uint16_t *get_ir_level_code_addr(void)
{
    return ir_level_code;
}


/**
 * @brief 数据发送初始化
 *
 */
void ir_send_init(void)
{
    if (false == hw_Init)
    {
        ir_hw_init();
        hw_Init = true;
    }

    ir_code_send_count = 0;
    ir_code_total_count = 0;
	ir_time_function = 1;
    ir_state.is_busy = false;
    memset(ir_level_code,0,ir_level_code_size);/*mask for debug*/
}


/**
 * @brief 获取ir数据buf地址
 *
 * @return uint16_t* buf地址
 */
uint16_t * get_ir_driver_buf(void)
{
    if(true == ir_state.is_busy)/*busy state*/
    {
        return NULL;
    }
    else
    {
        return ir_level_code;
    }
}


/**
 * @brief 红外数据发送
 * @note no support multi thread,used this only one task!
 * @param count uint32_t freq,uint32_t dutycycle  now just 38khz 50%
 *
 * @retval RETURN_OK 发送成功
 * @retval RETURN_ERR 发送失败
 */
int32_t send_ir_code_start(uint32_t count)
{
    int ret = RETURN_OK;
    /*if busy just discard message, demo code no return error*/
    if(true == ir_state.is_busy)
    {
        ir_state.event = IR_EVENT_ERR;
        ret = RETURN_ERR;
    }
    else
    {
        ir_state.is_busy = true;
        ir_state.event = IR_SEND_START;
    }

    #if 0
    mprintf("TX:\n");
    for (size_t i = 0; i < count; i++)
    {
        mprintf("%d ", ir_level_code[i]);
    }
    mprintf("\n");
    #endif

    /*调用回调*/
    if(is_callback_vaild())
    {
        g_ir_callback(&ir_state);
    }

    if(ir_state.event == IR_SEND_START)
    {
        ir_code_send_count = 0;
        ir_code_total_count = count;
        ci_loginfo(LOG_IR,"read ir ok\n");
        send_ir_code_continue();
    }

    return ret;
}


/**
 * @brief 红外数据开始接收
 *
 */
void ir_receive_start(int time_out)
{
    if(ir_state.is_busy)
    {
        ir_state.event = IR_EVENT_ERR;
    }
    else
    {
        ir_state.is_busy = true;
        ir_state.event = IR_RECEIVE_START;
    }

    /*调用回调*/
    if(is_callback_vaild())
    {
        g_ir_callback(&ir_state);
    }


    if(ir_state.event == IR_RECEIVE_START)
    {
        ir_time_function = 0;

        /*io input mode,interrupt enable*/
        gpio_set_input_mode(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);
        gpio_irq_unmask(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);
        gpio_clear_irq_single(ir_driver_info.revPin.GpioBase, ir_driver_info.revPin.PinNum);

        timer_stop(ir_driver_info.irTimer.ir_use_timer);
        timer_set_count(ir_driver_info.irTimer.ir_use_timer,time_out*1000*TIMER0_ONEUS_COUNT);
        timer_start(ir_driver_info.irTimer.ir_use_timer);

        ir_receive_state = IR_RECEIVE_STATE_INIT;
        ir_receive_level_flag = IR_RECEIVE_FIRST_LEVEL;
        ir_receive_level_count = 0;
    }


}


/**
 * @brief 检查红外接收是否正确
 *
 * @retval RETURN_OK 接收正确
 * @retval RETURN_ERR 接收出错
 */
int32_t check_ir_receive(void)
{
    if((IR_RECEIVE_STATE_NOINPUT == ir_receive_state)\
    ||(IR_RECEIVE_STATE_ERR == ir_receive_state)\
    ||(IR_RECEIVE_STATE_INIT == ir_receive_state))
    {
        ci_loginfo(LOG_IR,"check receive error\n");
        return RETURN_ERR;
    }
    ci_loginfo(LOG_IR,"check receive ok\n");
    return RETURN_OK;
}


/**
 * @brief 检查红外发送状态是否空闲
 *
 * @retval RETURN_OK 非空闲
 * @retval RETURN_ERR 空闲
 */
int32_t check_ir_busy_state(void)
{
    if(true == ir_state.is_busy)
    {
        return RETURN_OK;
    }
    else
    {
        return RETURN_ERR;
    }
}

uint32_t get_receive_level_count(void)
{
    return ir_receive_level_count;
}

/**
 * @brief 设置接收数据长度
 *
 * @param level_cnt
 */
void set_receive_level_count(uint32_t level_cnt)
{
    ir_receive_level_count = level_cnt;
}

/*设置奇数还是偶数电平载波*/
int32_t set_odd_even_carry_pwm_wave(int odd_even)
{
    odd_even_carry_pwm_wave = (1 == (odd_even%2)) ? 1 : 0;
    return 0;
}
