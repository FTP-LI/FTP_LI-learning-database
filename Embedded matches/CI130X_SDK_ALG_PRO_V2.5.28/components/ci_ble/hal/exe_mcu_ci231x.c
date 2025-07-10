/**
 * @file exe_mcu_py32f003.c
 * @brief Host MCU PY32F003 HAL layer in EXE stack.
 * @date 2023/12/25
 * @author luoqiang
 *
 * @addtogroup HAL
 * @ingroup EXE
 * @details
 *
 * @{
 */

/*********************************************************************
 * INCLUDES
 */
#include <stdlib.h>
#include "ble_ll.h"
#include "exe_hal.h"
#include "exe_pkt.h"
#include "exe_api.h"
#include "exe_app.h"
#include "exe_hal_ci231x.h"
#include "exe_hal_pm.h"

#include "FreeRTOS.h"
#include "stdint.h"
#include "stdlib.h"
#include "ci_ble_spi.h"
#include "ci130x_gpio.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "ci130x_dpmu.h"
#include "ci130x_core_misc.h"
#include "ci130x_gpio.h"
/*********************************************************************
 * MACROS
 */
/**
 * @brief Define this macro to enable high resolution timer (<=1us),
 *        i.e. use SysTick, otherwise use RTC (buggy).
 */
#define HAL_TIMER_HIGH_RESOLUTION       1

#if HAL_TIMER_HIGH_RESOLUTION
#define HAL_HTIME_MASK                  0xFFFFFFFF
#define HAL_HTIME_HZ                    (120000000)
#else
#define HAL_HTIME_MASK                  0xFFFFFFFF
#define HAL_HTIME_HZ                    (32768)
#endif

///host time wrapback check
#define HAL_HTIME_HALF                  (HAL_HTIME_MASK >> 1)

/**
 * Checks whether the given time is older than the reference (occurs before in time).
 *
 * @verbatim
 -----------|---------------------|-----------> t
            t1                    t2
            time                  reference

 EXE_TIME_OLDER_THAN(t1, t2) => true
 EXE_TIME_OLDER_THAN(t2, t1) => false
 @endverbatim
 */
#define EXE_TIME_OLDER_THAN(time, ref) (bool)(((((uint32_t) (time)) - ((uint32_t) (ref))) & HAL_HTIME_MASK) > HAL_HTIME_HALF)

/**
 * @brief The extra cost when implement hal_delay_us().
 */
//#define PORT_CPU_DELAY_COUNTER_OVERHEAD  63 //-O0
#define PORT_CPU_DELAY_COUNTER_OVERHEAD  23 //-O3

/**
 * @brief The advance wakeup time for anchor point:
 *        MCU wakeup;wakeup; SPI traffic; RF setup =34
 */
#define HAL_XCVR_XTIME_ADVANCE_WAKEUP   EXE_US_TO_XTIME(2 + 1000 +257 +8+130)


extern uint32_t exe_bletick_wakeup_point;

/*******************************  RTOS相关函数  *******************************/
volatile bool hal_os_cr_flag = false;
void hal_os_cr_enter(void)
{
  portENTER_CRITICAL();  //关全局中断
  hal_os_cr_flag = true;
}
void hal_os_cr_exit(void)
{
  portEXIT_CRITICAL(); //开全局中断
  hal_os_cr_flag = false;
}


/**************************  系统time/clock相关函数  **************************/

uint32_t hal_us_to_htime(uint32_t us)
{
#if HAL_TIMER_HIGH_RESOLUTION
  return us * (HAL_HTIME_HZ/1000000);
#else
  return ((1ULL<<32) * HAL_HTIME_HZ / 1000000 * (us)) >> 32;
#endif
}
uint32_t hal_ms_to_htime(uint32_t ms)
{
  return ms * (HAL_HTIME_HZ/1000);
}
uint32_t hal_1d25ms_to_htime(uint32_t dms)
{
  return dms * (HAL_HTIME_HZ/1000) * 125 / 100;
}

uint32_t hal_xtime_to_htime(uint32_t xtime)
{
  return xtime * (HAL_HTIME_HZ / XCVR_XTIME_HZ);
}

static uint32_t rtc_read_time(void)
{
  
}
static void rtc_write_time(uint32_t time)
{
 
}

uint32_t hal_htime_get_current(void)
{

  /* RTC version, but the access to registers seems slow due to across clock domain. */
  //return timer3_get_time_us();
  return timer3_get_sys_tick();

}

void hal_htime_set_current(uint32_t htime)
{
 // rtc_write_time(htime);
}

uint32_t hal_htime_diff(uint32_t time1, uint32_t time2)
{
  if (((time1 - time2) & HAL_HTIME_MASK) > HAL_HTIME_HALF)
  {
    return time2 - time1;
  }
  else
  {
    return time1 - time2;
  }
}

bool hal_htime_elapsed(uint32_t ref_time, int32_t span_time)
{
  return EXE_TIME_OLDER_THAN(ref_time + span_time, hal_htime_get_current());
}
     
/*
 * @brief 毫秒级延时。
 * @param [in] ms - the delay time in milliseconds.
 *                  max 1398 ms due to 24-bit counter @6MHz.
 *                  max  524 ms due to 24-bit counter @16MHz.
 *                  max  262 ms due to 24-bit counter @32MHz.
 * @note 精度不用太高，可以偏大，但不能偏小
 */
void hal_delay_ms(int ms) 
{
#if 1
  uint32_t now, start = hal_htime_get_current();
  uint32_t delta, delay_cycles;
  if (ms >= 262) ms = 262;
  delay_cycles = hal_ms_to_htime(ms);
  do {
    now = hal_htime_get_current();
    delta = ((now - start) > HAL_HTIME_HALF) ? start-now : now-start;
  } while (delta < delay_cycles); 
#endif
  //Delay1ms(ms);
}

/**
 * @brief Poll wait at microseconds level. 微秒级延时。
 *
 * @param [in] us - the delay time in microseconds.
 *                  value range in [1, implementation limited].
 * @note 精度需要尽可能精确。
 */
RAM_CODE_SECTION void hal_delay_us(int us) 
{
#if 0
  uint32_t now, start = hal_htime_get_current();
  uint32_t delta, delay_cycles;
  delay_cycles = hal_us_to_htime(us);
  delay_cycles -= PORT_CPU_DELAY_COUNTER_OVERHEAD;
  if ((int)delay_cycles < 0) return;
  do {
    now = hal_htime_get_current();
    delta = ((now - start) > HAL_HTIME_HALF) ? start-now : now-start;
  } while (delta < delay_cycles); 
#endif
  Delay1us(us);
}

uint32_t bsp_wheel_scan(uint32_t xtime_duration)
{
#if !APP_MOUSE_AUTO_CIRCLE && (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
  extern void wheel_scan(void);
  if (0 == xtime_duration) {
    /* Scan once. */
    wheel_scan();
    return 0;
  } else {
    extern unsigned char wheel_count, wheel_data;
    /* 如果有滚动，启动密集扫描：1ms一次，持续8*4ms。*/
    if ((0 != wheel_count) &&
        (xtime_duration < XCVR_XTIME_HALF)) {
      uint8_t no_scan = 0;
      while (xtime_duration > 2/*FIXME*/) {
        wheel_scan();
        hal_delay_us(1000-18);
        xtime_duration -= 1;
        no_scan++;
        if (no_scan >= 8) {
          if (wheel_count > 0) wheel_count--;
          no_scan = 0;
          if (0 == wheel_count) break;
        }
	if (wheel_data) break;
      }
      /* 如果有滚轮数据，不睡返回，避免发送延迟。*/
      if (wheel_data) xtime_duration = -1;
    }
    return xtime_duration;
  }
#else
  return xtime_duration;
#endif
}

/* 用于轮询方式判断是否 收到包头hal_rf_is_rx_hdr()，或发包完成hal_rf_is_tx_done()。*/
bool hal_rf_irq_is_asserted(void)
{
  //return EXTI->PR & RF_PIN_IRQ ? true : false;
  /* IRQ is asserted when low. */
  //return GPIOA->IDR & RF_PIN_IRQ ? false : true;
  //gpio_set_output_toggle(PA, pin_4);
  return gpio_get_input_level_single(CI231X_RF_IRQ_PORT, CI231X_RF_IRQ_PIN) ? false : true;
}


/**************************  调试diagnostic相关函数  **************************/
void hal_gpio_init(void)
{
	scu_set_device_gate((uint32_t)PA, ENABLE);
	dpmu_set_io_reuse(PA2, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	//dpmu_set_io_reuse(PA3, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	dpmu_set_io_reuse(PA4, FIRST_FUNCTION); // 设置引脚功能复用为GPIO
  dpmu_set_io_reuse(PC3, SECOND_FUNCTION); // 设置引脚功能复用为GPIO

	dpmu_set_io_direction(PA2, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	//dpmu_set_io_direction(PA3, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	dpmu_set_io_direction(PA4, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式
    dpmu_set_io_direction(PC3, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式

	dpmu_set_io_pull(PA2, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	//dpmu_set_io_pull(PA3, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	dpmu_set_io_pull(PA4, DPMU_IO_PULL_DISABLE); // 设置关闭上下拉
  dpmu_set_io_pull(PC3, DPMU_IO_PULL_DISABLE); // 设置关闭上下拉

	gpio_set_output_mode(PA, pin_2); // GPIO的pin脚配置成输出模式
	//gpio_set_output_mode(PA, pin_3); // GPIO的pin脚配置成输出模式
	gpio_set_output_mode(PA, pin_4);
  gpio_set_output_mode(PC, pin_3);

  gpio_set_output_level_single(PA, pin_2, 0);
  gpio_set_output_level_single(PC, pin_3, 0);
  gpio_set_output_level_single(PA, pin_4, 0);

  for(int i=0;i<0;i++)
  {
     bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_HIGH_1US);
     bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_HIGH_1US);
     bsp_led_set(EXE_LED_ID_2, EXE_LED_PULSE_HIGH_1US);
  }
}

/* debug routins for adv. */
void hal_diag_adv_tx_scan_rsp(void)
{
  bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_HIGH_1US);
}
void hal_diag_adv_tx_timeout(void)
{
  bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_HIGH_1US);
}

/* debug routins for adv & connect. */
void hal_diag_event_rx_timeout(void)
{
  bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_HIGH_1US);
}
void hal_diag_event_rx_crcerr(void)
{
  bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_HIGH_1US);
}
void hal_diag_conn_rx_nonempty(void)
{
  bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_HIGH_1US);
}
void hal_diag_conn_tx_nonempty(void)
{
  bsp_led_set(EXE_LED_ID_2, EXE_LED_PULSE_HIGH_1US);
}

/* debug routins for schedule. */
void hal_diag_sch_skip_rx(void)
{
  bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_HIGH_1US);
}
void hal_diag_sch_skip_sleep(void)
{
  bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_HIGH_1US);
}
void hal_diag_sch_wait_wakeup(void)
{
  bsp_led_set(EXE_LED_ID_2, EXE_LED_PULSE_HIGH_1US);
}
void hal_diag_sch_abort_latency(void)
{
  bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_HIGH_1US);
}

/* assertion. */
void hal_diag_assert(bool cond)
{
#if 0
  if (!cond) {
    bsp_led_set(EXE_LED_ID_1, EXE_LED_PULSE_TOGGLE_1US);
  }
  __disable_irq();
  while(1);
#endif
}


void bsp_led_set(exe_led_id_t id, exe_led_status_t st)
{
//    mprintf("id=%d", id);
  return;
    switch (id)
    {
    case EXE_LED_ID_0:
      gpio_set_output_level_single(PA, pin_2, 1);
      Delay1us(1);
      gpio_set_output_level_single(PA, pin_2, 0);
      break;
    case EXE_LED_ID_1:
      gpio_set_output_level_single(PC, pin_3, 1);
      Delay1us(1);
      gpio_set_output_level_single(PC, pin_3, 0);
      break;
    case EXE_LED_ID_2:
      gpio_set_output_level_single(PA, pin_4, 1);
      Delay1us(1);
      gpio_set_output_level_single(PA, pin_4, 0);
      break;
    default:
      break;
    }
}
/************************  随机数/加密crypto相关函数  ************************/

uint32_t hal_rng_get_word(void)
{
  return 0x00000000;//rand();
}

/* There is no hardware AES in PY32F0xx. */
extern void aes128_setkey_rij(uint8_t *p_key);
extern void aes128_encrypt_rij(uint32_t *p_wdata);
void aes128_setkey(uint8_t *p_key)
{
  aes128_setkey_rij(p_key);
}
void aes128_encrypt(uint32_t *p_wdata)
{
  //bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_TOGGLE_1US);
  aes128_encrypt_rij(p_wdata);
  //bsp_led_set(EXE_LED_ID_0, EXE_LED_PULSE_TOGGLE_1US);
}

/***********************************************************************************
3.3	HAL PM
   MCU休眠和低功耗相关管理
***********************************************************************************/

SemaphoreHandle_t pmSemaphore = NULL;
bool pm_sem_wait = false;
/**
 * @brief 初始化蓝牙协议栈运行任务信号量，控制蓝牙任务的切换
 */
void hal_pm_init(void)
{
    if (pmSemaphore == NULL)
    {
        pmSemaphore = xSemaphoreCreateBinary();
    }
}

void hal_pm_reset(void)
{
    /*MCU软复位， 用于OTA */
}

/**
 * @brief 蓝牙协议栈获取到信号量，进入运行态
 */
void hal_pm_wakeup()
{
    if (pm_sem_wait)
    {
        xSemaphoreGiveFromISR(pmSemaphore, pdFALSE);
        //mprintf("wakeup hal_pm_sleep\r\n"); 
    }   
}

extern bool status_connected;
/**
 * @brief 蓝牙协议释放信号量，进入休眠，系统开始运行其他的语音，IOT等任务
 */
void hal_pm_sleep(uint8_t sleep_mode)
{
    uint8_t ret = 0;
    bool recv_irq = false;
    exe_wakeup_src = 0;
    if ((exe_stk_state == EXE_LINK_STATE_ADV)  || status_connected )
    {
  #if CIAS_BLE_SCAN_ENABLE
      change_to_rx_work_around();
      change_to_scan_adv();
      pm_sem_wait = true;
      ret = xSemaphoreTake(pmSemaphore, portMAX_DELAY);
      pm_sem_wait = false;
      restor_from_scan_adv();
  #else 
      pm_sem_wait = true;
      ret = xSemaphoreTake(pmSemaphore, portMAX_DELAY);
      pm_sem_wait = false;
  #endif
    } 
    
    /*中断里面唤醒,从唤醒到可以操作SPI需要延时400us */
    hal_rf_clear_rtc_timer_interrupt();
}

void hal_pm_save_context(uint8_t sleep_mode)
{
  /*有些MCU休眠前保护上下文现场 */ 
}
void hal_pm_restore_context(void) 
{
  /* 唤醒后恢复上下文现场 */
}

void hal_pm_clear_wakeup_status(void)
{
  /* 唤醒后清除唤醒源标志 */
}
void hal_pm_set_wakeup_source(uint8_t wakeup_src)
{
  /* 睡眠前设置允许的唤醒源 */
  if (wakeup_src & PM_WAKEUP_TIMER) /*TBD*/;
  if (wakeup_src & PM_WAKEUP_PAD)   /*TBD*/;
}

uint8_t hal_pm_get_wakeup_status(void)
{
/*   if (0 == (GPIOA->IDR & RF_PIN_IRQ)) {
    hal_rf_clear_rtc_timer_interrupt();
    return PM_WAKEUP_TIMER;
  } */
  return PM_WAKEUP_PAD;
}


/******************************  时钟管理相关函数  ****************************/

uint32_t hal_clk_get_tick_32k(void)
{
#if HAL_BLE_HW_TIMER
  /* 如果HOST MCU的32k时钟很准，则用MCU的32k计时；否则
     使用32k计时。*/
  return hal_xtime_get_current();
#else
  return rtc_read_time();
#endif
}

uint32_t hal_clk_set_alarm_32k(uint32_t tick, uint32_t cal_cnt)
{
#if HAL_BLE_HW_TIMER
  if(hal_rf_irq_is_asserted())
      rf_clear_all_irq();
  if(EXE_TIME_OLDER_THAN(exe_bletick_wakeup_point+EXE_US_TO_XTIME(1000), tick-HAL_XCVR_XTIME_ADVANCE_WAKEUP)) {
     hal_xtime_set_alarm(tick -HAL_XCVR_XTIME_ADVANCE_WAKEUP);
  }
  else {
    hal_xtime_set_alarm(exe_bletick_wakeup_point);
  }

#else
  rtc_write_time(tick);
#endif

  /* 同时返回复位时刻的高频计时，以作为恢复高频计时的基数。*/
  return hal_htime_get_current();
}

void hal_sys_tick()
{
    hal_htime_get_current();
}
/** @} */
