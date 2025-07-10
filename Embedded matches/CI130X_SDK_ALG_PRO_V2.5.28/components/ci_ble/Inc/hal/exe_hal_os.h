/*============================================================================*/
/* @file exe_hal_os.h
 * @brief EXE HAL OS header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_OS_H__
#define __EXE_HAL_OS_H__

/*******************************  RTOSÏà¹Øº¯Êý  *******************************/

/**
 * @brief Enter critical region in RTOS environment.
 * @note  Only implemented when exBLE is running as a RTOS task.
 */
void hal_os_cr_enter(void);

/**
 * @brief Exit critical region in RTOS environment.
 * @note  Only implemented when exBLE is running as a RTOS task.
 */
void hal_os_cr_exit(void);

#endif /* #ifndef __EXE_HAL_OS_H__ */
