
#ifndef __EXE_ADV_H__
#define __EXE_ADV_H__
#include "stdbool.h"
//-----------------RF command---------------------
#define CI231X_RF_RD_REG				0x00 // Define read command to register
#define CI231X_RF_WR_REG				0x20 // Define write command to register
#define CI231X_RF_RX_PAYLOAD			0x61 // Define RX payload register address
#define CI231X_RF_TX_PAYLOAD			0xA0 // Define TX payload register address
#define CI231X_RF_FLUSH_TX				0xE1 // Define flush TX register command
#define CI231X_RF_FLUSH_RX		        0xE2 // Define flush RX register command
#define CI231X_RF_REUSE_TX_PL			0xE3 // Define reuse TX payload register command
#define CI231X_RF_ACTIVATE				0x50 // Define ACTIVATE features register command   
#define CI231X_RF_RX_PL_WID			    0x60 // Define read RX payload width register command
#define CI231X_RF_ACK_PAYLOAD		    0xA8 // Define write ACK payload register command
#define CI231X_RF_TX_PAYLOAD_NOACK	    0xB0 // Define disable TX ACK for one time register command
#define CI231X_RF_NOP					0xFF // Define No Operation, might be used to read status register
#define CI231X_RF_READ_STATUS			0xFF


#define CI231X_RF_ACTIVATE_DATA		    0x53

#define CI231X_RF_CMD_CE_HIGH			0xD5
#define CI231X_RF_CMD_CE_LOW			0xD6

#define RF_TIM_LOCK							0x90

/// Bank0 Register Addr
/**************************************************/               
#define CI231X_RF_BANK0_CONFIG			0x00 // Config register address
#define CI231X_RF_BANK0_EN_AA			0x01 // Enable Auto Acknowledgment register address
#define CI231X_RF_BANK0_EN_RXADDR		0x02 // Enabled RX addresses register address
#define CI231X_RF_BANK0_PMU_CTL		    0x03 // Setup address width register address
#define CI231X_RF_BANK0_SETUP_RETR		0x04 // Setup Auto. Retrans register address
#define CI231X_RF_BANK0_RF_CH			0x05 //  channel register address
#define CI231X_RF_BANK0_RF_SETUP		0x06 // setup register address
#define CI231X_RF_BANK0_STATUS			0x07 // Status register address                  
#define CI231X_RF_BANK0_RX_ADDR_P0		0x0A // RX address pipe0 register address
#define CI231X_RF_BANK0_ACCESS_ADDR	    0x0A
#define CI231X_RF_BANK0_RX_ADDR_P1		0x0B // RX address pipe1 register address
#define CI231X_RF_BANK0_BLE_TIM_CNT	    0x0B 
#define CI231X_RF_BANK0_RX_ADDR_P2		0x0C // RX address pipe2 register address
#define CI231X_RF_BANK0_BLE_CTL		    0x0C // BLE Ctrol register address
#define CI231X_RF_BANK0_TX_ADDR		    0x10 // TX address register address
#define CI231X_RF_BANK0_TX_ADVA_ADDR	0x10
#define CI231X_RF_BANK0_RX_PW_P0		0x11 // RX payload width, pipe0 register address
#define CI231X_RF_BANK0_TX_HEADER		0x11 // BLE TX Header' register address
#define CI231X_RF_BANK0_RX_PW_P1		0x12 // RX payload width, pipe1 register address
#define CI231X_RF_BANK0_CRC_INIT		0x12
#define CI231X_RF_BANK0_RX_PW_P2		0x13 // RX payload width, pipe2 register address
#define CI231X_RF_BANK0_BLE_SETUP_DELAY 0x13
#define CI231X_RF_BANK0_RX_HEADER		0x14
#define CI231X_RF_BANK0_RX_ADVA_ADDR	0x15
#define CI231X_RF_BANK0_STATUS_EXT		0x16
#define CI231X_RF_BANK0_FIFO_STATUS	    0x17 // FIFO Status Register register address
#define CI231X_RF_BANK0_CONFIG_EXT		0x18
#define CI231X_RF_BANK0_DYNPD			0x1C // Enable dynamic payload length register address
#define CI231X_RF_BANK0_FEATURE		    0x1D // Feature register address
#define CI231X_RF_BANK0_SETUP_VALUE	    0x1E
#define CI231X_RF_BANK0_PRE_GURD		0x1F

#define CI231X_RF_BANK0				    0x00
#define CI231X_RF_BANK1				    0x80

/// Bank1 register
#define CI231X_RF_BANK1_LINE		    0x00
#define CI231X_RF_BANK1_PLL_CTL0		0x01
#define CI231X_RF_BANK1_PLL_CTL1		0x02
#define CI231X_RF_BANK1_CAL_CTL		    0x03
#define CI231X_RF_BANK1_STATUS			0x07
#define CI231X_RF_BANK1_STATE			0x08
#define CI231X_RF_BANK1_CHAN			0x09
#define CI231X_RF_BANK1_FDEV			0x0C
#define CI231X_RF_BANK1_DAC_RANGE		0x0D
#define CI231X_RF_BANK1_CTUNING		    0x0F
#define CI231X_RF_BANK1_FTUNING		    0x10
#define CI231X_RF_BANK1_RX_CTRL		    0x11
#define CI231X_RF_BANK1_FAGC_CTRL_1	    0x13
#define CI231X_RF_BANK1_DOC_DACI		0x1A
#define CI231X_RF_BANK1_DOC_DACQ		0x1B
#define CI231X_RF_BANK1_AGC_CTRL		0x1C
#define CI231X_RF_BANK1_AGC_GAIN		0x1D
#define CI231X_RF_BANK1_RF_IVGEN		0x1E
#define CI231X_RF_BANK1_TEST_PKDET		0x1F

/* register bit Mask define */
/* R 0x17: */
#define CI231X_RF_FIFO_STA_TX_REUSE	    0x40
#define CI231X_RF_FIFO_STA_TX_FULL		0x20
#define CI231X_RF_FIFO_STA_TX_EMPTY	    0x10
#define CI231X_RF_FIFO_STA_RX_FULL		0x02
#define CI231X_RF_FIFO_STA_RX_EMPTY	    0x01
/* R 0x07 */
#define CI231X_RF_STATUS_RX_DR			0x40
#define CI231X_RF_STATUS_TX_DS			0x20
#define CI231X_RF_STATUS_MAX_RT		    0x10
#define CI231X_RF_STATUS_TX_FULL		0x01

#define CI231X_RF_FIFO_MAX_PACK_SIZE	0x20

typedef enum {
	Rf_PTX_Mode = 0,
	Rf_PRX_Mode,
	Rf_Carrier_Mode
}RF_ModeTypeDef;

typedef enum {
	Rf_Bank0 = 0,
	Rf_Bank1 = 0x80
}RF_Bank_TypeDef;

/* rf Power */
typedef enum {

	pwr_level_0  = 0x00,
	pwr_level_1  = 0x01,
	pwr_level_2  = 0x02,
	pwr_level_3  = 0x03,
	pwr_level_4  = 0x04,
	pwr_level_5  = 0x05,
	pwr_level_6  = 0x06,
	pwr_level_7  = 0x07,
	pwr_level_8  = 0x40,
	pwr_level_9  = 0x41,
	pwr_level_10 = 0x42,
	pwr_level_11 = 0x43,
	pwr_level_12 = 0x44,
	pwr_level_13 = 0x45,
	pwr_level_14 = 0x46,
	pwr_level_15 = 0x47
}rf_tx_power_t;


void ci231x_rf_bank_switch(RF_Bank_TypeDef bank);
void ci231x_rf_ce_high(void);
void ci231x_rf_ce_low(void);
void ci231x_rf_ce_pulse(void);
unsigned char ci231x_rf_read_status(void);
void ci231x_rf_change_addr_reg(unsigned char* AddrBuf,unsigned char len);
void ci231x_adv_change_pwr(rf_tx_power_t Pwr_lev);
void ci231x_change_ch(unsigned char ch_index);
void ci231x_change_xtalcc(unsigned char xtal_cc);
void ci231x_enable_scramble(unsigned char en_flag);
unsigned char ci231x_get_chip_id(void);				 
void ci231x_mode_switch(RF_ModeTypeDef mod);
void ci231x_clear_all_irq(void);
void ci231x_reuse_tx_pl(void);
unsigned char ci231x_adv_receive_pack(unsigned char* buf);
void ci231x_write_ack_payload(unsigned char PipeNum, unsigned char *pBuf, unsigned char bytes);
void ci231x_enter_sleep(void);
void ci231x_leave_sleep(void);
void ci231x_soft_rst(void);

void ble_adv_scan(void);
void get_unique_id(void);
void report_group(void);
bool ci_rf_recv_group_data(unsigned char* buf);
bool covent_rfdata_to_bledata(unsigned char* buf);
void user_dev_adv_init(unsigned char dev_type_id, unsigned char dev_number_id, unsigned char config_type);
#endif
