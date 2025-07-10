
#include "cias_rgb_driver.h"
#include "FreeRTOS.h" 
#include "task.h"
#include "ci130x_dpmu.h"
#include "rf_msg_deal.h"
#include "sdk_default_config.h"
#include "exe_api.h"
#if RGB_DRIVER_EN
static int rgb_first_use = 0;
static int rgb_mode_flag = 0;
volatile uint8_t music_mode_flag = 0;
static int rgb_music_status = 0;
volatile int rgb_switch_status = 0;        //rgb灯开关状态判断  0：关  1：开

int r_flag = 0;
int g_flag = 0;
int b_flag = 0;
int orange_color_flag = 0;
int yellow_color_flag = 0;
int purple_color_flag = 0;
// unsigned int duty_lasted = 0;
int duty_lasted = 0;
extern rf_cb_funcs_t rf_cb_funcs;
void rgb_music_mode(void)
{
    int duty = 0;
    int duty2 = 0;

    rgb_music_status = 1;
    b_flag = r_flag = g_flag = 1;
    purple_color_flag = yellow_color_flag = orange_color_flag = 0;
    while(music_mode_flag)
    {
        pwm_set_duty(G_PWM_BASE,0,1000);
        pwm_set_duty(R_PWM_BASE,0,1000);
        for(duty = 0;duty < 900;duty += 10)
        {
            if(!music_mode_flag)
                break;
            pwm_set_duty(B_PWM_BASE,duty,1000);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
        for(duty2 = 900;duty2 > 0;duty2 -= 10)
        {
            if(!music_mode_flag)
                break;
            pwm_set_duty(B_PWM_BASE,duty2,1000);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
        pwm_set_duty(G_PWM_BASE,0,1000);
        pwm_set_duty(B_PWM_BASE,0,1000);
        for(duty = 0;duty < 900;duty += 10)
        {
            if(!music_mode_flag)
                break;
            pwm_set_duty(R_PWM_BASE,duty,1000);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
        for(duty2 = 900;duty2 > 0;duty2 -= 10)
        {
            if(!music_mode_flag)
                break;
            pwm_set_duty(R_PWM_BASE,duty2,1000);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
        pwm_set_duty(R_PWM_BASE,0,1000);
        pwm_set_duty(B_PWM_BASE,0,1000);
        for(duty = 0;duty < 900;duty += 10)
        {
            if(!music_mode_flag)
                break;
            pwm_set_duty(G_PWM_BASE,duty,1000);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
        for(duty2 = 900;duty2 > 0;duty2 -= 10)
        {
            if(!music_mode_flag)
                break;
            pwm_set_duty(G_PWM_BASE,duty2,1000);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}

void rgb_music_mode_task(void *p_arg)
{
    rgb_init();
    mprintf("\n=====rgb_init finished \n");
    while(1)
    {
        if(music_mode_flag)
            rgb_music_mode();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void rgb_turn_off(void)
{
    rgb_switch_status = 0;
    music_mode_flag = 0;
    for(uint32_t i = 0;i < 1000;i++)
    {
        for(uint32_t j = 0;j < 185;j++)
        {}
    }
    pwm_set_duty(RED,0,1000);
    pwm_set_duty(GREEN,0,1000);
    pwm_set_duty(BLUE,0,1000);
    for(uint32_t i = 0;i < 1000;i++)
    {
        for(uint32_t j = 0;j < 200;j++)
        {}
    }
}
void rgb_control_light(uint8_t light)
{
    rgb_switch_status = 1;
    if(rgb_first_use)
    {
        pwm_set_duty(RED,500,1000);
        pwm_set_duty(GREEN,500,1000);
        pwm_set_duty(BLUE,500,1000);
        r_flag = g_flag = b_flag = 1;
        duty_lasted = 500;
        rgb_first_use = 0;
    }
    duty_lasted = light*10;
    if(duty_lasted >= 999)
    {
        duty_lasted = 999;
    }
    else if (duty_lasted <= 100)
    {
        duty_lasted = 100;
    }

    if(r_flag)
        pwm_set_duty(RED,duty_lasted,1000);
    if(g_flag)
        pwm_set_duty(GREEN,duty_lasted,1000);
    if(b_flag)
        pwm_set_duty(BLUE,duty_lasted,1000);
}

void rgb_control_color(uint8_t *color_data)
{
    pwm_set_duty(RED, color_data[0]*2,1000);
    pwm_set_duty(GREEN, color_data[1]*2,1000);
    pwm_set_duty(BLUE, color_data[2]*2,1000);
    music_mode_flag = 0;
}

int rgb_control(unsigned short cmd_id)
{   
    int ret = 1;
    if ((cmd_id >= 39) && (cmd_id <= 42))
        rgb_mode_flag = !rgb_mode_flag;
    mprintf("====cmd_id:%d\n",cmd_id);
    mprintf("====cmd_id:%d\n",cmd_id);
    switch (cmd_id)
    {
        case 3:/*打开灯光*/
        case 4://我想打开灯光
        case 5://帮我开灯
        case 6://替我开灯
        case 7://开灯
        {
            mprintf("开灯！\n");
            rgb_switch_status = 1;
            if(rgb_first_use)
            {
                pwm_set_duty(RED,500,1000);
                pwm_set_duty(GREEN,500,1000);
                pwm_set_duty(BLUE,500,1000);
                r_flag = g_flag = b_flag = 1;
                duty_lasted = 500;
                rgb_first_use = 0;
            }
            else
            {
                if(purple_color_flag)
                {
                    pwm_set_duty(RED,((duty_lasted > 895)?895:duty_lasted) + 128,1000);
                    pwm_set_duty(GREEN,0,1000);
                    pwm_set_duty(BLUE,((duty_lasted > 895)?895:duty_lasted) + 128,1000);
                }
                else if(yellow_color_flag)
                {
                    pwm_set_duty(RED,((duty_lasted > 509)?510:duty_lasted) + 255,1000);
                    pwm_set_duty(GREEN,((duty_lasted > 509)?510:duty_lasted) + 255,1000);
                    pwm_set_duty(BLUE,0,1000);
                }
                else if(orange_color_flag)
                {
                    pwm_set_duty(RED,((duty_lasted > 509)?510:duty_lasted) + 255,1000);
                    pwm_set_duty(GREEN,(((duty_lasted > 509)?510:duty_lasted) + 255)/5,1000);
                    pwm_set_duty(BLUE,0,1000);
                }
                else
                {
                    if(r_flag)
                        pwm_set_duty(RED,duty_lasted,1000);
                    if(g_flag)
                        pwm_set_duty(GREEN,duty_lasted,1000);
                    if(b_flag)
                        pwm_set_duty(BLUE,duty_lasted,1000);
                }
            }
            break;
        }
        case 8:/*关闭灯光*/
        case 9:/*我想关闭灯光*/
        case 10:/*我要关灯*/
        case 11:/*替我关灯*/
        case 12:/*关灯*/
        {
            rgb_turn_off();
            break;
        }
        case 13://调亮一点
        case 14://亮一点
        case 15://再亮一点
        {
            rgb_switch_status = 1;
            if(rgb_first_use)
            {
                pwm_set_duty(RED,500,1000);
                pwm_set_duty(GREEN,500,1000);
                pwm_set_duty(BLUE,500,1000);
                r_flag = g_flag = b_flag = 1;
                duty_lasted = 500;
                rgb_first_use = 0;
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,((duty_lasted >= 764)?764:(((int)(duty_lasted/255))*255)),1000);
                pwm_set_duty(GREEN,((duty_lasted >= 764)?764:(((int)(duty_lasted/255))*255)),1000);
            }
            else if(orange_color_flag)
            {
                mprintf("duty_lasted:%d,,%d\n",duty_lasted,((int)(duty_lasted/255)));
                pwm_set_duty(RED,((duty_lasted >= 764)?764:(((int)(duty_lasted/255))*255)),1000);
                pwm_set_duty(GREEN,((duty_lasted >= 764)?764:(((int)(duty_lasted/255))*255))/5,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,duty_lasted,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,duty_lasted,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,duty_lasted,1000);
            }
            duty_lasted += 200;
            if(duty_lasted >= 999)
            {
                duty_lasted = 999;
                iot_light_brightness_broadcast(10);
                ret = -1;
            }
            break;
        }
        case 16://调暗一点
        case 17://暗一点
        case 18://再暗一点
        {
            rgb_switch_status = 1;
            if(rgb_first_use)
            {
                pwm_set_duty(RED,500,1000);
                pwm_set_duty(GREEN,500,1000);
                pwm_set_duty(BLUE,500,1000);
                r_flag = g_flag = b_flag = 1;
                duty_lasted = 500;
                rgb_first_use = 0;
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,((duty_lasted <= 255)?255:(((int)(duty_lasted/255))*255)),1000);
                pwm_set_duty(GREEN,((duty_lasted <= 255)?255:(((int)(duty_lasted/255))*255)),1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,((duty_lasted <= 255)?255:(((int)(duty_lasted/255))*255)),1000);
                pwm_set_duty(GREEN,((duty_lasted <= 255)?255:(((int)(duty_lasted/255))*255))/5,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,duty_lasted,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,duty_lasted,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,duty_lasted,1000);
            }
            duty_lasted -= 200;
            // mprintf("调暗duty_lasted:%d\n",duty_lasted);
            if(duty_lasted <= 100)
            {
                duty_lasted = 100;
                iot_light_brightness_broadcast(0);
                ret = -1;
            }
            break;
        }
        case 19:/*最高亮度*/
        case 20:/*灯光最亮*/
        {
            rgb_switch_status = 1;
            if(rgb_first_use)
            {
                pwm_set_duty(RED,999,1000);
                pwm_set_duty(GREEN,999,1000);
                pwm_set_duty(BLUE,999,1000);
                r_flag = g_flag = b_flag = 1;
                duty_lasted = 999;
                rgb_first_use = 0;
            }
            if(yellow_color_flag)
            {
                pwm_set_duty(RED,765,1000);
                pwm_set_duty(GREEN,765,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,765,1000);
                pwm_set_duty(GREEN,150,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,999,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,999,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,999,1000);
            }
            duty_lasted = 999;
            break;
        }
        case 21:/*最低亮度*/
        case 22:/*灯光最暗*/
        {
            rgb_switch_status = 1;
            if(rgb_first_use)
            {
                pwm_set_duty(RED,100,1000);
                pwm_set_duty(GREEN,100,1000);
                pwm_set_duty(BLUE,100,1000);
                r_flag = g_flag = b_flag = 1;
                duty_lasted = 100;
                rgb_first_use = 0;
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,255,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,50,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,100,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,100,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,100,1000);
            }
            duty_lasted = 100;
            break;
        }
        case 23:/*中等亮度*/
        case 24:/*灯光中等*/
        {
            rgb_switch_status = 1;
            if(rgb_first_use)
            {
                pwm_set_duty(RED,500,1000);
                pwm_set_duty(GREEN,500,1000);
                pwm_set_duty(BLUE,500,1000);
                r_flag = g_flag = b_flag = 1;
                duty_lasted = 500;
                rgb_first_use = 0;
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,510,1000);
                pwm_set_duty(GREEN,510,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,510,1000);
                pwm_set_duty(GREEN,100,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,500,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,500,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,500,1000);
            }
            duty_lasted = 500;
            break;
        }
        case 25://红色灯光
        case 26://打开红色灯光
        {
            if(rgb_music_status)
            {
                
                rgb_music_status = 0;
                rgb_turn_off();
                vTaskDelay(5);
            }
            if(rgb_first_use)
            {
                rgb_first_use = 0;
                duty_lasted = 500;
            }
            rgb_switch_status = 1;
            pwm_set_duty(RED,duty_lasted,1000);
            pwm_set_duty(GREEN,0,1000);
            pwm_set_duty(BLUE,0,1000);
            r_flag = 1;
            purple_color_flag = yellow_color_flag = orange_color_flag = g_flag = b_flag = 0;
            music_mode_flag = 0;

            break;
        }
        case 27://橙色灯光
        case 28://打开橙色灯光
        {
            if(rgb_music_status)
            {
                rgb_music_status = 0;
                rgb_turn_off();
                vTaskDelay(5);
            }
            if(rgb_first_use)
            {
                rgb_first_use = 0;
                duty_lasted = 500;
            }
            rgb_switch_status = 1;
            pwm_set_duty(RED,((duty_lasted > 509)?509:duty_lasted) + 255,1000);
            pwm_set_duty(GREEN,((duty_lasted > 150)?100:duty_lasted) + 50,1000);
            pwm_set_duty(BLUE,0,1000);
            purple_color_flag = yellow_color_flag = b_flag = 0;
            orange_color_flag = r_flag = g_flag = 1;
            music_mode_flag = 0;

            break;
        }
        case 29://黄色灯光
        case 30://打开黄色灯光
        {
            if(rgb_music_status)
            {
                rgb_music_status = 0;
                rgb_turn_off();
                vTaskDelay(5);
            }
            if(rgb_first_use)
            {
                rgb_first_use = 0;
                duty_lasted = 500;
            }
            rgb_switch_status = 1;
            pwm_set_duty(RED,((duty_lasted > 743)?744:duty_lasted) + 255,1000);
            pwm_set_duty(GREEN,((duty_lasted > 743)?744:duty_lasted) + 255,1000);
            pwm_set_duty(BLUE,0,1000);
            purple_color_flag = orange_color_flag = b_flag = 0;
            yellow_color_flag = r_flag = g_flag = 1;
            music_mode_flag = 0;

            break;
        }
        case 31://绿色灯光
        case 32://打开绿色灯光
        {
            if(rgb_music_status)
            {
                rgb_music_status = 0;
                rgb_turn_off();
                vTaskDelay(5);
            }
            if(rgb_first_use)
            {
                rgb_first_use = 0;
                duty_lasted = 500;
            }
            rgb_switch_status = 1;
            pwm_set_duty(GREEN,duty_lasted,1000);
            pwm_set_duty(RED,0,1000);
            pwm_set_duty(BLUE,0,1000);
            g_flag = 1;
            purple_color_flag = yellow_color_flag = orange_color_flag = r_flag = b_flag = 0;
            music_mode_flag = 0;

            break;
        }
        case 33://蓝色灯光
        case 34://打开蓝色灯光
        {
            if(rgb_music_status)
            {
                rgb_music_status = 0;
                rgb_turn_off();
                vTaskDelay(5);
            }
            if(rgb_first_use)
            {
                rgb_first_use = 0;
                duty_lasted = 500;
            }
            rgb_switch_status = 1;
            pwm_set_duty(BLUE,duty_lasted,1000);
            pwm_set_duty(RED,0,1000);
            pwm_set_duty(GREEN,0,1000);
            b_flag = 1;
            purple_color_flag = yellow_color_flag = orange_color_flag = r_flag = g_flag = 0;
            music_mode_flag = 0;

            break;
        }
        case 35://紫色灯光
        case 36://打开紫色灯光
        {
            if(rgb_music_status)
            {
                rgb_music_status = 0;
                rgb_turn_off();
                vTaskDelay(5);
            }
            if(rgb_first_use)
            {
                rgb_first_use = 0;
                duty_lasted = 500;
            }
            rgb_switch_status = 1;
            pwm_set_duty(RED,128,1000);
            pwm_set_duty(GREEN,0,1000);
            pwm_set_duty(BLUE,128,1000);
            purple_color_flag = b_flag = r_flag = 1;
            g_flag = yellow_color_flag = orange_color_flag = 0;
            music_mode_flag = 0;

            break;
        }
        case 37://白色灯光
        case 38://打开白色灯光
        {
            if(rgb_music_status)
            {
                rgb_music_status = 0;
                rgb_turn_off();
                vTaskDelay(5);
            }
            if(rgb_first_use)
            {
                rgb_first_use = 0;
                duty_lasted = 500;
            }
            rgb_switch_status = 1;
            pwm_set_duty(BLUE,duty_lasted,1000);
            pwm_set_duty(RED,duty_lasted,1000);
            pwm_set_duty(GREEN,duty_lasted,1000);
            b_flag = r_flag = g_flag = 1;
            purple_color_flag = yellow_color_flag = orange_color_flag = 0;
            music_mode_flag = 0;

            break;
        }
            case 39://切换灯光
            case 40://切换模式
            case 41://模式切换
            case 42://换个模式
        {
            rgb_switch_status = 1;
            if(rgb_first_use)
            {
                duty_lasted = 500;
                rgb_first_use = 0;
            }
            if(rgb_mode_flag == 0)
            {
                music_mode_flag = 0;
                vTaskDelay(pdMS_TO_TICKS(6));
                pwm_set_duty(BLUE,duty_lasted,1000);
                pwm_set_duty(RED,duty_lasted,1000);
                pwm_set_duty(GREEN,duty_lasted,1000);
                b_flag = r_flag = g_flag = 1;
            }
            else
                music_mode_flag = 1;
            break;
        }
        case 43://音乐模式
        case 44://打开音乐模式
        case 45://音乐律动
        case 46://打开音乐律动
        {
            music_mode_flag = 1;
            break;
        }
        case 49://发送数据
        {
            uint8_t send_data[RF_RX_TX_MAX_LEN];
            for (size_t i = 0; i < RF_RX_TX_MAX_LEN; i++)
            {
                send_data[i] = i;
            }   
            cias_crypto_data(send_data, RF_RX_TX_MAX_LEN);         
            rf_cb_funcs.rf_send(send_data, RF_RX_TX_MAX_LEN);
            break;
        }
        case 100:
        {
            pwm_set_duty(BLUE,0,1000);
            pwm_set_duty(RED,0,1000);
            pwm_set_duty(GREEN,0,1000);
            break;
        }
        case 101:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128,1000);
                pwm_set_duty(BLUE,128,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,255,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,50,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,100,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,100,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,100,1000);
            }
            duty_lasted = 100;
            break;
        }
        case 102:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*2,1000);
                pwm_set_duty(BLUE,128*2,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,255,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,50,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,200,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,200,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,200,1000);
            }
            duty_lasted = 200;
            break;
        }
        case 103:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*3,1000);
                pwm_set_duty(BLUE,128*3,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,255,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,50,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,300,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,300,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,300,1000);
            }
            duty_lasted = 300;
            break;
        }
        case 104:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*4,1000);
                pwm_set_duty(BLUE,128*4,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,255,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255,1000);
                pwm_set_duty(GREEN,50,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,400,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,400,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,400,1000);
            }
            duty_lasted = 400;
            break;
        }
        case 105:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*5,1000);
                pwm_set_duty(BLUE,128*5,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,255*2,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,50*2,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,500,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,500,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,500,1000);
            }
            duty_lasted = 500;
            break;
        }
        case 106:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*6,1000);
                pwm_set_duty(BLUE,128*6,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,255*2,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,50*2,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,600,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,600,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,600,1000);
            }
            duty_lasted = 600;
            break;
        }
        case 107:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*7,1000);
                pwm_set_duty(BLUE,128*7,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,255*2,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,50*2,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,700,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,700,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,700,1000);
            }
            duty_lasted = 700;
            break;
        }
        case 108:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*7,1000);
                pwm_set_duty(BLUE,128*7,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,255*2,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,50*2,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,800,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,800,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,800,1000);
            }
            duty_lasted = 800;
            break;
        }
        case 109:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*7,1000);
                pwm_set_duty(BLUE,128*7,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,255*2,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255*2,1000);
                pwm_set_duty(GREEN,50*2,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,900,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,900,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,900,1000);
            }
            duty_lasted = 900;
            break;
        }
        case 110:
        {
            if(purple_color_flag)
            {
                pwm_set_duty(RED,128*7,1000);
                pwm_set_duty(BLUE,128*7,1000);
            }
            else if(yellow_color_flag)
            {
                pwm_set_duty(RED,255*3,1000);
                pwm_set_duty(GREEN,255*3,1000);
            }
            else if(orange_color_flag)
            {
                pwm_set_duty(RED,255*3,1000);
                pwm_set_duty(GREEN,50*3,1000);
            }
            else
            {
                if(r_flag)
                    pwm_set_duty(RED,999,1000);
                if(g_flag)
                    pwm_set_duty(GREEN,999,1000);
                if(b_flag)
                    pwm_set_duty(BLUE,999,1000);
            }
            duty_lasted = 999;
            
            break;
        }
        default:
            break;
    }
    return ret;
}

void rgb_init(void)
{
    pwm_init_t pwm_config;
    rgb_first_use = 1;

    // scu_set_device_gate(RGB_GPIOA_BASE,ENABLE);
    // scu_set_device_gate(RGB_GPIOB_BASE,ENABLE);

    // /*R*/
    // dpmu_set_io_reuse(R_PIN_PAD,R_PIN_FUNC);
    // scu_set_device_gate(R_PWM_BASE,ENABLE);
    // pwm_config.clk_sel = 0;
    // pwm_config.freq = 10000;
    // pwm_config.duty = 0;
    // pwm_config.duty_max = 1000;
    // pwm_init(R_PWM_BASE,pwm_config);
    // pwm_set_restart_md(R_PWM_BASE, 1); /*重新计数生效模式：1，等待正在输出的PWM波完成以后才生效*/
    // pwm_start(R_PWM_BASE);
    // dpmu_set_io_reuse(R_PIN_PAD,R_PIN_FUNC);/*pwm pad enable*/
    // // color_light_r_pwm_name = R_PWM_BASE;
    
    // /*G*/
    // dpmu_set_io_reuse(G_PIN_PAD,G_PIN_FUNC);
    // scu_set_device_gate(G_PWM_BASE,ENABLE);
    // pwm_config.clk_sel = 0;
    // pwm_config.freq = 10000;
    // pwm_config.duty = 0;
    // pwm_config.duty_max = 1000;
    // pwm_init(G_PWM_BASE,pwm_config);
    // pwm_set_restart_md(G_PWM_BASE, 1); /*重新计数生效模式：1，等待正在输出的PWM波完成以后才生效*/
    // pwm_start(G_PWM_BASE);
    // dpmu_set_io_reuse(G_PIN_PAD,G_PIN_FUNC);/*pwm pad enable*/
    // // color_light_g_pwm_name = G_PWM_BASE;

    // /*B*/
    // dpmu_set_io_reuse(B_PIN_PAD,B_PIN_FUNC);
    // scu_set_device_gate(B_PWM_BASE,ENABLE);
    // pwm_config.clk_sel = 0;
    // pwm_config.freq = 10000;
    // pwm_config.duty = 0;
    // pwm_config.duty_max = 1000;
    // pwm_init(B_PWM_BASE,pwm_config);
    // pwm_set_restart_md(B_PWM_BASE, 1); /*重新计数生效模式：1，等待正在输出的PWM波完成以后才生效*/
    // pwm_start(B_PWM_BASE);
    // dpmu_set_io_reuse(B_PIN_PAD,B_PIN_FUNC);/*pwm pad enable*/
    // // color_light_b_pwm_name = B_PWM_BASE;
}

void play_done_callback()
{

}
void iot_light_brightness_broadcast(int brightness)
{
    if(brightness == 10)
    {
        mprintf("已经是最高亮度\r\n");
        prompt_play_by_cmd_string("<already highest>", -1, play_done_callback, true);
    }
    else if(brightness == 0)
    {
        mprintf("已经是最低亮度\r\n");
        prompt_play_by_cmd_string("<already lowest>", -1, play_done_callback, true);
    }
}

#endif//RGB_DRIVER_EN