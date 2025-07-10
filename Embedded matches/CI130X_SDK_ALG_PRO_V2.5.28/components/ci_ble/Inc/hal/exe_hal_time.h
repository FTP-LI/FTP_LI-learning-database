/*============================================================================*/
/* @file exe_hal_time.h
 * @brief EXE HAL time header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_TIME_H__
#define __EXE_HAL_TIME_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief xtime (transceiver rtc time) is based on CI231X's timer,
 *        while host MCU's htime is inaccurate or local.
 */
#define XCVR_XTIME_HZ                   32000
#define XCVR_XTIME_MASK                 0xFFFFFFFF
#define XCVR_XTIME_HALF                 (XCVR_XTIME_MASK >> 1)
#define EXE_TIMESTAMP_MS()              hal_xtime_get_current() / 1000 * 3125 / 100

/**
 * @brief The global variable for anchor point.
 */
#define XTIME_ON_SYNC                   *(uint32_t *)ll_gbuf_xpkt_rx
#define XTIME_END_OF_RX                 XTIME_ON_SYNC + EXE_US_TO_XTIME((1+4+2+ll_gbuf_xpkt_rx[5]+3)*8) //=13


/**
 * @brief The translation from time to RTC time at accurate frequency.
 * @param [in] us,ms,dms,sec - The time in us,ms,1.25ms,sec.
 * @return the tranceiver rtc time.
 */
#if defined(__C51__)
#define EXE_US_TO_XTIME(us)             ((XCVR_XTIME_HZ / 1000 * (us) / 1000) + 1)
#else
#define EXE_US_TO_XTIME(us)             ((((1ULL<<32) * XCVR_XTIME_HZ / 1000000 * (us)) >> 32) + 1)
#endif
#define EXE_MS_TO_XTIME(ms)             ((ms) * XCVR_XTIME_HZ/1000)
#define EXE_1D25MS_TO_XTIME(dms)        ((dms) *  XCVR_XTIME_HZ/1000 * 125 / 100)
#define EXE_SEC_TO_XTIME(sec)           ((sec) * XCVR_XTIME_HZ)
#define EXE_XTIME_TO_MS(xtime)          ((xtime) * 1000 / XCVR_XTIME_HZ)

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
#define XTIME_OLDER_THAN(time, ref) (bool)((((uint32_t) (time)) - ((uint32_t) (ref))) > XCVR_XTIME_HALF)

/**
 * @brief 检查传入的参考时间点(bletick)+时间跨度(bletick)是否已发生。
 * @note 获得BLE tick的开销比较大，超过100us，注意使用场合和频次。
 *
 * @param[in] ref_tick  传入的参考时间点(单位bletick)
 * @param[in] span_tick 上述时间点再加一个时间跨度(单位bletick)
 *
 * @returns true已发生 false未发生
 */
#define EXE_XTIME_ELAPSED(ref_tick, span_tick) \
  XTIME_OLDER_THAN((ref_tick) + (span_tick), hal_xtime_get_current())


/**************************  系统time/clock相关函数  **************************/

/**
 * @brief The translation from time to host time at high but possible corse frequency.
 * @param [in] us,ms,dms - The time in us, ms, 1.25ms
 * @return the host time.
 */
uint32_t hal_us_to_htime(uint32_t us);
uint32_t hal_ms_to_htime(uint32_t ms);
uint32_t hal_1d25ms_to_htime(uint32_t dms);

/**
 * @brief The translation from transceiver rtc time at 32KHz to host time at high frequency.
 * @param [in] xtime - The transceiver rtc time at 32KHz.
 * @return the host time.
 */
uint32_t hal_xtime_to_htime(uint32_t xtime);

/**
 * @brief Read the current MCU/host time for timeout routines.
 * @note  It is not a global system time.
 *
 * @reutrun The absolute system time at high frequency.
 */
uint32_t hal_htime_get_current(void);

/**
 * @brief Restore MCU/host time after exit low power mode.
 * @note  It will become obsoloted for CI231X version exBLE.
 *
 * @param[in] time - The absolute system time at high frequency.
 */
void hal_htime_set_current(uint32_t time);

/**
 * @brief Get the absolute difference between two timestamps, regardless of order.
 *
 * @param[in] time1 First timestamp to compare
 * @param[in] time2 Second timestamp to compare
 *
 * @returns The difference between the two time parameters.
 */
uint32_t hal_htime_diff(uint32_t time1, uint32_t time2);

/**
 * @brief 检查传入的参考时间点(systick)+时间跨度(systick)是否已发生。
 *
 * @param[in] ref_time  传入的参考时间点(单位systick)
 * @param[in] span_time 上述时间点再加一个时间跨度(单位systick)
 *
 * @returns true已发生 false未发生
 */
bool hal_htime_elapsed(uint32_t ref_time, int32_t span_time);

/**
 * @brief Time/Clock module initialization.
 */


/**
 * @brief Poll wait at milliseconds level base on host time. 毫秒级延时。
 *
 * @param [in] ms - the delay time in milliseconds.
 *                  value range in [1, implementation limited].
 * @note 精度不用太高，可以偏大，但不能偏小
 */
void hal_delay_ms(int ms);

/**
 * @brief Poll wait at microseconds level based on host time. 微秒级延时。
 *
 * @param [in] us - the delay time in microseconds.
 *                  value range in [1, implementation limited].
 * @note 精度需要尽可能精确。
 */
void hal_delay_us(int us);

#endif /* #ifndef __EXE_HAL_TIME_H__ */

