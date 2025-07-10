
/**
 * @file es7243e.c
 * @brief 为ES7243E封装的驱动程序
 * @version 0.1
 * @date 2019-03-28
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */

#include "sdk_default_config.h"
#include "ci130x_iic.h"
#include "es7243e.h"
#include <stdint.h>
#include <string.h>
#include "ci_log.h"
#include "ci130x_system.h"
#include "board.h"
#include "stdbool.h"

#define LOG_7243E_DEBUG LOG_USER


static void __delay(unsigned int cnt)
{
    unsigned int j=0;
    unsigned int i,sum = 0;
    for(i = 0; i < cnt; i++)
    {
        for(j = 0; j < 0x3f; j++)
        {
            sum += j % 10;
        }
    }
}



/**
 * @brief 使用的I2C初始化
 * 
 * @param 
 */
void es7243e_i2c_init(void)
{
    pad_config_for_i2c();
    iic_polling_init(IIC0,100,0,LONG_TIME_OUT);
}



/**
 * @brief 写入一个字节数据
 * 
 * @param regaddr 寄存器地址
 * @param val 需要写入的值
 */
static void I2C_WriteEs7243eData(unsigned int regaddr,char val)
{
    uint8_t last_ack_flag = 0;
    char data[8];
    data[0] = regaddr;
    data[1] = val;
    int test1_send_count = iic_master_polling_send(IIC0,0x10,data,2,&last_ack_flag);
    // mprintf("send_count = %d last_ack_flag = %d\n",test1_send_count,last_ack_flag);
    __delay(500);
}    


/**
 * @brief 读取某个寄存器的值
 * 
 * @param regaddr 寄存器地址
 * @return int 读出寄存器的值
 */
static int I2C_ReadEs7243eData(unsigned int regaddr)
{
    char data[8];
    data[0] = regaddr;
    int num = i2c_master_send_recv(0x10,data,1,1);
    // mprintf("test_out = %x\n",data[0]);
    // mprintf("num = %d\n",num);
    __delay(500);
    return (unsigned char)data[0];   
}


void es7243e_pga_gain_l(es7243e_pga_gain_t gain)
{
    //同时使能了PGA的input从MIC输入
    uint8_t reg_data = gain | (1<<4);
    I2C_WriteEs7243eData(0x20,reg_data);
}


void es7243e_pga_gain_r(es7243e_pga_gain_t gain)
{
    //同时使能了PGA的input从MIC输入
    uint8_t reg_data = gain | (1<<4);
    I2C_WriteEs7243eData(0x21,reg_data);
}


//0.25dB的变化花费的时间
typedef enum
{
    ES7243E_ALC_WINSIZE_40US = 0,
    ES7243E_ALC_WINSIZE_80US,
    ES7243E_ALC_WINSIZE_180US,
    ES7243E_ALC_WINSIZE_320US,
    ES7243E_ALC_WINSIZE_640US,
    ES7243E_ALC_WINSIZE_1_28MS,
    ES7243E_ALC_WINSIZE_2_56MS,
    ES7243E_ALC_WINSIZE_5_12MS,
    ES7243E_ALC_WINSIZE_10_24MS,
    ES7243E_ALC_WINSIZE_20_48MS,
    ES7243E_ALC_WINSIZE_40_96MS,
    ES7243E_ALC_WINSIZE_81_92MS,
    ES7243E_ALC_WINSIZE_162_84MS,
    ES7243E_ALC_WINSIZE_327_68MS,
    ES7243E_ALC_WINSIZE_655_36MS,
}es7243e_alc_winsize_t;



/**
 * @brief 配置7243的ALC
 * 
 * @param ALC_str ES7243_ALC_Config_TypeDef类型结构体指针
 * @param ALC_gate 左声道、右声道或者双声道的ALC开关
 * @return int32_t 正确
 */
int32_t ES7243E_ALC_config(es7243e_alc_config_t* ALC_str)
{
    volatile uint32_t reg_data = 0;
    uint32_t samples = 0;
    uint32_t ALC_peak_detector_windows_size = ES7243E_ALC_WINSIZE_5_12MS; 

    float max_gain_f = ALC_str->max_gain;
    if(max_gain_f > 32.0f)
    {
        max_gain_f = 32.0f;
    }
    else if(max_gain_f < -95.5f)
    {
        max_gain_f = -95.5f;
    }
    uint32_t reg_max_gain = (int)((max_gain_f + 95.5f) * 2.0f);
    I2C_WriteEs7243eData(0xE,reg_max_gain);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",0xE,I2C_ReadEs7243eData(0xE));

    reg_data = (uint32_t)(ALC_peak_detector_windows_size<<4) | (ALC_str->alc_level);
    I2C_WriteEs7243eData(0x13,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",0x13,I2C_ReadEs7243eData(0x13));
    
    // uint8_t reg_addr = 0x14;
    // I2C_WriteEs7243eData(reg_addr,0x07);//20Hz的高通滤波器
    // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    // reg_addr = 0x15;
    // I2C_WriteEs7243eData(reg_addr,0x0c);//和0x14一起形成20Hz高通滤波器
    // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    uint8_t reg_addr = 0x14;
    I2C_WriteEs7243eData(reg_addr,0x06);//60Hz的高通滤波器
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x15;
    I2C_WriteEs7243eData(reg_addr,0x07);//和0x14一起形成60Hz高通滤波器
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    return RETURN_OK;
}


void es7243e_alc_maxgain_set(float gain)
{
    float max_gain_f = gain;
    if(max_gain_f > 32.0f)
    {
        max_gain_f = 32.0f;
    }
    else if(max_gain_f < -95.5f)
    {
        max_gain_f = -95.5f;
    }
    uint32_t reg_max_gain = (int)((max_gain_f + 95.5f) * 2.0f + 0.5f);
    I2C_WriteEs7243eData(0xE,reg_max_gain);
}


int8_t es7243e_ramprate_cfg(es7243e_vc_ramprate_t ramp)
{
    volatile uint32_t reg_data = I2C_ReadEs7243eData(0xF);
    reg_data &= ~(0xf<<0);
    reg_data |= ramp;
    I2C_WriteEs7243eData(0xF,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",0xF,I2C_ReadEs7243eData(0xF));
    
    return RETURN_OK;
}


int8_t es7243e_alc_gate_en(es7243e_alc_gate_t ALC_gate)
{
    volatile uint32_t reg_data = I2C_ReadEs7243eData(0xF);
    reg_data &= ~(1<<6);
    reg_data |= ALC_gate;
    I2C_WriteEs7243eData(0xF,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",0xF,I2C_ReadEs7243eData(0xF));
    
    return RETURN_OK;
}


void es7243e_bias_low_mode(void)
{
    uint8_t reg_addr = 0x18;
    uint8_t reg_data = 1;
    I2C_WriteEs7243eData(reg_addr,reg_data);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x19;
    reg_data = 0x22;
    I2C_WriteEs7243eData(reg_addr,reg_data);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1a;
    reg_data = 0x22;
    I2C_WriteEs7243eData(reg_addr,reg_data);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1b;
    reg_data = 0x22;
    I2C_WriteEs7243eData(reg_addr,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1c;
    reg_data = 0x22;//这个值不能跳到11,
    I2C_WriteEs7243eData(reg_addr,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
}


void es7243e_bias_normal_mode(void)
{
    uint8_t reg_addr = 0x18;
    uint8_t reg_data = 6;
    I2C_WriteEs7243eData(reg_addr,reg_data);
 //   ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x19;
    reg_data = 0x77;
    I2C_WriteEs7243eData(reg_addr,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1a;
    reg_data = 0xf4;
    I2C_WriteEs7243eData(reg_addr,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1b;
    reg_data = 0x66;
    I2C_WriteEs7243eData(reg_addr,reg_data);
 //   ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1c;
    reg_data = 0x44;
    I2C_WriteEs7243eData(reg_addr,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
}


void es7243e_close_right_cha(void)
{
    volatile uint32_t reg_data = I2C_ReadEs7243eData(0x16);
    reg_data |= ( (1<<4) | (1<<2) | (1<<0) );
    I2C_WriteEs7243eData(0x16,reg_data);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",0x16,I2C_ReadEs7243eData(0x16));
}


void es7243e_open_right_cha(void)
{
    volatile uint32_t reg_data = I2C_ReadEs7243eData(0x16);
    reg_data &= ~( (1<<4) | (1<<2) | (1<<0) );
    I2C_WriteEs7243eData(0x16,reg_data);
 //   ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",0x16,I2C_ReadEs7243eData(0x16));
}


void es7243e_enter_lowpower_mode(void)
{
    es7243e_bias_low_mode();
    uint8_t reg_addr = 0x1d;
    I2C_WriteEs7243eData(reg_addr,0xff);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1e;
    I2C_WriteEs7243eData(reg_addr,0x3f);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
}


void es7243e_exit_lowpower_mode(void)
{
    es7243e_bias_normal_mode();
    uint8_t reg_addr = 0x1d;
    I2C_WriteEs7243eData(reg_addr,0);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x1e;
    I2C_WriteEs7243eData(reg_addr,0);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
}


void es7243e_init(bool is_linein)
{
    es7243e_i2c_init();
    uint8_t reg_addr = 0;
    
    reg_addr = 0x02;
    I2C_WriteEs7243eData(reg_addr,0x00);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x3;
    I2C_WriteEs7243eData(reg_addr,0x20);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x0d;//MIC左右声道排列，MICL在左声道，MICR在右声道；；scale 0dB（是否是24dB是0dB）
    I2C_WriteEs7243eData(reg_addr,0);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x4;
    I2C_WriteEs7243eData(reg_addr,1);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x5;
    I2C_WriteEs7243eData(reg_addr,0);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x9;
    I2C_WriteEs7243eData(reg_addr,0xe0);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0xa;
    I2C_WriteEs7243eData(reg_addr,0xa0);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x0b;
    I2C_WriteEs7243eData(reg_addr,0x0c);//16 bit 标准I2S格式
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    // reg_addr = 0x0e;
    // I2C_WriteEs7243eData(reg_addr,0xbf);//0dB的maxgain
    // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x0f;
    I2C_WriteEs7243eData(reg_addr,0x80);//dynamic HPF、ALC disable、automute关
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    

    es7243e_alc_config_t alc_cfg_str;
    memset((void*)&alc_cfg_str,0,sizeof(es7243e_alc_config_t));
    if(is_linein)
    {
        alc_cfg_str.max_gain = 2.0f;
    }
    else
    {
        alc_cfg_str.max_gain = 14.0f;
    }
    alc_cfg_str.alc_level = ES7243E_ALC_LEVEL__6dB;
    ES7243E_ALC_config(&alc_cfg_str);
    es7243e_alc_gate_en(ES7243E_ALC_OFF);


    reg_addr = 0x18;
    I2C_WriteEs7243eData(reg_addr,0x06);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x19;
    I2C_WriteEs7243eData(reg_addr,0x77);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x1a;
    I2C_WriteEs7243eData(reg_addr,0xf4);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x1b;
    I2C_WriteEs7243eData(reg_addr,0x66);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x1c;
    I2C_WriteEs7243eData(reg_addr,0x44);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x1e;
    I2C_WriteEs7243eData(reg_addr,0x00);
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x1f;
    I2C_WriteEs7243eData(reg_addr,0x0c);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    if(is_linein)
    {
        reg_addr = 0x20;
        es7243e_pga_gain_l(ES7243E_PGA_GAIN_0dB);
     //   ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
        
        reg_addr = 0x21;
        es7243e_pga_gain_r(ES7243E_PGA_GAIN_0dB);
    //    ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    }
    else
    {
        reg_addr = 0x20;
        es7243e_pga_gain_l(ES7243E_PGA_GAIN_30dB);
    //    ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
        
        reg_addr = 0x21;
        es7243e_pga_gain_r(ES7243E_PGA_GAIN_30dB);
    //    ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    }
    
    reg_addr = 0x00;
    I2C_WriteEs7243eData(reg_addr,0x80);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x1;
    I2C_WriteEs7243eData(reg_addr,0x3a);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x16;
    I2C_WriteEs7243eData(reg_addr,0x3f);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
    
    reg_addr = 0x16;
    I2C_WriteEs7243eData(reg_addr,0);
   // ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));

    reg_addr = 0x0f;
    es7243e_ramprate_cfg(ES7243E_RAMPRATE_5_12MS);
    if(!is_linein)
    {
        es7243e_alc_gate_en(ES7243E_ALC_ON);
    }
  //  ci_loginfo(LOG_7243E_DEBUG,"%02x%02x\n",reg_addr,I2C_ReadEs7243eData(reg_addr));
}

