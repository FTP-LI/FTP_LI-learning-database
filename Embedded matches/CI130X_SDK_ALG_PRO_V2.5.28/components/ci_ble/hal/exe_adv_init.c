#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "stdlib.h"
#include "stdio.h"
#include "exe_api.h"
#include "exe_adv_init.h"
#include "ci_ble_spi.h"
#include "ci_log.h"
#include "cias_heattable_msg_deal.h"
#include "rf_msg_deal.h"
#include "prompt_player.h"
#include "system_msg_deal.h"
#include "rf_msg_deal.h"
#include "ci_flash_data_info.h"
#define PACKET_LEN 16
#define SPI_WIRE  SPI_4_WIRE  // 使用几线制SPI通信方式

unsigned char Rx_Payload[42];
// BLE广播的三个频点
static const unsigned char channel_index[3] = {
	2, // BLE CHANNLE MAP 37
	26, // BLE CHANNLE MAP 38
	80 // BLE CHANNLE MAP 39
};
unsigned char adv_buf[42] = {
  ///.header
  0x02, 37,
  ///.AdvA
  0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  ///Flags: BLE limited discoverable mode and BR/EDR not supported
  0x02, 0x01, 0x06,
  ///incomplete list of service class UUIDs: (0x1812, 0x180F)
  0x07, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  ///Complete Local Name in 18 bytes always.
  0x07, 0x09,
  'C','I','_','B','L','E',
  ///RFstar BLE Light's MANUFACTURER_ID
  //2, 1, 6, 3, 2, -80, -1, 9, -1, 83, -85, -38, 26, 31, -54, -25, -125,
  ///CRC24
  0, 0, 0,
};
void user_dev_adv_init(unsigned char dev_type_id, unsigned char dev_number_id, unsigned char config_type)
{
  
  adv_buf[14] = config_type;
  adv_buf[15] = dev_type_id;
  adv_buf[16] = dev_number_id;

  unsigned short crc_cal = crc16_ccitt(0, &adv_buf[13], 4);
  adv_buf[17] = crc_cal>>8;
  adv_buf[18] = crc_cal&0xFF;        //设备初始化厂商信息                                             

}


unsigned char SPI_Operation(unsigned char opt)
{	
	ci231x_rf_operation(opt);
	return 1;
}


void SPI_wr_cmd(unsigned char cmd,unsigned char Data)
{
	ci231x_rf_wr_cmd(cmd,Data);
}


void SPI_write_byte(unsigned char addr,unsigned char D)
{
	ci231x_rf_write_byte(addr,D);
}


unsigned char SPI_read_byte(unsigned char addr)
{
	unsigned char rxdata;
	
	rxdata = ci231x_rf_read_byte(addr);
	
	return(rxdata);
}


void SPI_wr_buffer(unsigned char addr,unsigned char* buf,unsigned char len)
{
	ci231x_rf_wr_buffer(addr,buf,len);
}


void SPI_read_buffer(unsigned char addr,unsigned char *buf,unsigned char len)
{
	ci231x_rf_read_buffer(addr,buf,len);
}

/* 切换寄存器Bank */
void ci231x_rf_bank_switch(RF_Bank_TypeDef bank)
{  
	unsigned char sta;
	sta = ci231x_rf_read_status();

	if(bank != Rf_Bank0)
	{
		if(!(sta & CI231X_RF_BANK1))
			SPI_wr_cmd(CI231X_RF_ACTIVATE, CI231X_RF_ACTIVATE_DATA);
	}
	else
	{
		if(sta & CI231X_RF_BANK1)
			SPI_wr_cmd(CI231X_RF_ACTIVATE, CI231X_RF_ACTIVATE_DATA);
	}  
}


/* 设置CE脉冲 至少得时20us时间 */
void ci231x_rf_ce_pulse(void)
{
	ci231x_rf_ce_high();
	Delay1us(40); // > 20us
	ci231x_rf_ce_low();
}

/* 读状态寄存器 */
unsigned char ci231x_rf_read_status(void)
{	
	return SPI_read_byte(CI231X_RF_BANK0_STATUS);	
}

/* 切换RF通信地址宽度 */
void ci231x_rf_change_addr_wid(unsigned char wid)
{  
	// CI231X固定5字节地址  
}

/* 切换RF通信地址 */
void ci231x_rf_change_addr_reg(unsigned char* AddrBuf,unsigned char len)
{
//	ci231x_rf_ce_low();  
	SPI_wr_buffer(CI231X_RF_BANK0_RX_ADDR_P0, AddrBuf, len);  
	SPI_wr_buffer(CI231X_RF_BANK0_TX_ADDR, AddrBuf, len);
//	ci231x_rf_ce_high();
}

/* 切换RF输出功率 */
void ci231x_adv_change_pwr(rf_tx_power_t Pwr_lev)
{
	unsigned char reg_val;
	
//	ci231x_rf_ce_low();
	ci231x_rf_bank_switch(Rf_Bank0);  
	reg_val = SPI_read_byte(CI231X_RF_BANK0_RF_SETUP);
	reg_val &= ~0x47;
	reg_val |= Pwr_lev; 
	SPI_write_byte(CI231X_RF_BANK0_RF_SETUP, reg_val);  
//	ci231x_rf_ce_high();	
}

/* 切换RF通信频点 */
void ci231x_change_ch(unsigned char ch_index)
{
	if(ch_index > 83)
		ch_index = 83;
	
//	ci231x_rf_ce_low();

	SPI_write_byte(CI231X_RF_BANK0_RF_CH, ch_index);	

//	ci231x_rf_ce_high();		
}

/* 切换晶体负载电容值 */
void ci231x_change_xtalcc(unsigned char xtal_cc)
{
	unsigned char temp[4] = {0x00};
	
	if(xtal_cc > 15)
		xtal_cc = 15;
	
	// 先切换到bank1,晶体负载电容值从0到15,数值越小频率越往大走，数值越大频率越往小走
	ci231x_rf_bank_switch(Rf_Bank1);  
	SPI_read_buffer(CI231X_RF_BANK1_RF_IVGEN, temp, 4);
	temp[0] &= ~0x1f;
	temp[0] |= xtal_cc; // 这个值最大到15
	SPI_wr_buffer(CI231X_RF_BANK1_RF_IVGEN, temp, 4);  
	ci231x_rf_bank_switch(Rf_Bank0); // 设置完之后要记得切回bank0
}

/* 使能白化 */
void ci231x_enable_scramble(unsigned char en_flag)
{  
	unsigned char temp;
	
	if(en_flag)
	{
		SPI_read_byte(CI231X_RF_BANK0_EN_RXADDR);
		temp |= 0x08;
		temp |= 0x07;
		SPI_write_byte(CI231X_RF_BANK0_EN_RXADDR, temp);
	}
	else
	{
		SPI_read_byte(CI231X_RF_BANK0_EN_RXADDR);
		temp &= ~0x08;
		temp |= 0x07;
		SPI_write_byte(CI231X_RF_BANK0_EN_RXADDR, temp);
	}  
}

/* 读取RF芯片的ID */
unsigned char ci231x_get_chip_id(void)
{  
	unsigned char ReadArr[2];
	
	ci231x_rf_bank_switch(Rf_Bank1);
	SPI_read_buffer(CI231X_RF_BANK1_LINE, ReadArr, 2);
	ci231x_rf_bank_switch(Rf_Bank0);
	return ReadArr[0];  
}

/* RF工作模式切换 */
void ci231x_mode_switch(RF_ModeTypeDef mod)
{
	unsigned char tmp;

	ci231x_rf_ce_low();
	
	tmp = SPI_read_byte(CI231X_RF_BANK0_CONFIG);  
	if(mod != Rf_PRX_Mode)
		tmp &= 0xFE;
	else	
		tmp |= 0x01;
	SPI_write_byte(CI231X_RF_BANK0_CONFIG,tmp);  
	if(mod == Rf_Carrier_Mode)
	{
		tmp = 0x80 | SPI_read_byte(CI231X_RF_BANK0_RF_SETUP);
		SPI_write_byte(CI231X_RF_BANK0_RF_SETUP, tmp); /* 0xC7:8dbM ,0x83:0dBm*/
//		ci231x_rf_ce_high();
//		printf("enter Rf_Carrier_Mode.\n");
//		while(1);
		/* 注意：最高位为载波使能位，设置了载波模式之后，CE要拉高，载波才会出来；
	 如果不要切换频点，功率等，就不要去在操作RF，否则会看不到载波信号的*/
	}
}


/* TX_FIFO的内容重用，要使用这个数据包重用功能，要先在写TX_FIFO前写这个命令
** 然后CE一直为高就会一直往外发相同的数据；如果时CE脉冲方式，则一个脉冲就
** 发送医保相同的数据；
** 需要注意的时，使用这个命令，不能情况tx_fifo，也不能切换信道，否则tx_fifo的内容就没有了。*/
void ci231x_reuse_tx_pl(void)
{  
	SPI_Operation(CI231X_RF_REUSE_TX_PL);  
}



/* RF接收数据包的函数。 buf:读取RX_FIFO的数据到这里 */
unsigned char ci231x_adv_receive_pack(unsigned char* buf)
{
	unsigned char sta,fifo_sta,len;

	sta = ci231x_rf_read_status();
	
	if(CI231X_RF_STATUS_RX_DR & sta)
	{
		{
			len = SPI_read_byte(CI231X_RF_RX_PL_WID);
			SPI_read_buffer(CI231X_RF_RX_PAYLOAD, buf, len);
		}
		return len;
	}


	return 0;		
}

/* 写自动ACK数据到TX_FIFO，这个命令需要在进入接收之前先写好，接收到
** 要回ACK的数据之后就会自动把数据回回去发送端。*/
void ci231x_write_ack_payload(unsigned char PipeNum, unsigned char *pBuf, unsigned char bytes)
{
	SPI_wr_buffer(CI231X_RF_ACK_PAYLOAD | PipeNum, pBuf, bytes);
}

/* 批量配置寄存器的函数，这个函数在初始化的时候有用到 */
static void ci231x_configure_reg(const unsigned char* Dbuf)
{
	unsigned char cnt = 0, Reg_Addr, data_lenth;
	const unsigned char* pdata;
	
	while(1)
	{
		/*Get Reg_Addr*/
		Reg_Addr = Dbuf[cnt];	
		if(Reg_Addr == 0xFF)
		{
			break;	/*Reg_Addr Error,configuration is complete,break*/
		}

		/*Get Data_Lenth*/
		cnt += 1;
		data_lenth=Dbuf[cnt];	
		
		/*Get Data address*/
		cnt += 1;
		pdata = &Dbuf[cnt];	

		/*Write buffer*/ 
		SPI_wr_buffer(Reg_Addr, (unsigned char*)pdata, data_lenth);	
		cnt += data_lenth;
	}
}

/* 芯片进入低功耗模式 */
void ci231x_enter_sleep(void)
{  
	SPI_write_byte(CI231X_RF_BANK0_PMU_CTL, 0xae); // RF进入deepsleep模式设置

	Delay1ms(1);  
}
	
/* 芯片退出低功耗模式 */
void ci231x_leave_sleep(void)
{  
	SPI_write_byte(CI231X_RF_BANK0_PMU_CTL, 0xac); // 设置芯片退出deepsleep模式	

	Delay1ms(1);  
}


/* 软件复位RF */
void ci231x_soft_rst(void)
{  
	SPI_write_byte(CI231X_RF_BANK0_FEATURE, 0x20); // soft_reset  
}

extern uint8_t ble_xtal;
void ci231x_ble_init(void)
{
	unsigned char temp[6];

	ci231x_soft_rst(); // soft_reset
	SPI_write_byte(CI231X_RF_BANK0_DYNPD, 0x08);

	ci231x_rf_ce_low();	
	SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0x8b); // 0x03); // power up
	Delay1ms(3); // wait 3 ms
	SPI_write_byte(CI231X_RF_BANK0_PMU_CTL, 0x8c); //0x04); // RF_PWRDWN = 00 32K_en = 1
	Delay1ms(2); // wait 2 ms
	SPI_write_byte(CI231X_RF_BANK0_RF_CH, 0x28); 

	ci231x_rf_bank_switch(Rf_Bank1);
	SPI_write_byte(CI231X_RF_BANK1_TEST_PKDET, 0x20); // 0x24); // pll_vdiv2_sel = 01

//	temp[1] = 0x11; // vco_amp_mn=1, 手动模式
//	temp[0] = 0xc3; // vco_amp_ctl= 0xc最大值
//	SPI_wr_buffer(CI231X_RF_BANK1_FAGC_CTRL_1, temp, 2);
	SPI_write_byte(CI231X_RF_BANK1_FAGC_CTRL_1, 0x01);  // 300mV
	/* 必须在校准前写入。*/
   SPI_write_byte(CI231X_RF_BANK1_FDEV, 0x25); // 0x20=250khz 0x25=290khz
	ci231x_rf_bank_switch(Rf_Bank0);    

	SPI_write_byte(CI231X_RF_BANK0_FEATURE, 0x10);

	ci231x_rf_ce_high();
	Delay1us(300);
	ci231x_rf_ce_low();
	uint8_t run_time = 0;
	while((SPI_read_byte(CI231X_RF_BANK0_RF_SETUP) & 0x20) == 0x00) // wait cal done
	{	
		Delay1ms(2);
		run_time++;
		if(run_time > 35)
			return; // 等待35ms都没有检测到校准完成标志，则重新复位初始化RF，避免SPI有问题导致RF初始化有问题
	}
	
	SPI_write_byte(CI231X_RF_BANK0_RF_SETUP, SPI_read_byte(CI231X_RF_BANK0_RF_SETUP)&(~0x10)); // clear cal_en

	mprintf("cali done.\n");

	SPI_write_byte(CI231X_RF_BANK0_FEATURE, 0x48); // BLE_EN = 1, bp_gau = 0

	SPI_write_byte(CI231X_RF_BANK0_PRE_GURD, 0x67); // 1byte pramble

	ci231x_rf_bank_switch(Rf_Bank1);	

//	temp[3] = 0x28;
	temp[2] = 0x75; // bp_dac =1 bp_rc = 1
	temp[1] = 0x98; // bp_vco_amp = 1 bp_vco_ldo=1
	temp[0] = 0x20;
	SPI_wr_buffer(CI231X_RF_BANK1_CAL_CTL, temp, 3); // 4); // 

//	temp[2] = 0x11;
//	temp[1] = 0x04;  
//	temp[0] = 0x1D;    
//	SPI_wr_buffer(CI231X_RF_BANK1_RF_IVGEN, temp, 3); // xtal_cc = 0x1d
	SPI_write_byte(CI231X_RF_BANK1_RF_IVGEN, 0x17);

	ci231x_rf_bank_switch(Rf_Bank0);	
	
	SPI_write_byte(CI231X_RF_BANK0_FEATURE, 0x40); // BLE_EN=1, bp_gau=0(GFSK), vco_amp_tx_mux=0(small 

	SPI_write_byte(CI231X_RF_BANK0_PRE_GURD, 0x76); // 15-bit preamble

//	SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0x0a);
	SPI_write_byte(CI231X_RF_BANK0_EN_AA, 0x00);

//	SPI_write_byte(CI231X_RF_BANK0_SETUP_VALUE, 0xff); // rx timeout 
	SPI_write_byte(CI231X_RF_BANK0_CONFIG_EXT, 0x1d); // 边写tx fifo 边发送, 使能ci231x内部计数器

	SPI_write_byte(CI231X_RF_BANK0_BLE_SETUP_DELAY, 0xf0); // sub extra 7us delay

	temp[0] = 0x46;
	temp[1] = 0x0b;
	temp[2] = 0xaf;
	temp[3] = 0x43;
	temp[4] = 0x98;
	temp[5] = 0xaf;
	SPI_wr_buffer(CI231X_RF_BANK0_TX_ADVA_ADDR, temp, 6);

	temp[0] = 0x55;
	temp[1] = 0x55;
	temp[2] = 0x55;
	SPI_wr_buffer(CI231X_RF_BANK0_CRC_INIT, temp, 3);

	temp[0] = 0xd6;
	temp[1] = 0xbe;
	temp[2] = 0x89;
	temp[3] = 0x8e;
	SPI_wr_buffer(CI231X_RF_BANK0_ACCESS_ADDR, temp, 4); // 0x8E89BED6

	SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x03); // adv mode 
	
	ci231x_change_xtalcc(1);
}

void ble_adv_rx_process()
{
	volatile unsigned short status_ext = 0x00;
	unsigned char len = 0;
	while(1) // 芯片处于扫描模式，之后就退出去切发射发广播，如此循环
	{
		SPI_read_buffer(CI231X_RF_BANK0_STATUS_EXT, (unsigned char *)&status_ext, 2);
		if (status_ext & 0x08/*RX_TIME_OUT*/) 
		{
			ci231x_clear_all_irq();
			/* In case of rx timeout on the 2nd tifs rx. */
			SPI_Operation(0x8B); // 复位状态机命令，这个命令不可少
			SPI_Operation(0xD9); // CE脉冲命令
		} 
		else if (status_ext & 0x0E00/*RX erros*/) 
		{
			ci231x_clear_all_irq();
			/* In case of rx error on the 2nd tifs rx. */
			SPI_Operation(0x8B); // 复位状态机命令，这个命令不可少
			SPI_Operation(0xD9); // CE脉冲命令
		} 
		else if (status_ext & 0x40/*RX_DR*/) 
		{
			ci231x_clear_all_irq();
			memset(Rx_Payload, 0, sizeof(Rx_Payload));
			SPI_read_buffer(CI231X_RF_BANK0_RX_HEADER, &Rx_Payload[0], 2); // 新版本的芯片header从这里读出
			len = Rx_Payload[1];
			if (len > 0) 
			{
				SPI_read_buffer(CI231X_RF_BANK0_RX_ADVA_ADDR, &Rx_Payload[2], 6); // 新版本的芯片Adva从这里读出
				if (len > 6) 
				{
					SPI_read_buffer(CI231X_RF_RX_PAYLOAD, &Rx_Payload[8], len-6); // 新版本的芯片AdvData从这里读出
				} 
			}
			ci231x_rf_flush_rx();
			/* Reset fsm to abort scan's rx2tx2rx timing. */
			SPI_Operation(0x8B); // 复位状态机命令，这个命令不可少
			SPI_Operation(0xD9); // CE脉冲命令
			if ((Rx_Payload[8]== 0x11)&&(Rx_Payload[9] == 0x07))  
			{
				if(ci_rf_recv_group_data(&Rx_Payload[10]))
					break;
			}
			if ((Rx_Payload[14]== 0x11)&&(Rx_Payload[15] == 0x07))  //Apple
			{
				if(ci_rf_recv_group_data(&Rx_Payload[16]))
					break;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

void ble_adv_tx_process()
{
/* 循环发送5次广播，每次广播间隔30ms，每次在3个广播频点都发送信号 */
	unsigned short status_ext = 0;
	for(int j = 0; j < 200; j ++)
	{
		for(int idx = 0; idx < 3; idx ++) // 3个广播频点依次发送
		{
			SPI_write_byte(CI231X_RF_BANK0_RF_CH, channel_index[idx]); // 设置广播频点
			SPI_wr_buffer(CI231X_RF_BANK0_TX_HEADER, adv_buf, 2); // 设置广播包头信息
			SPI_wr_buffer(CI231X_RF_TX_PAYLOAD, adv_buf + 8, adv_buf[1] - 6); // 将广播内容写入TX FIFO
			SPI_Operation(0xD9); // 则使用D9命令激活发送
			while(1) // 等待数据发送完成	
			{
				status_ext = SPI_read_byte(CI231X_RF_BANK0_STATUS); // 读取状态寄存器看是否发送完成
				if ((CI231X_RF_STATUS_TX_DS & status_ext) || (CI231X_RF_STATUS_MAX_RT & status_ext)) // 如果发送完成
				{
					SPI_write_byte(CI231X_RF_BANK0_STATUS, status_ext); // 则清空状态寄存器
					break; 
				}
			}
			
		}
		vTaskDelay(pdMS_TO_TICKS(20)); // 延时30ms然后再去发送广播，SIG规定的广播里面也不会发的很快，这里要注意。
	}
}

void ble_adv_scan(void)
{

	volatile unsigned short status_ext = 0x00;
	unsigned char rf_chip_id;
	get_unique_id();
	ci231x_ble_init(); // RF初始化
	
	rf_chip_id = ci231x_get_chip_id() & 0xff; // 读取芯片的chip id 主要是为了区分新旧芯片
	SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0x8b); // CONFIG寄存器配置为接收模式，使能CRC
	SPI_write_byte(CI231X_RF_BANK0_RF_CH, channel_index[0]); // channle map:37:2402,38:2426,39:2480
	if(rf_chip_id > 0x24)
	{
		SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x06);	// 7:event abort,6:md dsb,5:nesn sn dsb,4:infiniterx,
													// 3~0:3-ble broacast,5-ble connection,9-ble tx test,b-ble rx test 
	}
	else
	{
		SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x0b); // 旧版本的芯片没有白名单模式
	}
	
//	CI231X_Dump_RF_Register(); // 打印RF所有寄存器的内容
	ci231x_adv_change_pwr(pwr_level_15);
	while(1)
	{
		
		mprintf("RX start!\n");
		/* 从广播模式切换到扫描模式，主要是配置CONFIG寄存器为接收模式，BLE_CTL为广播扫描模式，然后发一下8B命令复位下状态机 */
		ci231x_rf_ce_low();
		SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0x8b); // power up
		if(rf_chip_id > 0x24)
			SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x06);	//7:event abort,6:md dsb,5:nesn sn dsb,4:infiniterx,
														//3~0:3-ble broacast,5-ble connection,9-ble tx test,b-ble rx test 
		else
			SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x0b);
		if(rf_chip_id > 0x24)
			SPI_Operation(0x8B); // 复位状态机。。。旧的芯片没有这个命令，
		
		SPI_write_byte(CI231X_RF_BANK0_RF_CH, channel_index[0]); // 设置广播频点
		
		if(rf_chip_id > 0x24)
			SPI_Operation(0xD9); // CE脉冲命令，新版本芯片才有的，让芯片进入接收状态
		else	
			ci231x_rf_ce_high(); // 拉高CE使芯片处于接收状态，等待数据到来，就版本的芯片必须这么做
		
		/* 值得注意的是，早扫描期间，一定要一直去读取状态机存钱，看是否有数据到来，有数据到来要及时取走数据
		千万不要使用间隔多长时间去读取一次状态寄存器，然后读取数据的方式，这样很容易丢数据。
		已经有客户出现过这样的问题，就是因为不是一直读取状态寄存器，所以丢了数据，这并不是我们扫描不到数据。 */ 
		ble_adv_rx_process();

		mprintf("TX start!\n");
		/* 做为广播发送模式的时候，新旧芯片没什么太大的区别 */
		/* 从扫描模式切换到广播模式，主要是配置CONFIG寄存器为发射模式，BLE_CTL为广播模式，然后发一下8B命令复位下状态机 */
		ci231x_rf_ce_low();
		SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0x8A); // CONFIG寄存器配置为发送模式，使能CRC
		SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x03); // 配置为广播模式
		if(rf_chip_id > 0x24)
			SPI_Operation(0x8B); // 复位状态机。。。旧的芯片没有这个命令，
		
		ci231x_flush_tx(); // 清空TX FIFO
		ci231x_clear_all_irq(); // 清状态寄存器	
		ble_adv_tx_process();
	}
} 

uint8_t rf_unique_id[4];
//获取唯一id，用于单个控制
void get_unique_id()
{
	uint8_t flash_unique_id[20];
	post_spic_read_unique_id((uint8_t*)flash_unique_id);
	memcpy(rf_unique_id,&flash_unique_id[2], 4); //设备初始化MAC地址
}
extern void* ble_msg_queue;
//处理消息，返回为1表示需要转到发送模式发送广播
bool ci_rf_recv_group_data(unsigned char* buf)
{
	static unsigned char recv_counter = 255;
	static uint8_t group_id = 0xff;
	static bool is_config_group = false;
	unsigned char rf_recv_data[PACKET_LEN];
 	//recv_counter++;
	//mprintf("recv_counter = %d\r\n",recv_counter); 
	if (buf[0]== 0xA5)        //遥控器，正向
	{
		memcpy(rf_recv_data,buf,PACKET_LEN);
	}
	else if (buf[PACKET_LEN-1]== 0xA5)   //反向
	{
		for (size_t i = 0; i < PACKET_LEN; i++)
		{
			rf_recv_data[i] = buf[PACKET_LEN-1-i];
		}
	}
	else
		return 0;

	if (recv_counter == rf_recv_data[2]) //帧号重复
		return 0;	
 	for (size_t i = 0; i < 16; i++)
	{
		mprintf("%02X ",buf[i]);
	}
	mprintf("\r\n"); 
	recv_counter = rf_recv_data[2];
	switch (rf_recv_data[1])
	{
		case 0x76:            //对码
			//mprintf("对码\r\n");
			if (is_config_group)
				return 0;
			is_config_group = true;
			group_id = rf_recv_data[7];
			//prompt_play_by_voice_id(52, default_play_done_callback,true); 
			mprintf("对码成功 group_id = %d\r\n", group_id);
			if (rf_recv_data[3] == 0xFE) //APP对码，需返回falsh id
			{
				report_group();
				return 1;
			}
			break;
			
		case 0x78:            //清码
			if (!is_config_group)
					return 0;
			if (group_id == rf_recv_data[7])
			{
				mprintf("清码成功 group_id = %d\r\n", group_id);
				group_id = 0;
				is_config_group = false;
				//prompt_play_by_voice_id(53,default_play_done_callback ,true);
			}
			break;

		case 0x79:            //组控
			if (group_id == rf_recv_data[7])
			{
				covent_rfdata_to_bledata(rf_recv_data);
				ble_msg_V1_t *recv_ble_msg = (ble_msg_V1_t *)rf_recv_data;
				BaseType_t err = xQueueSend(ble_msg_queue, recv_ble_msg, 0);   
				if (err != pdPASS)
				{
					mprintf("deal_ble_recv_msg send fail:%d,%s\n",__LINE__,__FUNCTION__);
				}
			}
			else 
			{
				mprintf("其他分组：%d\r\n",rf_recv_data[7]);
			}
			break;	

		case 0x80:            //广播设备类型
			//mprintf("开始广播\r\n");
			return 1;	
		case 0xF9:           //单个控制
			if (memcmp(rf_unique_id, &rf_recv_data[4], 4) == 0)
			{
				covent_rfdata_to_bledata(rf_recv_data);
				ci_rf_recv_data_handle(rf_recv_data,PACKET_LEN);
			}				
			break;	

		default:
			break;
	}
	return 0; 
}
 
void report_group()
{
	adv_buf[13] = 0xA5;
	adv_buf[14] = 0x77;
	adv_buf[15] = 0xFF;
	adv_buf[16] = 0x00;
	memcpy(&adv_buf[17], rf_unique_id, 4);
	adv_buf[21] = 0x01;
	adv_buf[22] = 0x01;
	adv_buf[23] = 0x03;
	adv_buf[24] = 0x07;
}

bool covent_rfdata_to_bledata(unsigned char* buf)
{
	unsigned char rf_send_data[PACKET_LEN];
	memset(rf_send_data, 0, PACKET_LEN);
	rf_send_data[0] = 0xa5;//帧头
	rf_send_data[1] = 0x5a;

	rf_send_data[2] = 0x01;//协议版本
	rf_send_data[3] = 0x1;//厂商ID
	rf_send_data[4] = 0x3;//设备类型
	rf_send_data[5] = 0x7;//设备编号
	rf_send_data[6] = 0x11;//消息类型-功能高4位数据低4位
	if (buf[8]!= 0x03)
	{
		memcpy(&rf_send_data[7], &buf[8], 4);
	}
	else
	{
		rf_send_data[7] = buf[8];
		rf_send_data[8] = 0;
		rf_send_data[9] = 6;
		rf_send_data[15] = buf[11];
	}
	memcpy(buf,rf_send_data,PACKET_LEN);
	for (int i = 0; i < PACKET_LEN; i++)
	{
		mprintf("%02X ",buf[i]);
	}
	mprintf("\r\n");
}

#include "semphr.h"
#include "FreeRTOS.h"
//STATUS_EXT register bit define
#define  TX_AEMPTY        BIT0
#define  RX_AFULL         BIT1
#define  RTC_TIMER_INT    BIT2
#define  RX_TIME_OUT      BIT3
#define  SYNC_DS          BIT4
#define  TX_DS            BIT5
#define  RX_DR            BIT6
#define  EVT_DS           BIT7
#define  RX_SYNC_ERR      BIT8
#define  RX_TYPE_ERR      BIT9
#define  RX_LEN_ERR       BIT10
#define  RX_CRC_ERR       BIT11
#define  RX_NESN_ERR      BIT12
#define  RX_SN_ERR        BIT13
#define  LAST_EMPTY       BIT14
#define  RR               BIT15
extern SemaphoreHandle_t pmSemaphore ;
extern bool pm_sem_wait ;
extern bool status_connected;
extern uint8_t connect_ind_crc[];
extern uint32_t connect_ind_aa;
extern unsigned char rf_chip_id;
extern uint32_t exe_bletick_event_expect_time;
extern  uint32_t exe_bletick_wakeup_point;
#define RF_BLE_CTL                    0x0C // 'BLE Ctrol' register address
#include "exe_hal_time.h"
uint32_t hal_rf_set_crc_init(uint32_t crc_init);
bool scan_adv_flag = false;
uint32_t scan_adv_windows = 0;

 void change_to_rx_work_around(void)
 {
	
	if(!status_connected)
		return;

    SPI_Operation(0x8B); 
    hal_rf_set_freq_point(42);
    ll_pkt_rx(hal_us_to_htime(400));
	SPI_write_byte(CI231X_RF_BANK0_STATUS_EXT, 0x04);
	SPI_write_byte(CI231X_RF_BANK0_STATUS, 0x70);
	ci231x_rf_flush_rx();
	ci231x_flush_tx();
	SPI_Operation(0x8B); 

 }

 void change_to_send_adv(void)
 {
	if(!status_connected)
		return;
    ci231x_rf_ce_low();
    SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0x8A);
   // if(status_connected)
	{
	    SPI_write_byte(CI231X_RF_BANK0_STATUS_EXT, 0x04);
	    SPI_write_byte(CI231X_RF_BANK0_STATUS, 0x70);
	    ci231x_rf_flush_rx();
	    ci231x_flush_tx();
	    exe_stk_state = EXE_LINK_STATE_ADV/*0x01*/;
	    SPI_write_byte(CI231X_RF_BANK0_RF_CH, channel_index[0]); 
	    hal_rf_set_crc_init(0x00555555);
	    hal_rf_set_aa(0x8E89BED6);//0xd6be898e);
	    hal_rf_setup_buf();
        test_exe_ll_adv_tx(7);
	    exe_stk_state = EXE_LINK_STATE_CONN;
//UartPollingSenddata(CONFIG_CI_LOG_UART_PORT,0x5a);
	}
   	SPI_write_byte(CI231X_RF_BANK0_STATUS_EXT, 0x04);
	SPI_write_byte(CI231X_RF_BANK0_STATUS, 0x70);
	ci231x_rf_flush_rx();
    ci231x_flush_tx();
    SPI_Operation(0x8B); 

 }

void change_to_scan_adv(void)
{
  static uint8_t channel_idx = 0;
  if ((exe_stk_state != EXE_LINK_STATE_ADV) && (!status_connected) && scan_adv_flag)
      return;

   scan_adv_windows = exe_bletick_event_expect_time - exe_bletick_wakeup_point - EXE_1D25MS_TO_XTIME(3);

   scan_adv_flag = true;

	SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0xbb); // CONFIG寄存器配置为接收模式，使能CRC
	//SPI_write_byte(CI231X_RF_BANK0_RF_CH, channel_index[(channel_idx++)%3]); // channle map:37:2402,38:2426,39:2480
	SPI_write_byte(CI231X_RF_BANK0_RF_CH, channel_index[0]); // channle map:37:2402,38:2426,39:2480
	hal_rf_set_crc_init(0x00555555);
	hal_rf_set_aa(0x8E89BED6);//0xd6be898e);
	if(rf_chip_id > 0x24)
	{
		SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x06);	// 7:event abort,6:md dsb,5:nesn sn dsb,4:infiniterx,
													// 3~0:3-ble broacast,5-ble connection,9-ble tx test,b-ble rx test 
		SPI_Operation(0x8B); // 复位状态机。。。旧的芯片没有这个命令，
	}
	else
	{
		SPI_write_byte(CI231X_RF_BANK0_BLE_CTL, 0x0b); // 旧版本的芯片没有白名单模式
	}
	
    if(rf_chip_id > 0x24)
  	    SPI_Operation(0xD9); // CE脉冲命令，新版本芯片才有的，让芯片进入接收状态
    else	
  	    ci231x_rf_ce_high(); // 拉高CE使芯片处于接收状态，等待数据到来，就版本的芯片必须这么做
//UartPollingSenddata(CONFIG_CI_LOG_UART_PORT,0xbb);
}

void restor_from_scan_adv(void)
{
	uint8_t *p_crcinit;

//	if(!scan_adv_flag) 
//		return;

	scan_adv_flag = false;

	if(rf_chip_id > 0x24) {
		 SPI_Operation(0x8B); // 复位状态机。。。旧的芯片没有这个命令，
	}
	else
	{
     ci231x_rf_ce_low();
	}

   SPI_write_byte(CI231X_RF_BANK0_CONFIG, 0x8b);
   //从scan切换到广播或者连接态
   if (exe_stk_state == EXE_LINK_STATE_ADV) {
       SPI_write_byte(RF_BLE_CTL, 0x03);  //adv mode
   }
   else // status_connected
   {
       p_crcinit = &connect_ind_crc[0];
	   hal_rf_set_aa(connect_ind_aa);
       hal_rf_set_crc_init((p_crcinit[2]<<16) | (p_crcinit[1]<<8) | (p_crcinit[0]<<0));
       SPI_write_byte(RF_BLE_CTL, 0xed);  //connect mode
   }
}

void scan_adv_callback(void)
{
	   volatile unsigned short status_ext = 0x00;
	    unsigned char len = 0;
		uint32_t current_time;

		if(!scan_adv_flag) 
			return;

        current_time = hal_htime_get_current();

        if (hal_htime_elapsed(current_time, hal_xtime_to_htime(scan_adv_windows)))
		{
            hal_pm_wakeup();
			SPI_write_byte(CI231X_RF_BANK0_STATUS_EXT, 0x04);
			SPI_write_byte(CI231X_RF_BANK0_STATUS, 0x70);
			SPI_Operation(0x8B); 
			//UartPollingSenddata(CONFIG_CI_LOG_UART_PORT,0x88);
			return;
		}


check_status_start:
		SPI_read_buffer(CI231X_RF_BANK0_STATUS_EXT, (unsigned char *)&status_ext, 2);
	    if (status_ext & 0x0004/* wakeup */) 
		{
            hal_pm_wakeup();
			SPI_write_byte(CI231X_RF_BANK0_STATUS_EXT, 0x04);
			SPI_write_byte(CI231X_RF_BANK0_STATUS, 0x70);
			//UartPollingSenddata(CONFIG_CI_LOG_UART_PORT,0xff);
		} 
		else if (status_ext & 0x08/*RX_TIME_OUT*/) 
		{
			ci231x_clear_all_irq();
			/* In case of rx timeout on the 2nd tifs rx. */
			SPI_Operation(0x8B); // 复位状态机命令，这个命令不可少
			SPI_Operation(0xD9); // CE脉冲命令
		} 
		else if (status_ext & 0xff00/*RX erros*/) 
		{
			ci231x_clear_all_irq();
			/* In case of rx error on the 2nd tifs rx. */
			SPI_Operation(0x8B); // 复位状态机命令，这个命令不可少
			SPI_Operation(0xD9); // CE脉冲命令
		} 
		else if (status_ext & 0x40/*RX_DR*/) //接收成功
		{
			ci231x_clear_all_irq();
			memset(Rx_Payload, 0, sizeof(Rx_Payload));
			SPI_read_buffer(CI231X_RF_BANK0_RX_HEADER, &Rx_Payload[0], 2); // 新版本的芯片header从这里读出
			len = Rx_Payload[1];
			if (len > 0) 
			{
				SPI_read_buffer(CI231X_RF_BANK0_RX_ADVA_ADDR, &Rx_Payload[2], 6); // 新版本的芯片Adva从这里读出
				if (len > 6) 
				{
					SPI_read_buffer(CI231X_RF_RX_PAYLOAD, &Rx_Payload[8], len-6); // 新版本的芯片AdvData从这里读出
				} 
			}
			ci231x_rf_flush_rx();
			SPI_Operation(0x8B); // 复位状态机命令，这个命令不可少
			SPI_Operation(0xD9); // CE脉冲命令

			if ((len == 0x25)&&(Rx_Payload[2] == 0x46)&&(Rx_Payload[7] == 0xAF)) //遥控器
			{
				//mprintf("%02x ",Rx_Payload[1]);
			}

			if ((Rx_Payload[8]== 0x11)&&(Rx_Payload[9] == 0x07))  //启英遥控器
			{
				ci_rf_recv_group_data(&Rx_Payload[10]);
			}
			if ((Rx_Payload[14]== 0x11)&&(Rx_Payload[15] == 0x07))  //Apple手机
			{
				ci_rf_recv_group_data(&Rx_Payload[16]);
			}
		}
		else {
			ci231x_clear_all_irq();
			/* In case of rx error on the 2nd tifs rx. */
			SPI_Operation(0x8B); // 复位状态机命令，这个命令不可少
			SPI_Operation(0xD9); // CE脉冲命令			
		}
	if(hal_rf_irq_is_asserted())
		goto check_status_start;
}