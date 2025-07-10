#include "ci_ble_spi.h"
#include "ci130x_gpio.h"
#include "ci130x_dpmu.h"
#include "ci130x_timer.h"
#include "ci130x_core_eclic.h"
#include "ci130x_core_misc.h"
#include "ci_log.h"
#include "FreeRTOS.h"
#include "ci_log.h"
#define SPI_OUT 0


static const unsigned char channel_index[3] = {
	2,
	26,
	80};

#define APP_GAP_APPEARANCE 961

int rf_sleep_cnt = 1;
int rf_reset_cnt = 15;

void Delay1ms(unsigned int cnt)
{
	volatile unsigned int i, j;
	for (i = 0; i < cnt; i++)
	{
		for (j = 0; j < 18500; j++)
		{
		}
	}
}

void Delay1us(unsigned int cnt)
{
	volatile unsigned int i, j;
	for (i = 0; i < cnt; i++)
	{
		for (j = 0; j < 20; j++)
		{
		}
	}
}


uint32_t timer3_systick_count = 0;
#define MAX_COUNT    0xFFFFFFFF
#define TIMER3_ONUS_COUNT (get_apb_clk()/1000000)
/**
 * @brief 系统的timer3定时器到期后，蓝牙协议栈运行的tick加1
 */
void ble_timer3_irq_deal(void)
{
	timer_clear_irq(TIMER3);
}

uint32_t timer3_get_sys_tick()
{
	uint32_t count;
	timer_get_count(TIMER3,&count);
	count = MAX_COUNT - count;
	return count;
}
uint32_t timer3_get_time_us()
{
	uint32_t count = timer3_get_sys_tick();
	count /= TIMER3_ONUS_COUNT;
	return count;
}
void timer3_set_sys_tick(uint32_t tick)
{
	uint32_t count;
	count = MAX_COUNT - tick;
	timer_set_count(TIMER3, &count);
}


void timer3_init(void)
{
	timer_init_t init_t;
	scu_set_device_gate(HAL_TIMER3_BASE, ENABLE);
	scu_set_device_reset(HAL_TIMER3_BASE);
	scu_set_device_reset_release(HAL_TIMER3_BASE);

	init_t.count = 0xffffffff; // 32768-30us
	init_t.div = timer_clk_div_0;
	init_t.width = timer_iqr_width_f;
	init_t.mode = timer_count_mode_auto;

	eclic_irq_enable(TIMER3_IRQn);
    __eclic_irq_set_vector(TIMER3_IRQn, (int32_t)ble_timer3_irq_deal);
	timer_init(TIMER3, init_t);
	timer_start(TIMER3);
}


/* CI231X 模拟SPI(3,4线)接口初始化函数
  已经没有硬件CE的控制脚了，
  IRQ脚根据需要进行配置
*/

/**
 * @brief 初始化射频芯片需要的软件SPI引脚
 */
extern void hal_gpio_init(void);
void ci231x_spi_init(void)
{
	// mprintf("ci231x_spi_init is called...\r\n");
	scu_set_device_gate((uint32_t)PB, ENABLE); // 开启PB时钟
	scu_set_device_gate((uint32_t)PC, ENABLE);
	dpmu_set_io_reuse(CI231X_RF_CSN_PIN_PAD, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	dpmu_set_io_reuse(CI231X_RF_SCK_PIN_PAD, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	dpmu_set_io_reuse(CI231X_RF_MOSI_PIN_PAD, FIRST_FUNCTION); // 设置引脚功能复用为GPIO

	dpmu_set_io_direction(CI231X_RF_CSN_PIN_PAD, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	dpmu_set_io_direction(CI231X_RF_SCK_PIN_PAD, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	dpmu_set_io_direction(CI231X_RF_MOSI_PIN_PAD, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式

	dpmu_set_io_pull(CI231X_RF_CSN_PIN_PAD, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	dpmu_set_io_pull(CI231X_RF_SCK_PIN_PAD, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	dpmu_set_io_pull(CI231X_RF_MOSI_PIN_PAD, DPMU_IO_PULL_DISABLE); // 设置关闭上下拉
	// dpmu_set_io_driver_strength(CI231X_RF_MOSI_PIN_PAD,DPMU_IO_DRIVER_STRENGTH_3);

	gpio_set_output_mode(CI231X_RF_CSN_PORT, CI231X_RF_CSN_PIN); // GPIO的pin脚配置成输出模式
	gpio_set_output_mode(CI231X_RF_SCK_PORT, CI231X_RF_SCK_PIN); // GPIO的pin脚配置成输出模式
	gpio_set_output_mode(CI231X_RF_MOSI_PORT, CI231X_RF_MOSI_PIN);
#if (CI231X_RF_SPI_NWIRE == SPI_4_WIRE)
	dpmu_set_io_reuse(CI231X_RF_MISO_PIN_PAD, FIRST_FUNCTION);
	dpmu_set_io_direction(CI231X_RF_MISO_PIN_PAD, DPMU_IO_DIRECTION_INPUT);
	dpmu_set_io_pull(CI231X_RF_MISO_PIN_PAD, DPMU_IO_PULL_DISABLE);
	gpio_set_input_mode(CI231X_RF_MISO_PORT, CI231X_RF_MISO_PIN);
	CI231X_RF_MISO_HIGH;
#endif
	CI231X_RF_MOSI_HIGH; // CI231X_RF_MOSI_LOW;//CI231X_RF_MOSI_HIGH;
	Delay1us(1);
	CI231X_RF_CSN_HIGH;
	Delay1us(1);
	CI231X_RF_SCK_LOW;
	Delay1us(1);
   //hal_gpio_init();
}
// 3线SPI写操作函数
void spi_3wire_send_byte(unsigned char TxData)
{
	unsigned char i;
	unsigned char data_output_bit;

	// 设置DIO为输出
	gpio_set_output_mode(CI231X_RF_MOSI_PORT, CI231X_RF_MOSI_PIN);
	for (i = 0; i < 8; i++)
	{
		data_output_bit = TxData & 0x80 ? 1 : 0;
		CI231X_RF_SCK_LOW;
		Delay1us(10);
		if (data_output_bit)
		{
			CI231X_RF_MOSI_HIGH;
		}
		else
		{
			CI231X_RF_MOSI_LOW;
		}
		Delay1us(10);
		CI231X_RF_SCK_HIGH;
		Delay1us(10);
		TxData = TxData << 1;
	}

	CI231X_RF_MOSI_LOW;
	Delay1us(10);
	CI231X_RF_SCK_LOW;
	Delay1us(10);
}

// 3线SPI读操作函数
unsigned char spi_3wire_read_byte(void)
{
	unsigned char bit_ctr;
	unsigned char byte = 0;
	unsigned char temp = 0;
	dpmu_set_io_direction(CI231X_RF_MOSI_PIN_PAD, DPMU_IO_DIRECTION_INPUT);
	gpio_set_input_mode(CI231X_RF_MOSI_PORT, CI231X_RF_MOSI_PIN);
	Delay1us(10);
	for (bit_ctr = 0; bit_ctr < 8; bit_ctr++) // output 8-bit
	{
		byte = byte << 1;
		CI231X_RF_SCK_HIGH;
		Delay1us(10);
		temp = gpio_get_input_level_single(CI231X_RF_MOSI_PORT, CI231X_RF_MOSI_PIN);
		byte |= temp;
		CI231X_RF_SCK_LOW;
		Delay1us(10);
	}

	return (byte); // return read byte
}

#if 1
// 4线spi读写---add by hw

static void Delayspi1us(unsigned int cnt)
{
	volatile unsigned int i, j;
	for (i = 0; i < cnt; i++)
	{
	}
}

#define DELAY_SPI Delayspi1us(2)
unsigned char spi_4wire_wrd(unsigned char TxData)
{
	unsigned char bit_ctr;
	unsigned char byte_read = 0;
	unsigned char temp = 0;

	if (TxData == ((unsigned char)0))
	{
		// mprintf("4 wire spi read\n");
		// 读 miso
		DELAY_SPI;
		for (bit_ctr = 0; bit_ctr < 8; bit_ctr++)
		{
			byte_read = byte_read << 1;
			CI231X_RF_SCK_HIGH;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 1);
#endif
			DELAY_SPI;
			temp = gpio_get_input_level_single(CI231X_RF_MISO_PORT, CI231X_RF_MISO_PIN);
#if SPI_OUT
			if (temp)
			{
				gpio_set_output_level_single(PC, pin_3, 1);
			}
			else
			{
				gpio_set_output_level_single(PC, pin_3, 0);
			}

#endif
			byte_read |= temp;
			CI231X_RF_SCK_LOW;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 0);
#endif
			DELAY_SPI;
		}
	}
	else
	{
		// mprintf("4 wire spi write\n");
		// 写
		for (bit_ctr = 0; bit_ctr < 8; bit_ctr++)
		{
			temp = TxData & 0x80 ? 1 : 0;
			CI231X_RF_SCK_LOW;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 0);
#endif
			DELAY_SPI;
			if (temp)
			{
				CI231X_RF_MOSI_HIGH;
#if SPI_OUT
				gpio_set_output_level_single(PA, pin_4, 1);
#endif
			}
			else
			{
				CI231X_RF_MOSI_LOW;
#if SPI_OUT
				gpio_set_output_level_single(PA, pin_4, 0);
#endif
			}
			// DELAY_SPI;
			CI231X_RF_SCK_HIGH;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 1);
#endif
			DELAY_SPI;
			TxData = TxData << 1;
		}
		CI231X_RF_MOSI_LOW;
#if SPI_OUT
		gpio_set_output_level_single(PA, pin_4, 0);
#endif
		DELAY_SPI;
		CI231X_RF_SCK_LOW;
#if SPI_OUT
		gpio_set_output_level_single(PA, pin_3, 0);
#endif
		DELAY_SPI;
	}
	return ((unsigned char)byte_read);
}
#endif

// 封装起来的对外可调用的SPI写函数
void spi_send_byte(unsigned char data)
{
#if (CI231X_RF_SPI_NWIRE == SPI_3_WIRE)
	// 3线SPI写
	spi_3wire_send_byte(data);
#else
	// 4线SPI读写
	spi_4wire_wrd(data);
#endif
}

// 封装起来的对外可调用的SPI读函数
unsigned char spi_read_byte(void)
{
#if (CI231X_RF_SPI_NWIRE == SPI_3_WIRE)
	// 3线SPI读
	return spi_3wire_read_byte();
#else
	// 4线SPI读
	return spi_4wire_wrd(0);
#endif
}

// CI231X写一个寄存器一个值操作函数
void ci231x_rf_write_byte(unsigned char addr, unsigned char D)
{
	CI231X_RF_CSN_LOW;
	Delay1us(1);
	spi_send_byte(CI231X_RF_W_REGISTER | addr);
	spi_send_byte(D);
	CI231X_RF_CSN_HIGH;
	Delay1us(1);
}

// CI231X写一个寄存器多个数值操作函数
void ci231x_rf_wr_buffer(unsigned char addr, const unsigned char *buf, unsigned char len)
{
	CI231X_RF_CSN_LOW;
	Delay1us(10);
	spi_send_byte(CI231X_RF_W_REGISTER | addr);
	while (len--)
	{
		spi_send_byte(*buf++);
	}
	CI231X_RF_CSN_HIGH;
	Delay1us(10);
}

// CI231X读一个寄存器一个值操作函数
unsigned char ci231x_rf_read_byte(unsigned char addr)
{
	unsigned char rxdata;

	CI231X_RF_CSN_LOW;
	Delay1us(10);
	spi_send_byte(CI231X_RF_REGISTER | addr);
	rxdata = spi_read_byte();
	CI231X_RF_CSN_HIGH;
	Delay1us(10);

	return (rxdata);
}

// CI231X读一个寄存器多个值操作函数
void ci231x_rf_read_buffer(unsigned char addr, unsigned char *buf, unsigned char len)
{
	CI231X_RF_CSN_LOW;
	spi_send_byte(CI231X_RF_REGISTER | addr);

	while (len--)
	{
		*buf++ = spi_read_byte();
	}
	CI231X_RF_CSN_HIGH;
}

// CI231X直接发命令函数
void ci231x_rf_operation(unsigned char opt)
{
	CI231X_RF_CSN_LOW;
	Delay1us(10);
	spi_send_byte(opt);
	CI231X_RF_CSN_HIGH;
	Delay1us(10);
}

/*CI231X 读写寄存器函数
 cmd = code;
 D = data
 */
void ci231x_rf_wr_cmd(unsigned char cmd, unsigned char D)
{
	CI231X_RF_CSN_LOW;
	Delay1us(10);
	spi_send_byte(cmd);
	spi_send_byte(D);
	CI231X_RF_CSN_HIGH;
	Delay1us(10);
}


// CI231X 切换速率
void ci231x_change_rate(CI231X_Rate_TypeDef rate)
{
	unsigned char tmp;

	tmp = ci231x_rf_read_byte(CI231X_RF_BANK0_RF_SETUP);

	if (rate == Rate_1M)
	{
		tmp &= 0xf7;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
	}
	else if (rate == Rate_2M)
	{
		tmp |= 0x08;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
	}
}

/*
 * @brief 切换信道
 *
 * @param chn 值为2-80，表示蓝牙芯片发送频段为2402-2480Mhz，每个频道间隔2Mhz
 *
 */
void ble_change_channel(unsigned char chn)
{
	if (chn < 0x80)
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_CH, chn);
}

/*
 * @brief 切换功率
 *
 * @param pwr 值为0-7，表示蓝牙芯片发送功率为-43db到8db
 *
 */

void ci231x_change_pwr(CI231X_Pwr_TypeDef pwr)
{
	unsigned char tmp;

	tmp = ci231x_rf_read_byte(CI231X_RF_BANK0_RF_SETUP);
	tmp &= ~0x47; // 1000111->10111000
	// tmp |= 0X80;
	switch (pwr)
	{
	case Pwr_8db:
		tmp |= 0x47;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_5db:
		tmp |= 0x40;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_4db:
		tmp |= 0x07;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_0db:
		tmp |= 0x03;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n6db:
		tmp |= 0x01;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n12db:
		tmp |= 0x01;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n16db:
		tmp |= 0x00;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n43db:
		tmp |= 0x00;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;

	default: // 默认就设置成0db
		tmp |= 0x03;
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
		break;
	}
}

/*
 * @brief 切换载波模式
 *
 * @param mod 1位普通模式，2位载波模式
 *
 */
void ble_change_mode(CI231X_ModeTypeDef mod)
{
	unsigned char tmp;

	tmp = ci231x_rf_read_byte(CI231X_RF_BANK0_CONFIG);

	if (mod == CI231X_RF_Carrier_Mode)
	{
		tmp |= 0X80 | ci231x_rf_read_byte(CI231X_RF_BANK0_RF_SETUP); // 0x80 | ci231x_rf_read_byte(CI231X_RF_BANK0_RF_SETUP);
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp);
	}
}

// CI231X 切换地址
void ci231x_change_addr(unsigned char *buf, unsigned char len)
{
	if (len <= 6)
		ci231x_rf_wr_buffer(CI231X_RF_BANK0_RX_ADDR_P0, buf, len); // set address
}

/*CI231X 接收数据包函数
buf：读取到的数据存放的地方
return：正常读取之后，返回的是读取到的数据包长度
*/
unsigned char ci231x_receive_pack(unsigned char *buf)
{
	unsigned char sta;
	unsigned char fifo_sta;
	unsigned char len;

	sta = ci231x_rf_read_byte(CI231X_RF_BANK0_STATUS);

	if (CI231X_RF_STATUS_RX_DR & sta)
	{
		do
		{
			len = ci231x_rf_read_byte(CI231X_RF_RX_PL_WID);

			if (len <= CI231X_RF_FIFO_MAX_PACK_SIZE)
			{
				ci231x_rf_read_buffer(CI231X_RF_RX_PAYLOAD, buf, len);
			}
			else
			{
				ci231x_rf_flush_rx();
			}

			fifo_sta = ci231x_rf_read_byte(CI231X_RF_BANK0_FIFO_STATUS);
		} while (!(fifo_sta & CI231X_RF_FIFO_STA_RX_EMPTY)); /*not empty continue read out*/

		ci231x_rf_write_byte(CI231X_RF_BANK0_STATUS, sta); /*clear irq*/
		ci231x_clear_all_irq();
		ci231x_flush_tx();
		ci231x_rf_flush_rx();
		return len;
	}

	if (sta & (CI231X_RF_STATUS_RX_DR | CI231X_RF_STATUS_TX_DS | CI231X_RF_STATUS_MAX_RT))
	{
		ci231x_rf_write_byte(CI231X_RF_BANK0_STATUS, sta); /*clear irq*/
	}
#if 1
	/* 以下的复位RF和让RF睡眠唤醒的动作是必须的，不可缺少 */
	if (rf_reset_cnt == 0) // 如果300ms都没有收到1包数据，则复位RF重新初始化
	{

		ci231x_init();
		ci231x_rf_write_byte(CI231X_RF_BANK0_FEATURE, 0x10);
		ci231x_rf_write_byte(CI231X_RF_BANK0_EN_AA, 0x00);
		ci231x_rf_write_byte(CI231X_RF_BANK0_CONFIG, 0xfa);
		// ci231x_rf_write_byte(CI231X_RF_BANK0_RX_PW_P0,10);
		ci231x_rf_write_byte(CI231X_RF_BANK0_RX_PW_P0, rx_lenth);
		ci231x_rf_write_byte(CI231X_RF_BANK0_RF_CH, 5);
		ci231x_rf_write_byte(CI231X_RF_BANK0_EN_RXADDR, 0x03);
		ci231x_rf_ce_high();

		// rf_reset_cnt=rf_reset_time;
	}

	if (rf_sleep_cnt == 0) // 如果20ms都没有收到1包数据，则让RF睡眠唤醒一次
	{

		ci231x_rf_ce_low();
		ci231x_rf_flush_rx();
		ci231x_clear_all_irq();

		ci231x_rf_write_byte(CI231X_RF_BANK0_PMU_CTL, 0xae);
		Delay1ms(1); // 这里的1ms延时不可缺少，也不能小
		ci231x_rf_write_byte(CI231X_RF_BANK0_PMU_CTL, 0xac);
		Delay1ms(1); // 这里的1ms延时不可缺少，也不能小
		ci231x_rf_ce_high();
		rf_sleep_cnt = 10;
	}
	/* 以上的复位RF和让RF睡眠唤醒的动作是必须的，不可缺少 */
#endif
	return 0;
}

/* CI231X发送数据包函数
  buf:要发送的数据包内容指针
  len:数据包的长度，从1到32字节
  cmd: 发送数据包的命令，CI231X_RF_TX_PAYLOAD和CI231X_RF_TX_PAYLOAD_NOACK
*/
void ci231x_send_pack(unsigned char cmd, unsigned char *buf, unsigned char len)
{
	unsigned char sta;
#if 0
	for(int i = 0;i < 10;i++)
	{
		mprintf("buf[%d]:%d\n",i,buf[i]);
	}
#endif
	sta = ci231x_rf_read_byte(CI231X_RF_BANK0_STATUS);
	if (!(sta & CI231X_RF_STATUS_TX_FULL))
	{
		// mprintf("sta:%d",sta);
		ci231x_rf_wr_buffer(cmd, buf, len);
	}
	else
		mprintf("tx_full\n");
}

// CI231X 清中断标志
void ci231x_clear_all_irq(void)
{
	ci231x_rf_write_byte(CI231X_RF_BANK0_STATUS, 0x70);
}

// CI231X 清空TX FIFO
void ci231x_flush_tx(void)
{
	ci231x_rf_operation(CI231X_RF_FLUSH_TX);
}

// CI231X 清空RX FIFO
void ci231x_rf_flush_rx(void)
{
	ci231x_rf_operation(CI231X_RF_FLUSH_RX);
}

// CI231X CE拉高
void ci231x_rf_ce_high(void)
{
    ci231x_rf_operation(CI231X_RF_CMD_CE_HIGH);
}
// CI231X CE拉低
void ci231x_rf_ce_low(void)
{
	ci231x_rf_operation(CI231X_RF_CMD_CE_LOW);
}

// 读取CI231X接收到的数据包长度的函数
unsigned char CI231X_rf_read_payload_length(void)
{
	return ci231x_rf_read_byte(CI231X_RF_RX_PL_WID);
}
