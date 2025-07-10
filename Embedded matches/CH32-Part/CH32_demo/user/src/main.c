/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library 即（CH32V307VCT6 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是CH32V307VCT6 开源库的一部分
*
* CH32V307VCT6 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MounRiver Studio V1.8.1
* 适用平台          CH32V307VCT6
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期                                      作者                             备注
* 2022-09-15        大W            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"


// *************************** 例程硬件连接说明 ***************************
// 核心板正常供电即可 无需额外连接


// *************************** 例程测试说明 ***************************
//本工程外部鼾声识别为7秒识别一次


// **************************** 代码区域 ****************************
#define LED1                    (E2 )

#define KEY1                    (C9 )                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用

#define KEY1_EXTI               (EXTI9_5_IRQn)                                  // 对应外部中断的中断编号

#define UART_INDEX              (DEBUG_UART_INDEX   )                           // 默认 UART_3
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)                           // 默认 115200
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )                           // 默认 UART3_MAP0_TX_B10
#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )                           // 默认 UART3_MAP0_RX_B11

#define UART_PRIORITY           (USART3_IRQn)                                   // 对应串口中断的中断编号 在 ch32v30x.h 头文件中查看 IRQn_Type 枚举体

uint8 exti_state[1];

uint8       uart_get_data[64];                                                  // 串口接收数据缓冲区
uint8       fifo_get_data[64];                                                  // fifo 输出读出缓冲区

uint8       get_data = 0;                                                       // 接收数据变量
uint32      fifo_data_count = 0;                                                // fifo 数据个数

fifo_struct uart_data_fifo;

uint8 Send_buf[10] = {0} ;

void SendCmd(uint8 len)
{

    uart_write_byte(UART_INDEX,0x7E); //起始

	uart_write_buffer(UART_INDEX,Send_buf,len); //控制数据

    uart_write_byte(UART_INDEX,0xEF) ;//结束
}

void DoSum( uint8 *Str, uint8 len)
{
    uint16 xorsum = 0;
    uint8 i;

    for(i=0; i<len; i++)
    {
        xorsum  = xorsum + Str[i];
    }
	xorsum     = 0 -xorsum;
	*(Str+i)   = (uint8)(xorsum >>8);
	*(Str+i+1) = (uint8)(xorsum & 0x00ff);
}

void Uart_SendCMD(uint8 CMD ,uint8 feedback , uint16 dat)
{
    Send_buf[0] = 0xff;    //保留字节 
    Send_buf[1] = 0x06;    //长度
    Send_buf[2] = CMD;     //控制指令
    Send_buf[3] = feedback;//是否需要反馈
    Send_buf[4] = (uint8)(dat >> 8);//datah
    Send_buf[5] = (uint8)(dat);     //datal
    DoSum(&Send_buf[0],6);        //校验
    SendCmd(8);       //发送此帧数据
}


int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                       // 务必保留，本函数用于初始化MPU 时钟 调试串口

    // 此处编写用户代码 例如外设初始化代码等

    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // 初始化 LED1 输出 默认高电平 推挽输出模式

    exti_init(KEY1, EXTI_TRIGGER_RISING);                                       // 初始化 KEY1 为外部中断输入 上升沿触发

    interrupt_set_priority(KEY1_EXTI, (0<<5) || 1);                             // 设置 KEY1 对应外部中断的中断抢占优先级0，子优先级1

    //------------------------------------------------------------------------------------------------------------//
    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // 初始化 fifo 挂载缓冲区

    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // 初始化编码器模块与引脚 正交解码编码器模式
    uart_rx_interrupt(UART_INDEX, ZF_ENABLE);                                   // 开启 UART_INDEX 的接收中断
    interrupt_set_priority(UART_PRIORITY, (0<<5) || 1);                         // 设置对应 UART_INDEX 的中断抢占优先级0，子优先级1
    system_delay_ms(2000);
    exti_state[0] = 0; 
    //------------------------------------------------------------------------------------------------------------//
    // 此处编写用户代码 例如外设初始化代码等
    while(1)//三段状态机
    {
        // 此处编写需要循环执行的代码
        if(gpio_get_level(KEY1) == 1)
        {
            exti_state[0] = 1;                                                      // 外部中断触发 标志位置位
        }

        if(exti_state[0] == 1)//第一音频阶段
        {
            Uart_SendCMD(0x03,0,0x02);//播放第一音频
            exti_state[0] = 2;
            system_delay_ms(10000);
            if(gpio_get_level(KEY1) == 1)
            {
                exti_state[0] = 1;                                                      // 外部中断触发 标志位置位
            } 
        }
        else if(exti_state[0] == 2)//第二音频阶段
        {
            Uart_SendCMD(0x03,0,0x01);
            system_delay_ms(1000);                                           // 延时
            exti_state[0] = 0;                                                  // 清空外部中断触发标志位
        }
        // 此处编写需要循环执行的代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     KEY1 的按键外部中断处理函数 这个函数将在 KEY1 引脚对应的外部中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     key1_exti_handler();
//-------------------------------------------------------------------------------------------------------------------
void key1_exti_handler (void)
{
    // 上升沿触发，进入中断的时候GPIO状态是高电平。
    if(gpio_get_level(KEY1) == 1)
    {
        exti_state[0] = 1;                                                      // 外部中断触发 标志位置位
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     UART_INDEX 的接收中断处理函数 这个函数将在 UART_INDEX 对应的中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler (void)
{
//    get_data = uart_read_byte(UART_INDEX);                                      // 接收数据 while 等待式 不建议在中断使用
    uart_query_byte(UART_INDEX, &get_data);                                     // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
    fifo_write_buffer(&uart_data_fifo, &get_data, 1);                           // 将数据写入 fifo 中
}

// **************************** 代码区域 ****************************
