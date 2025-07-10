/*============================================================================*/
/* @file exe_hal_pm.h */

#ifndef __EXE_HAL_PM_H__
#define __EXE_HAL_PM_H__

/* power save mode: suspend or deepsleep */
typedef enum {
  ///System will enter sleep mode on ADV state.
  PM_SUSPEND_SLEEP_ADV      = (0x1 << 0),
  ///System will enter sleep mode on CONN state.
  PM_SUSPEND_SLEEP_CONN     = (0x1 << 1),
  ///System will enter deep sleep mode on ADV state.
  PM_SUSPEND_DEEPSLEEP_ADV  = (0x1 << 2),
  ///Unused.
  PM_SUSPEND_DEEPSLEEP_CONN	= (0x1 << 3),
} exe_suspend_modes_t;

/* wakeup sources. */
typedef enum {
  PM_WAKEUP_CORE    = (0x1 << 0),
  PM_WAKEUP_TIMER   = (0x1 << 1),
  PM_WAKEUP_PAD     = (0x1 << 2),
} exe_wakeup_sources_t;

/* 唤醒源bitmap. @ref exe_wakeup_sources_t. */
extern uint32_t exe_wakeup_src;

/* 低功耗模式bitmap。@ref exe_suspend_modes_t. */
extern uint8_t exe_gbitmask_sleep_mode;


/**************************  休眠和低功耗管理相关函数  ************************/

/**
 * @brief Power Management module initialization.
 *        MCU 低功耗相关初始化，包括CI231X IRQ唤醒。
 */
void hal_pm_init(void);

/**
 * @brief Reset MCU to reboot system for OTA etc.
 *        MCU软复位，用于OTA。
 */
void hal_pm_reset(void);

/**
 * @brief Enter the specific low power mode, sleep or deep sleep.
 *        进入休眠，这里需要关闭相关时钟，设置唤醒源。
 *
 * @param [in] sleep_mode - 0 for suspend/sleep;
 *                          1 for deep sleep
 */
void hal_pm_sleep(uint8_t sleep_mode);

/**
 * @brief Save some context to retention memory before enter low power mode.
 *        MCU休眠前保存上下文现场，比如RF校准信息和某些外设寄存器。
 *
 * @param [in] sleep_mode - 0 for suspend/sleep;
 *                          1 for deep sleep
 */
void     hal_pm_save_context(uint8_t sleep_mode);

/**
 * @brief Restore some context from retention memory after exit low power mode.
 *        唤醒后恢复上下文现场。
 */
void     hal_pm_restore_context(void);

/**
 * @brief Clear the wakeup status before enter low power mode.
 *        清除唤醒状态/原因。
 */
void     hal_pm_clear_wakeup_status(void);

/**
 * @brief Setup the wakeup source before enter low power mode.
 *        设置唤醒源。
 *
 * @param [in] wakeup_src - The wakeup source.
 */
void     hal_pm_set_wakeup_source(uint8_t wakeup_src);

/**
 * @brief Get the wakeup status / sources after exit low power mode.
 *        唤醒后获取唤醒状态/原因。
 * @return the wakeup status/source.
 */
uint8_t  hal_pm_get_wakeup_status(void);


/******************************  时钟管理相关函数  ****************************/

/**
 * @brief Get the ticks in low frequency timer at 32KHz before enter low power mode.
 *        返回32KHz低频时钟的计数值，用于唤醒后恢复高频时钟system tick.
 */
uint32_t hal_clk_get_tick_32k(void);

/**
 * @brief Setup alarm timer before enter low power mode.
 *        设置32KHz低频时钟定时器，到时唤醒系统。
 *
 * @param [in] tick     - HAL_BLE_HW_TIMER=0: 以高频systick计时的睡眠时间长度
 *                        HAL_BLE_HW_TIMER=1: 以精确bletick计时的绝对唤醒时间点
 *        [in] cal_cnt  - 不精确的低频32KHz时钟的校准值
 */
uint32_t hal_clk_set_alarm_32k(uint32_t tick, uint32_t cal_cnt);

/**
 * @brief start the rtc timer
 *        开启RTC定时器， 用于计算鼠标休眠时间
 */
void hal_start_rtc_time(void);
#endif /* #ifndef __EXE_HAL_PM_H__ */

