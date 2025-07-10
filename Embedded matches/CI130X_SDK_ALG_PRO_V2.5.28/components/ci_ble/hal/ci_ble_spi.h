
#ifndef __CI231X_RF_H__
#define __CI231X_RF_H__
#include "user_config.h"
#include "sdk_default_config.h"
#include "ci130x_gpio.h"
#define CI231X_RF_CSN_PORT    	PB		              
#define CI231X_RF_CSN_PIN		pin_4
#define CI231X_RF_CSN_PIN_PAD	PB4			       
#define CI231X_RF_CSN_PIN_FUNCTION		FIRST_FUNCTION

// PB4
#define CI231X_RF_SCK_PORT    	PB		              
#define CI231X_RF_SCK_PIN		pin_3
#define CI231X_RF_SCK_PIN_PAD	PB3

#define CI231X_RF_MOSI_PORT    PB
#define CI231X_RF_MOSI_PIN		pin_1		              
#define CI231X_RF_MOSI_PIN_PAD	PB1	

#define CI231X_RF_MISO_PORT    PB
#define CI231X_RF_MISO_PIN		pin_2	              
#define CI231X_RF_MISO_PIN_PAD	PB2

#define CI231X_RF_IRQ_PORT    	PB
#define CI231X_RF_IRQ_PIN		pin_0		              
#define CI231X_RF_IRQ_PIN_PAD	PB0	

 
		
#define CI231X_RF_SCK_HIGH		gpio_set_output_level_single(CI231X_RF_SCK_PORT,CI231X_RF_SCK_PIN,1);
#define CI231X_RF_SCK_LOW		gpio_set_output_level_single(CI231X_RF_SCK_PORT,CI231X_RF_SCK_PIN,0);

#define CI231X_RF_MOSI_HIGH	gpio_set_output_level_single(CI231X_RF_MOSI_PORT,CI231X_RF_MOSI_PIN,1);
#define CI231X_RF_MOSI_LOW		gpio_set_output_level_single(CI231X_RF_MOSI_PORT,CI231X_RF_MOSI_PIN,0);

#define CI231X_RF_CSN_HIGH		gpio_set_output_level_single(CI231X_RF_CSN_PORT,CI231X_RF_CSN_PIN,1);
#define CI231X_RF_CSN_LOW		gpio_set_output_level_single(CI231X_RF_CSN_PORT,CI231X_RF_CSN_PIN,0);

#define CI231X_RF_MISO_HIGH	gpio_set_output_level_single(CI231X_RF_MISO_PORT,CI231X_RF_MISO_PIN,1);
#define CI231X_RF_MISO_LOW		gpio_set_output_level_single(CI231X_RF_MISO_PORT,CI231X_RF_MISO_PIN,0);

#define CI231X_RF_SDIO_HIGH	gpio_set_output_level_single(CI231X_RF_MISO_PORT,CI231X_RF_MISO_PIN,1);
#define CI231X_RF_SDIO_LOW		gpio_set_output_level_single(CI231X_RF_MISO_PORT,CI231X_RF_MISO_PIN,0);


#define SPI_3_WIRE						3 // SPI使用3线制通信
#define SPI_4_WIRE						4 // SPI使用4线制通信

#define CI231X_RF_SPI_NWIRE			SPI_4_WIRE // 使用几线制SPI通信方式

#define rx_lenth					20

//-------------RF command---------------------
#define CI231X_RF_REGISTER			    	0x00 // Define read command to register
#define CI231X_RF_W_REGISTER				0x20 // Define write command to register
#define CI231X_RF_RX_PAYLOAD				0x61 // Define RX payload register address
#define CI231X_RF_TX_PAYLOAD				0xA0 // Define TX payload register address
#define CI231X_RF_FLUSH_TX					0xE1 // Define flush TX register command
#define CI231X_RF_FLUSH_RX					0xE2 // Define flush RX register command
#define CI231X_RF_REUSE_TX_PL				0xE3 // Define reuse TX payload register command
#define CI231X_RF_ACTIVATE					0x50 // Define ACTIVATE features register command
#define CI231X_RF_RX_PL_WID				0x60 // Define read RX payload width register command
#define CI231X_RF_ACK_PAYLOAD			0xA8 // Define write ACK payload register command
#define CI231X_RF_TX_PAYLOAD_NOACK		0xB0 // Define disable TX ACK for one time register command
#define CI231X_RF_NOP						0xFF // Define No Operation, might be used to read status register
#define CI231X_RF_READ_STATUS				0xFF

#define CI231X_RF_ACTIVATE_DATA			0x53

#define CI231X_RF_CMD_CE_HIGH				0xD5
#define CI231X_RF_CMD_CE_LOW				0xD6

///  CI231X Bank0 Register Addr
/**************************************************/               
#define CI231X_RF_BANK0_CONFIG             0x00 // 'Config' register address
#define CI231X_RF_BANK0_EN_AA              0x01 // 'Enable Auto Acknowledgment' register address
#define CI231X_RF_BANK0_EN_RXADDR          0x02 // 'Enabled RX addresses' register address
#define CI231X_RF_BANK0_PMU_CTL            0x03 // 'Setup address width' register address
#define CI231X_RF_BANK0_SETUP_RETR         0x04 // 'Setup Auto. Retrans' register address
#define CI231X_RF_BANK0_RF_CH              0x05 // 'CI231x channel' register address
#define CI231X_RF_BANK0_RF_SETUP       	   0x06 // 'CI231x setup' register address
#define CI231X_RF_BANK0_STATUS             0x07 // 'Status' register address                  
#define CI231X_RF_BANK0_RX_ADDR_P0         0x0A // 'RX address pipe0' register address
#define CI231X_RF_BANK0_ACCESS_ADDR        0x0A
#define CI231X_RF_BANK0_RX_ADDR_P1         0x0B // 'RX address pipe1' register address
#define CI231X_RF_BANK0_BLE_TIM_CNT		   0x0B 
#define CI231X_RF_BANK0_RX_ADDR_P2         0x0C // 'RX address pipe2' register address
#define CI231X_RF_BANK0_BLE_CTL            0x0C // 'BLE Ctrol' register address
#define CI231X_RF_BANK0_TX_ADDR            0x10 // 'TX address' register address
#define CI231X_RF_BANK0_TX_ADVA_ADDR       0x10
#define CI231X_RF_BANK0_RX_PW_P0           0x11 // 'RX payload width, pipe0' register address
#define CI231X_RF_BANK0_TX_HEADER			0x11 // 'BLE TX Header' register address
#define CI231X_RF_BANK0_RX_PW_P1           0x12 // 'RX payload width, pipe1' register address
#define CI231X_RF_BANK0_CRC_INIT           0x12
#define CI231X_RF_BANK0_RX_PW_P2           0x13 // 'RX payload width, pipe2' register address
#define CI231X_RF_BANK0_STATUS_EXT         0x16
#define CI231X_RF_BANK0_FIFO_STATUS        0x17 // 'FIFO Status Register' register address
#define CI231X_RF_BANK0_CONFIG_EXT         0x18
#define CI231X_RF_BANK0_DYNPD              0x1C // 'Enable dynamic payload length' register address
#define CI231X_RF_BANK0_FEATURE            0x1D // 'Feature' register address
#define CI231X_RF_BANK0_SETUP_VALUE        0x1E
#define CI231X_RF_BANK0_PRE_GURD           0x1F

///   CI231X Bank1 register
#define CI231X_RF_BANK1_CHIP_ID            0x00
#define CI231X_RF_BANK1_PLL_CTL0           0x01
#define CI231X_RF_BANK1_PLL_CTL1           0x02
#define CI231X_RF_BANK1_CAL_CTL            0x03
#define CI231X_RF_BANK1_STATUS             0x07
#define CI231X_RF_BANK1_STATE              0x08
#define CI231X_RF_BANK1_CHAN               0x09
#define CI231X_RF_BANK1_FDEV               0x0C
#define CI231X_RF_BANK1_DAC_RANGE          0x0D
#define CI231X_RF_BANK1_CTUNING            0x0F
#define CI231X_RF_BANK1_FTUNING            0x10
#define CI231X_RF_BANK1_RX_CTRL            0x11
#define CI231X_RF_BANK1_FAGC_CTRL_1        0x13
#define CI231X_RF_BANK1_DOC_DACI           0x1A
#define CI231X_RF_BANK1_DOC_DACQ           0x1B
#define CI231X_RF_BANK1_AGC_CTRL           0x1C
#define CI231X_RF_BANK1_AGC_GAIN           0x1D
#define CI231X_RF_BANK1_RF_IVGEN       	0x1E
#define CI231X_RF_BANK1_TEST_PKDET         0x1F

#define CI231X_RF_FIFO_MAX_PACK_SIZE		0x20
#define CI231X_RF_FIFO_STA_RX_FULL			0x02
#define CI231X_RF_FIFO_STA_RX_EMPTY		0x01

#define CI231X_RF_STATUS_RX_DR				0x40
#define CI231X_RF_STATUS_TX_DS				0x20
#define CI231X_RF_STATUS_MAX_RT			0x10
#define CI231X_RF_STATUS_TX_FULL			0x01

#ifndef BIT0
#define BIT0	              0x00000001
#define BIT1	              0x00000002
#define BIT2	              0x00000004
#define BIT3	              0x00000008
#define BIT4	              0x00000010
#define BIT5	              0x00000020
#define BIT6	              0x00000040
#define BIT7	              0x00000080
#define BIT8	              0x00000100
#define BIT9	              0x00000200
#define BIT10	              0x00000400
#define BIT11	              0x00000800
#define BIT12	              0x00001000
#define BIT13	              0x00002000
#define BIT14	              0x00004000
#define BIT15	              0x00008000
#endif

//config register bit define
#define PRIM_RX                         BIT0
#define PWR_UP                          BIT1
#define CE_REG                          BIT2
#define EN_CRC                          BIT3
#define MASK_MAX_RT                     BIT4
#define MASK_TX_DS                      BIT5
#define MASK_RX_DR                      BIT6
#define GUARD_EN                        BIT7

//pmu_ctl register bit define
#define CI231X_RF_PWRDN_MASK				0x03   
#define CI231X_RF_PWRDN_WORK				0x00 
#define CI231X_RF_PWRDN_SDAND_BY			0x02 
#define CI231X_RF_PWRDN_SLEEP				0x01 

#define DLDO_ENB_REG                    BIT3
#define DLDO_ENB_MN                     BIT4

//DYNPD register bit define
#define SPI4_EN                         BIT3
#define XN297_EN                        BIT4
#define BYPASS_IO                       BIT5
#define REG_OUTPUT_EN                   BIT6
#define REG_OUTPUT                      BIT7

//FEATURE register bit define
#define EN_DYN_ACK                      BIT0
#define EN_ACK_PAY                      BIT1
#define EN_DPL                          BIT2
#define VCO_AMP_TX_MUX                  BIT3
#define BP_GAU                          BIT4
#define SOFT_RST                        BIT5
#define BLE_EN                          BIT6
#define LONG_PACKET_EN                  BIT7

//STATUS_EXT register bit define
#define LENERR                          BIT2
#define CRCERR                          BIT3
#define TX_AEMPTY                       BIT0
#define RX_AFULL                        BIT1
#define RTC_TIMER_INT                   BIT2
#define RX_TIMER_OUT                    BIT3
#define SYNC_DS													BIT4
#define TX_DS            								BIT5
#define RX_DR            								BIT6
#define EVT_DS           								BIT7


typedef enum {
	CI231X_RF_Bank0 = 0,
	CI231X_RF_Bank1 = 0x80
}CI231X_Bank_TypeDef;

typedef enum {
	CI231X_RF_PRX_Mode = 0,
	CI231X_RF_PTX_Mode,
	CI231X_RF_Carrier_Mode
}CI231X_ModeTypeDef;

typedef enum {
	Rate_1M = 0,
	Rate_2M = 0x80
}CI231X_Rate_TypeDef;

typedef enum {
	Pwr_n43db = 0,
	Pwr_n16db,
	Pwr_n12db,
	Pwr_n6db,
	Pwr_0db,
	Pwr_4db,
	Pwr_5db,
	Pwr_8db,
}CI231X_Pwr_TypeDef;

/*********************涉及到最底层的操作函数***********************/
void Delay1ms(unsigned int cnt);
void Delay1us(unsigned int cnt);
// 4线SPI读写1字节数据操作函数
unsigned char spi_4wire_wrd(unsigned char Data);
// 3线SPI写1字节数据操作函数
void spi_3wire_send_byte(unsigned char TxData);
// 3线SPI读1字节数据操作函数
unsigned char spi_3wire_read_byte(void);
// OM6220 模拟SPI(3,4线)接口初始化函数
void ci231x_spi_init(void);
// OM6220 硬件SPI(4线)接口初始化函数
void hard_spi_init(void);

// 封装起来对外用的SPI写1字节数据操作函数
void spi_send_byte(unsigned char data);
// 封装起来对外用的SPI读1字节数据操作函数
unsigned char spi_read_byte(void);

// OM6220 直接写命令的函数
void ci231x_rf_operation(unsigned char opt);
// OM6220 写一个寄存器一个值的函数
void ci231x_rf_write_byte(unsigned char addr,unsigned char D);
// OM6220 写一个寄存器多个值的函数
void ci231x_rf_wr_buffer(unsigned char addr,const unsigned char* buf,unsigned char len);
// OM6220 读一个寄存器一个值的函数
unsigned char ci231x_rf_read_byte(unsigned char addr);
// OM6220 读一个寄存器多个值的函数
void ci231x_rf_read_buffer(unsigned char addr, unsigned char* buf, unsigned char len);

// OM6220 清标志
void ci231x_clear_all_irq(void);
// OM6220 清TX FIFO
void ci231x_flush_tx(void);
// OM6220 清RX FIFO
void ci231x_rf_flush_rx(void);
// OM6220 CE 拉高
void ci231x_rf_ce_high(void);
// OM6220 CE 拉低
void ci231x_rf_ce_low(void);


// OM6220 切换速率
void ci231x_change_rate(CI231X_Rate_TypeDef rate);
// OM6220 切换信道
void ble_change_channel(unsigned char chn);
// OM6220 切换功率
void ci231x_change_pwr(CI231X_Pwr_TypeDef pwr);
// OM6220 切换地址
void ci231x_change_addr(unsigned char *buf, unsigned char len);
// OM6220 发送数据包
void ci231x_send_pack(unsigned char cmd, unsigned char* buf, unsigned char len);
// OM6220 接收数据包
unsigned char ci231x_receive_pack(unsigned char *buf);
// OM6220 读取接收数据包长度
unsigned char CI231X_rf_read_payload_length(void);

uint32_t timer3_get_time_us(void);
void timer3_init(void);
void send_ble_packet_test(void);
uint32_t timer3_get_sys_tick();

#endif

/*-------------------------------------------End Of File---------------------------------------------*/









