/**
 * @file ir_remote_driver.h
 * @brief  红外驱动
 * @version 1.0
 * @date 2017-08-22
 *
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 *
 */
#ifndef _IR_REMOTE_H_
#define _IR_REMOTE_H_


#include "ci130x_uart.h"
#include "ci130x_scu.h"
#include "ci130x_gpio.h"
#include "ci130x_pwm.h"
#include "ci130x_timer.h"
#include "ci_log_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    PinPad_Name PinName;
    gpio_base_t GpioBase;
    gpio_pin_t PinNum;
    IOResue_FUNCTION PwmFun;
    IOResue_FUNCTION IoFun;
    pwm_base_t PwmBase;
}stIrOutIoInfo;

typedef struct
{
    PinPad_Name PinName;
    gpio_base_t GpioBase;
    gpio_pin_t PinNum;
    IOResue_FUNCTION IoFun;
    IRQn_Type GpioIRQ;
}stIrRevIoInfo;

typedef struct
{
    timer_base_t ir_use_timer;
    IRQn_Type ir_use_timer_IRQ;
}stIrTimerInfo;

typedef struct
{
    stIrOutIoInfo outPin;
    stIrRevIoInfo revPin;
    stIrTimerInfo irTimer;
}stIrPinInfo;


enum IR_LOG_LEVEL
{
    IR_LOG_ERROR = 0,
    IR_LOG_WARN = 1,
    IR_LOG_DEBUG = 2,
    IR_LOG_INFO,
};

typedef enum
{
    IR_IDEL = 0,               //空闲
    IR_SEND_START,             //开始发送
    IR_SEND_END,               //发送结束
    IR_RECEIVE_START,          //开始接收
    IR_RECEIVE_END,            //结束接收
    IR_EVENT_ERR = -1,         //错误事件
    IR_SEND_DATA_ERR = -2,     //发送数据错误
    IR_RECEIVE_SHORT_ERR = -3, //接收数据太短错误

} IrRemoteEvent;

typedef struct
{
    bool is_busy;        //忙碌
    IrRemoteEvent event; //事件
} IrRemoteState;

typedef void (*ir_remote_event_callback_t)(IrRemoteState *state);

/*ir send and receive config*/
#define IR_OUT_PWM_PIN_NAME                      PA2
#define IR_OUT_GPIO_PIN_BASE                     PA
#define IR_OUT_PWM_NAME                          PWM0
#define IR_OUT_PWM_PIN_NUMBER                    pin_2
#define IR_OUT_PWM_FUNCTION                      FIFTH_FUNCTION
#define IR_OUT_GPIO_FUNCTION                     FIRST_FUNCTION

#define IR_REV_IO_PIN_NAME                       PA4
#define IR_REV_IO_PIN_BASE                       PA
#define IR_REV_IO_PIN_NUMBER                     pin_4
#define IR_REV_IO_IRQ                            PA_IRQn
#define IR_REV_IO_FUNCTION                       FIRST_FUNCTION

#define TIMER0_PRIORITY_PREEMPTION               (3)
#define TIMER0_PRIORITY_SUB                      (0)

#define GPIO0_PRIORITY_PREEMPTION                (3)
#define GPIO0_PRIORITY_SUB                       (0)

#define IR_USED_TIMER_NAME                       TIMER0
#define IR_USED_TIMER_IRQ                        TIMER0_IRQn
#define IR_USED_TIMER_IRQ_PRIORITY_PREEMPTION    TIMER0_PRIORITY_PREEMPTION
#define IR_USED_TIMER_IRQ_PRIORITY_SUB           TIMER0_PRIORITY_SUB

#define IR_REV_IO_IRQ_PRIORITY_PREEMPTION        GPIO0_PRIORITY_PREEMPTION
#define IR_REV_IO_IRQ_PRIORITY_SUB               GPIO0_PRIORITY_SUB
#define IR_DATA_DIV_COEFFICIENT (2)


/**
 * @addtogroup ir
 * @{
 */

/* 初始化API */
int32_t ir_hw_init(void);
int32_t ir_setPinInfo(stIrPinInfo* pIrPinInfo);
int32_t set_ir_level_code_addr(uint32_t addr, uint32_t size);
int32_t set_ir_level_code_addr_for_test(uint32_t addr, uint32_t size);

/* 发送API */
void ir_send_init(void);
int32_t send_ir_code_start(uint32_t count);

/* 接收API */
void ir_receive_start(int time_out);
int32_t check_ir_receive(void);
void ir_receive_end(void);

/* 状态查询API */
uint16_t *get_ir_level_code_addr(void);
uint16_t * get_ir_driver_buf(void);
int32_t check_ir_busy_state(void);
uint32_t get_receive_level_count(void);
void set_receive_level_count(uint32_t level_cnt);

/*设置奇数还是偶数电平载波*/
int32_t set_odd_even_carry_pwm_wave(int odd_even);

/**
 * @brief 注册IR事件回调函数
 *
 * @param ir_callback 回调函数
 */
void registe_ir_remote_callback(ir_remote_event_callback_t ir_callback);

/**
 * @brief 注销IR事件回调函数
 *
 */
void unregiste_ir_remote_callback(void);

/** @} */


#ifdef __cplusplus
}
#endif

#endif

