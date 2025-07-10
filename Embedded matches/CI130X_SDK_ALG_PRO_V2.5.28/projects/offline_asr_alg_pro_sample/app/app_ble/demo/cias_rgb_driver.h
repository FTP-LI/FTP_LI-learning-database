/*
 * @FileName:: 
 * @Author: 
 * @Date: 2023-03-07 14:32:23
 * @LastEditTime: 2023-03-09 11:00:25
 * @Description: 
 */
#ifndef __CIAS_RGB_DRIVER_H__
#define __CIAS_RGB_DRIVER_H__
#include "ci130x_gpio.h"
#include "ci130x_pwm.h"
#include "ci130x_scu.h"
#include "user_config.h"
#include "sdk_default_config.h"

#if RGB_DRIVER_EN
#define RGB_GPIOA_BASE   PA
#define RGB_GPIOB_BASE   PB


/*R*/
#define R_PIN_PAD       PB0
#define R_PIN_FUNC      SECOND_FUNCTION
#define R_PWM_BASE      HAL_PWM1_BASE
/*G*/
#define G_PIN_PAD       PB1
#define G_PIN_FUNC      SECOND_FUNCTION
#define G_PWM_BASE      HAL_PWM2_BASE
/*B*/
#define B_PIN_PAD       PA7
#define B_PIN_FUNC      SECOND_FUNCTION
#define B_PWM_BASE      HAL_PWM0_BASE

#define RED     PWM1
#define GREEN   PWM2
#define BLUE    PWM0

void rgb_init(void);
void rgb_control_color(uint8_t *color_data);
void rgb_control_light(uint8_t light);
int rgb_control(unsigned short cmd_id);
void rgb_music_mode_task(void *p_arg);

#endif//RGB_DRIVER_EN
#endif//__CIAS_RGB_DRIVER_H__