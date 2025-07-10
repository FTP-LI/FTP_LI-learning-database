#ifndef __BSP_rf_H__
#define __BSP_rf_H__

#include <stdint.h>

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

//----------------SPIRSTAT-----------------
#define RFSPI_RX_FIFO_FULL           BIT3           /*RX FIFO full.*/ 
#define RFSPI_RX_DATA_RDY            BIT2           /*Data available in RX FIFO*/
#define RFSPI_TX_FIFO_EMPTY          BIT1           /*TX FIFO empty.*/
#define RFSPI_TX_FIFO_RDY            BIT0           /*Location available in TX FIFO*/

#define RF_STATUS_RX_DR				0x40
#define RF_STATUS_TX_DS				0x20
#define RF_STATUS_MAX_RT			0x10
#define RF_STATUS_TX_FULL			0x01

/*register bit Mask define*/
/*R 0x17:*/
#define RF_FIFO_STA_TX_REUSE		0x40
#define RF_FIFO_STA_TX_FULL			0x20
#define RF_FIFO_STA_TX_EMPTY		0x10
#define RF_FIFO_STA_RX_FULL			0x02
#define RF_FIFO_STA_RX_EMPTY		0x01

//config register bit define
#define PRIM_RX        BIT0
#define PWR_UP         BIT1
#define CE_REG         BIT2
#define EN_CRC         BIT3
#define MASK_MAX_RT    BIT4
#define MASK_TX_DS   	 BIT5
#define MASK_RX_DR     BIT6
#define GUARD_EN       BIT7

//pmu_ctl register bit define
#define RF_PWRDN_MASK      0x03   
#define RF_PWRDN_WORK      0x00 
#define RF_PWRDN_SDAND_BY  0x02 
#define RF_PWRDN_SLEEP     0x01 
#define DLDO_ENB_REG       BIT3
#define DLDO_ENB_MN        BIT4
#define RF_DEEP_SLEEP      0x01
#define RF_LIGHT_SLEEP     0x02

//DYNPD register bit define
#define SPI4_EN            BIT3
#define XN297_EN           BIT4
#define BYPASS_IO          BIT5
#define REG_OUTPUT_EN      BIT6
#define REG_OUTPUT         BIT7

//FEATURE register bit define
#define EN_DYN_ACK         BIT0
#define EN_ACK_PAY         BIT1
#define EN_DPL             BIT2
#define VCO_AMP_TX_MUX     BIT3
#define BP_GAU             BIT4
#define SOFT_RST           BIT5
#define BLE_EN             BIT6
#define LONG_PACKET_EN     BIT7

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

//BLE_CTL register bit define
#define  FORMAT           0x0f  //0x09 dtm_tx 0x0b dtm_rx
#define  INFINITERX       BIT4
#define  NESN_DSB         BIT5
#define  SN_DSB           BIT6
#define  EVENT_ABORT      BIT7

//CONFIG_EXT register bit define
#define  BLE_TIM_ON        BIT0
#define  MASK_TIM_INT      BIT1
#define  MASK_TX_EMPTY     BIT2
#define  MASK_RX_AFULL     BIT3
#define  MASK_RX_TIMOUT    BIT4
#define  FIFO_LEN_CTL      BIT5
#define  CE_BEFORE_FIFO    BIT6

#define RF_BANK_BIT    BIT7      /*bank ָʾλ*/
#define RF_BANK1       BIT7
#define RF_BANK0       0x00  

//-------------RF command---------------------
#define RF_R_REGISTER                 0x00 // Define read command to register
#define RF_W_REGISTER                 0x20 // Define write command to register
#define RF_R_RX_PAYLOAD               0x61 // Define RX payload register address
#define RF_W_TX_PAYLOAD               0xA0 // Define TX payload register address
#define RF_FLUSH_TX                   0xE1 // Define flush TX register command
#define RF_FLUSH_RX                   0xE2 // Define flush RX register command
#define RF_REUSE_TX_PL                0xE3 // Define reuse TX payload register command
#define RF_ACTIVATE                   0x50 // Define ACTIVATE features register command
#define RF_R_RX_PL_WID                0x60 // Define read RX payload width register command
#define RF_W_ACK_PAYLOAD              0xA8 // Define write ACK payload register command
#define RF_W_TX_PAYLOAD_NOACK         0xB0 // Define disable TX ACK for one time register command
#define RF_NOP                        0xFF // Define No Operation, might be used to read status register
#define RF_TX_REUSE                   0xE3 // Define Reuse last transmitted payload command
#define RF_READ_STATUS                0xFF
#define RF_ACTIVATE_DATA              0x53
#define RF_CE_HIGH                    0xD5
#define RF_CE_LOW                     0xD6
#define RF_TIM_LOCK                   0x90
#define RF_RSTN                       0x8B // Reset PHY and MAC, not reset registers
#define RF_CE_PULSE                   0xD9



/**************************************************/               
#define RF_BANK0_CONFIG                     0x00 // 'Config' register address
#define RF_BANK0_EN_AA                      0x01 // 'Enable Auto Acknowledgment' register address
#define RF_BANK0_EN_RXADDR                  0x02 // 'Enabled RX addresses' register address
#define RF_BANK0_PMU_CTL                    0x03 // 'Setup address width' register address
#define RF_BANK0_SETUP_RETR                 0x04 // 'Setup Auto. Retrans' register address
#define RF_BANK0_RF_CH                      0x05 // 'rf channel' register address
#define RF_BANK0_RF_SETUP                   0x06 // 'rf setup' register address
#define RF_BANK0_STATUS                     0x07 // 'Status' register address                  
#define RF_BANK0_RX_ADDR_P0                 0x0A // 'RX address pipe0' register address
#define RF_BANK0_ACCESS_ADDR                0x0A
#define RF_BANK0_RX_ADDR_P1                 0x0B // 'RX address pipe1' register address
#define RF_BANK0_BLE_TIM_CNT                0x0B // BLE timer counter @32kHz in 32-bit
#define RF_BANK0_RX_ADDR_P2                 0x0C // 'RX address pipe2' register address
#define RF_BANK0_BLE_CTL                    0x0C // 'BLE Ctrol' register address
#define RF_BANK0_CLKN_SYNC                  0x0D // Latch 32k timer counter when sync detect
#define RF_BANK0_TX_ADDR                    0x10 // 'TX address' register address
#define RF_BANK0_TX_ADVA_ADDR               0x10
#define RF_BANK0_RX_PW_P0                   0x11 // 'RX payload width, pipe0' register address
#define RF_BANK0_TX_HEADER                  0x11 // 'BLE TX Header' register address
#define RF_BANK0_RX_PW_P1                   0x12 // 'RX payload width, pipe1' register address
#define RF_BANK0_CRC_INIT                   0x12
#define RF_BANK0_RX_PW_P2                   0x13 // 'RX payload width, pipe2' register address
#define RF_BANK0_BLE_SETUP_DELAY            0x13
#define RF_BANK0_RX_HEADER                  0x14
#define RF_BANK0_RX_ADVA_ADDR               0x15
#define RF_BANK0_STATUS_EXT                 0x16
#define RF_BANK0_FIFO_STATUS                0x17 // 'FIFO Status Register' register address
#define RF_BANK0_CONFIG_EXT                 0x18
#define RF_BANK0_RTC_TIMER_MODE             0x19 // b'00-absolute b'10-delta b'x1-immediately
#define RF_BANK0_WHITE_LIST_RXADDR_1        0x1A
#define RF_BANK0_WHITE_LIST_RXADDR_2        0x1B
#define RF_BANK0_DYNPD                      0x1C // 'Enable dynamic payload length' register address
#define RF_BANK0_FEATURE                    0x1D // 'Feature' register address
#define RF_BANK0_SETUP_VALUE                0x1E
#define RF_BANK0_PRE_GURD                   0x1F


#define RF_BANK1_CHIP_ID              0x00
#define RF_BANK1_PLL_CTL0             0x01
#define RF_BANK1_PLL_CTL1             0x02
#define RF_BANK1_CAL_CTL              0x03
//#define RF_BANK1_RESERVED1            0x06
#define RF_BANK1_STATUS               0x07
#define RF_BANK1_STATE                0x08
#define RF_BANK1_CHAN                 0x09
#define RF_BANK1_FDEV                 0x0C
#define RF_BANK1_DAC_RANGE            0x0D
#define RF_BANK1_CTUNING              0x0F
#define RF_BANK1_FTUNING              0x10
#define RF_BANK1_RX_CTRL              0x11
#define RF_BANK1_FAGC_CTRL_1          0x13
//#define RF_BANK1_RESERVED3            0x19
#define RF_BANK1_DOC_DACI             0x1A
#define RF_BANK1_DOC_DACQ             0x1B
#define RF_BANK1_AGC_CTRL             0x1C
#define RF_BANK1_AGC_GAIN             0x1D
#define RF_BANK1_RF_IVGEN             0x1E
#define RF_BANK1_TEST_PKDET           0x1F

#define RF_FIFO_MAX_PACK_SIZE		0x20

typedef enum {
	RF_Bank0 = 0,
	RF_Bank1
}RF_Bank_TypeDef;

/**
 * @brief write one byte command to ci231x.
 *
 * @param [in] none
 * @return none
 */
void rf_operation(unsigned char opt);

/**
 * @brief read ci231x one byte register.
 *
 * @param [in] addr - register address;
 * 
 * @return the register vale
 */
unsigned char rf_read_byte(unsigned char addr);

/**
 * @brief clear ci231x all interrupt [excpet rtc, clear rtc interrupt by write STATUS_EXT[bit2]].
 *
 */
void rf_clear_all_irq(void);

/**
 * @brief reuse the tx fifo.
 */
void rf_tx_reuse(void);
    
/**
 * @brief flush ci231x tx fifo.
 */
void rf_flush_tx(void);

/**
 * @brief flush ci231x rx fifo.
 */
void rf_flush_rx(void);

/**
 * @brief pull up the ce signal by command.
 */
void rf_ce_high(void);

/**
 * @brief pull down the ce signal by command.
 */
void rf_ce_low(void);

/**
 * @brief switch ci231x register bank.
 * @param [in] bank - the bank which want to switch
 */
void rf_bank_switch(RF_Bank_TypeDef bank);

/**
 * @brief read muti byte data from ci231x.
 *
 * @param [in] addr - register address or command(eg. RF_R_RX_PAYLOAD);
 *        [in] buf - the data point . 
 *        [in] len - the data length will be read. 
 * @return none
 */
void rf_read_buffer(unsigned char addr, unsigned char* buf, unsigned char len);

/**
 * @brief write one byte data to ci231x register.
 *
 * @param [in] addr - register address.
 *        [in] buf - the data will be write . 
 * @return none
 */
void rf_write_byte(unsigned char addr,unsigned char D);

/**
 * @brief write muti byte data to ci231x.
 *
 * @param [in] addr - register address or command(eg. RF_W_TX_PAYLOAD);
 *        [in] buf - the data point which will be write. 
 *        [in] len - the data length. 
 * @return none
 */
void rf_wr_buffer(unsigned char addr, const unsigned char* buf,unsigned char len);

/**
 * @brief dump all ci231x register.
 */
void rf_Dump_RF_Register(void);

/**
 * @brief ce pulse.
 * @param [in] us - the width of the pulse. 
 */
void hal_ce_high_pulse(uint8_t us);

/**
 * @brief clear the rx interrupts, including sync, rx done, rx timeout etc.
 */
void hal_rf_clear_rx_interrupt(void);

/**
 * @brief clear the rtc timer interrupt.
 */
void hal_rf_clear_rtc_timer_interrupt(void);

/**
 * @brief ci231x radio calibration.
 * @param [in] daterate - 0 for 1Mbps
 *                      - 1 for 2Mbps
 */
void ci231x_radio_calibration(uint8_t datarate);

/**
 * @brief ci231x radio restore the calibration value.
 *        Note: use for switch 1Mbps & 2Mbps
 * @param [in] daterate - 0 for 1Mbps
 *                      - 1 for 2Mbps
 */
void rf_restore_cali_value(uint8_t datarate);

/**
 * @brief lock the rtc current value.
 */
void hal_tim_tick_lock(void);
void hal_sys_tick();
#endif
