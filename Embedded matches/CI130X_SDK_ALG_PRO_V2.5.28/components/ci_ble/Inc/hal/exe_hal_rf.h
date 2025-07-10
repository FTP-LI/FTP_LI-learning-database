/*============================================================================*/
/* @file exe_hal_rf.h
 * @brief EXE HAL Radio header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_RF_H__
#define __EXE_HAL_RF_H__

#include <stdint.h>
#include <stdbool.h>

///transceiver/RF version is 2=CI231XA2,8inch 3=OM6220,12inch
#define HAL_BLE_HW_RF_VER               3

///BLE tick based on CI231X's timer.
#define HAL_BLE_HW_TIMER                1

///transceiver/RF supports hw ble flow control
#define HAL_BLE_HW_FC                   0//1
///transceiver/RF supports hw ble more data
#define HAL_BLE_HW_MD                   0//1

/***************************  �����շ���������غ���  *************************/

/**
 * @brief Initilize RF module.
 */
void hal_rf_init(void);

/**
 * @brief Uninitilize RF module.
 */
void hal_rf_cleanup(void);

/**
 * @note Obsoloted.
 */
void hal_rf_setup_buf(void);

/**
 * @brief Enable rx timeout, i.e. infiniterx=0 for CI231X.
 */
void hal_rf_enable_rx_timeout(void);

/**
 * @brief Abort connect/adv event, i.e. event_abort=1 for CI231X.
 */
void hal_rf_abort_ble_evt(void);

/**
 * @brief Set CRC Init for CRC generator in hw, and reverse it for CRC generator in sw.
 *
 * @param [in] crc_init - 0x00555555 for adv;
 *                        CONNECT_IND.CRCInit for conn.
 * @return the crc init value in reverse order.
 */
uint32_t hal_rf_set_crc_init(uint32_t crc_init);

/**
 * @brief Set Access Address to sync the received bit stream.
 *
 * @param [in] access_address - 0x8E89BED6 for adv;
 *                              CONNECT_IND.AA for conn.
 */
void hal_rf_set_aa(uint32_t access_adddress);

/**
 * @brief Set RF frequency point for transmit or receive.
 * @note ���������ŵ���RF Channel Number�������߼��ŵ�����RF Channel Index.
 *
 * @param [in] off_2mhz - The offset to 2400MHz, step in 2MHz,
 *                        ie. RF Channel Number.
 */
void hal_rf_set_freq_point(int off_2mhz);

/**
 * @brief Set transmit power level.
 *
 * @param [in] dBm - The tx power level in dBm.
 */
void hal_rf_set_tx_pwr_lvl(int dBm);

/**
 * @brief Adjust frequency offset for accurate RF PLL.
 *
 * @param [in] freq_off_cw - The control word of frequency offset.
 */
void hal_rf_set_freq_off(uint8_t freq_off_cw);


/**
 * @brief Set whiten index before transmit or receive.
 *
 * @param [in] chn_inx - The RF Channel Index.
 */
void hal_rf_set_whiten_idx(uint8_t chn_idx);

/**
 * @brief Fill a BLE packet, including header and payload, to RF module.
 *
 * @param [in] p_pkt - The pointer to BLE packet.
 */
void hal_rf_fill_pdu(uint8_t *p_pkt);

/**
 * @brief Fill an invalid BLE packet to tx fifo, so master will receive crc error.
 */
void hal_rf_fill_invalid_pdu(void);


/**
 * @brief Prepare the initial tx for adv tx in classic timing.
 */
void hal_rf_init_tx(void);

/**
 * @brief Clear the interrupt status of tx.
 */
void hal_rf_clear_tx(void);

/**
 * @brief Enable the initial tx for adv tx in classic timing.
 * @note ����������adv�¼��еĳ�ʼ���Ͱ�ʱ���䱾�����Ǿ�������ʱ��
 */
void hal_rf_enable_tx(void);

/**
 * @brief Enable tx for rx2tx in LE timing.
 * @note ����������adv��conn�¼��е���ת���ķ��Ͱ�ʱ��Ҫ��ѭLE��TIFSʱ��
 */
void hal_rf_enable_tx_tifs(void);

/**
 * @brief Check whether adv tx is done.
 *
 * @return true for transmitted, false for not.
 */
bool hal_rf_is_tx_done(void);

/**
 * @brief Enable rx in long rx always, for adv rx or conn rx.
 */
void hal_rf_enable_rx(void);

/**
 * @brief Disable rx after received or receive error.
 */
void hal_rf_disable_rx(void);

/**
 * @brief Check whether receive the header of a ble packet.
 * @note It also implies synced.
 *
 * @return true for received, false for not.
 */
bool hal_rf_is_rx_hdr(void);

/**
 * @brief Check whether receive timeout.
 * @note It also implies sync lost during rx window.
 *
 * @return true for timeout, false for not.
 */
bool hal_rf_is_rx_timeout(void);

/**
 * @brief Poll wait for received a ble packet or receive error.
 */
void hal_rf_wait_rx_done(void);

/**
 * @note Obsoloted.
 */
void hal_rf_set_rx_size(int size);
void hal_rf_rx_data(int bytes);

/**
 * @brief Read a BLE packet's payload, from RF module.
 * @param [in] pkt_len - The payload length of BLE packet.
 */
void hal_rf_rx_all(int pkt_len);

/**
 * @brief Clear the rx interrupts, including sync, rx done, rx timeout etc.
 */
void hal_rf_clear_rx_interrupt(void);

/**
 * @brief Clear the rtc timer interrupt.
 */
void hal_rf_clear_rtc_timer_interrupt(void);

/**
 * @brief Check whether receive CRC error occurs, including type or len errors.
 *
 * @return true for receive CRC error,
 *         false for receive a good packet.
 */
bool hal_rf_is_crc_err(void);

/**
 * @brief Check whether (connect) event is done, including rx len error.
 *
 * @return true for event done.
 */
bool hal_rf_is_evt_done(void);

#if HAL_BLE_HW_FC
/**
 * @brief Check whether receive SN error occurs.
 *
 * @return true for received a resent data packet;
 *         false for receive a new data packet.
 */
bool hal_rf_is_sn_err(void);

/**
 * @brief Check whether receive NESN error occurs.
 *
 * @return true for received NAK packet;
 *         false for receive ACK packet.
 */
bool hal_rf_is_nesn_err(void);
#endif

/**
 * @brief Stop all tx and rx state machine of RF module.
 * @note ������������ֹ��abort����ǰ��adv��conn�¼������緢��ADV_NONCONN_IND�󣬻����յ�CRC���İ���
 *       ��RFģ�飨CI231XA2����֧�ָù���ʱ��exBLE��Ҫ��������߼�������ʵ�֡�
 */
void hal_rf_stop_tx_rx(void);

/**
 * @note CI231X RF module specific.
 */
 
/**
 * @brief wakeup th RF module from sleep mode.
 * @param [in] mode - RF_DEEP_SLEEP for deep sleep wake up, the xtal is stop in sleep state.
 *                  - RF_LIGHT_SLEEP for standby sleep wake up, the xtal is active in sleep state.
 */
void hal_rf_wakeup(uint8_t mode);

/**
 * @brief th RF module enter sleep mode.
 * @param [in] mode - RF_DEEP_SLEEP for deep sleep, the xtal is stop.
 *                  - RF_LIGHT_SLEEP for standby sleep, the xtal is active.
 */
void hal_rf_sleep(uint8_t mode);

/**
 * @brief Set the absolute RTC time as alarm.
 *
 * @param [in] xtime - alarm time.
 * @return none
 */
void hal_xtime_set_alarm(uint32_t xtime);

/**
 * @brief set the delta tick value to rtc timer.
 * @note  OM6220 only.
 *
 * @param [in] delta - the delta tick value will be set.
 */
void hal_xtime_set_delta(uint32_t delta);

/**
 * @brief Read the current RTC time from transceiver rtc timer.
 * 
 * @return the current transceiver time.
 */
uint32_t hal_xtime_get_current(void);

/**
 * @brief Read the RTC time on the latest sync from transceiver.
 * 
 * @return the transceiver time on sync.
 */
uint32_t hal_xtime_get_onsync(void);

/**
 * @brief Tx long adv packet: rf transmit long adv packet while filling tx fifo.
 * @param [in] p_pkt - The pointer to long adv packet buffer,
 *                     including 2-byte header, and upto 255-byte payload data.
 */
RAM_CODE_SECTION void hal_rf_tx_adv_pkt_long(uint8_t *p_pkt);

/**
 * @brief Scan to recevie short adv packet at lower CPU & SPI frequency.
 * @param [in] p_pkt - The pointer to adv packet buffer,
 *                     including 2-byte header, and upto 37-byte payload data.
 * @return true for scanned; false for nothing.
 */
RAM_CODE_SECTION bool hal_rf_scan_adv_pkt_lowhz(uint8_t *p_pkt);

/**
 * @brief Scan to recevie adv packet and send SCAN_REQ or CONNECT_IND.
 * @note CI231XA2 or OM6220 can run this API.
 *
 * @param [out] p_pkt_rx - The pointer to rx adv packet buffer,
 *                         including 2-byte header, and upto 31(6220) or 37 bytes payload data.
 * @param [in]  p_pkt_tx - The pointer to tx adv packet buffer,
 *                         including 2-byte header, always SCAN_REQ in 12-byte or CONNECT_IND in 34-byte.
 * @return true for scanned; false for nothing.
 */
bool hal_rf_scan_adv_pkt_tifs(uint8_t *p_pkt_rx, uint8_t *p_pkt_tx);

/**
 * @brief Scan to recevie adv packet in whitelist and send SCAN_REQ or CONNECT_IND.
 * @note CI231X cannot run this API, OM6220 can.
 * @note The bluetooth device address in whitelist doesn't contain address type.
 *
 * @param [out] p_pkt_rx - The pointer to rx adv packet buffer,
 *                         including 2-byte header, and upto 37-byte payload data.
 * @param [in]  p_pkt_tx - The pointer to tx adv packet buffer,
 *                         including 2-byte header, always SCAN_REQ in 12-byte or CONNECT_IND in 34-byte.
 * @param [in] p_whitelist - The pointer to whitelist, 6-byte AdvA per entry, 2-entry.
 *                           The scanner receives the adv packets which AdvA is matched within whitelist.
 *                           Disable whitelist if NULL.
 * @return true for scanned; false for nothing.
 */
bool hal_rf_scan_adv_pkt_tifs_wl(uint8_t *p_pkt_rx, uint8_t *p_pkt_tx, uint8_t *p_whitelist);

#endif /* #ifndef __EXE_HAL_RF_H__ */

