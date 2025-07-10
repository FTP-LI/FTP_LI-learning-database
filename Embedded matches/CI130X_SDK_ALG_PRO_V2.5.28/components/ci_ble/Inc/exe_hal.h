/*============================================================================*/
/* @file exe_hal.h
 * @brief EXE Hardware Abstract Layer header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_H__
#define __EXE_HAL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "exe_hal_tc.h"
#include "exe_hal_cpu.h"
#include "exe_hal_time.h"
#include "exe_hal_os.h"
#include "exe_hal_cfg.h"
#include "exe_hal_rf.h"
#include "exe_hal_pm.h"

/**
 * @brief The LED Id enumeration.
 */
typedef enum {
    EXE_LED_ID_CONN  = 0x80,
    EXE_LED_ID_DANCE,
    EXE_LED_ID_0     = 0,
    EXE_LED_ID_1,
    EXE_LED_ID_2,
} exe_led_id_t;

/**
 * @brief The LED status enumeration.
 */
typedef enum {
    EXE_LED_ON  = 0,
    EXE_LED_OFF,
    EXE_LED_TOGGLE,

    EXE_LED_LEVEL_LOW      = 0,
    EXE_LED_LEVEL_HIGH     = 1,

    EXE_LED_PULSE_HIGH_1US    = 0x11,
    EXE_LED_PULSE_LOW_1US     = 0x21,
    EXE_LED_PULSE_TOGGLE_1US  = 0x31,
} exe_led_status_t;

/**
 * @brief The Button function bitmap, range [0,15]
 * @ref   The return value of bsp_button_get_status(), the bitmap of buttons.
 */
#define EXE_BTN_PRESSED_LEFT            (1u << 0)
#define EXE_BTN_PRESSED_RIGHT           (1u << 1)
#define EXE_BTN_PRESSED_MIDDLE          (1u << 2)
#define EXE_BTN_PRESSED_PAIR            (1u << 15)

/**
 * @brief Statistics buffer for realtime trace.
 */
#if defined(CONFIG_DEBUG_STATS)
#define HAL_STATS_ADD(event_data)       hal_stats_add(event_data)
#else
#define HAL_STATS_ADD(event_data)
#endif

typedef struct {
  ///register address: offset for mmap regs, address for analog regs, *256 for sleep.
  uint16_t off;
  ///register value or us for sleep.
  uint8_t  dat;
  ///3-mmap; 8-analog; 7-sleep
  uint8_t  cmd  :6;
  ///0 means the end of reg table.
  uint8_t  flag :2;
} hal_reg_tbl_t;

/**
 * @brief The direction type of digital GPIO.
 */
#define GPIO_DIR_OUTPUT  0
#define GPIO_DIR_INPUT   1

/**
 * @brief The operation type of NVM (Flash/OTP).
 */
#define PORT_NVM_OP_READ          0x03
#define PORT_NVM_OP_PROGRAM       0x02
#define PORT_NVM_OP_ERASE         0x20


/******************************  ����IO��غ���  ******************************/

/**
 * @brief Digital GPIO module initialization for LEDs and buttons etc.
 */
void hal_gpio_init(void);

/**
 * @brief Get status of the pressed buttons on board, for shutter.
 *        ��ȡ���ĸ˰���״̬��
 *
 * @return the bitmap of buttons, @ref EXE_BTN_xxx
 *         1 for pressed, 0 for released.
 * @note The bitmap is consistent with the HID Usages of Consume Control.
 */
uint16_t bsp_button_get_status(void);

/**
 * @brief Scan wheel for mouse in polling mode.
 * @note  It is also called in exBLE internal at necessary.
 *
 * @param [in] bletick_duration - scan duration at 32khz tick.
 * @return the remain time of bletick_duration.
 */
uint32_t bsp_wheel_scan(uint32_t bletick_duration);

/**
 * @brief Get wheel data for mouse.
 * @note  It is called by app_mouse_scan() every ~8ms.
 *
 * @return the wheel data: +clockwise, -counterclockwise
 * @note   The wheel data is consistent with the HID Usages of Consume Control.
 */
int8_t bsp_wheel_get_data(void);

/**
 * @brief Set the LED on board to specific status.
 * @note If this API is implemented, don't define HAL_PIN_LED_XXX above.
 *
 * @param [in] id - LED id.
 * @param [in] st - LED status.
 */
void bsp_led_set(exe_led_id_t id, exe_led_status_t st);

/**
 * @brief Get status of CI231X transceiver interrupt request line.
 *        ��ȡCI231X IRQ״̬��
 *
 * @return true for asserted on low level,
 *         false for deasserted on high level.
 */
bool hal_rf_irq_is_asserted(void);


/**************************  ����diagnostic��غ���  **************************/

/**
 * @brief Diagnostic indication for whether TIFS violation
 *        between the end of SCAN_REQ and the start of SCAN_RSP.
 */
void hal_diag_adv_tx_scan_rsp(void);

/**
 * @brief Diagnostic indication for sw host time timeout or 
 *        hw TX_DS irq lost during advertising.
 * @note  sw bug is a critical indication which means bug in HAL time APIs;
 *        hw bug will occur occasionally, just omit it.
 */
void hal_diag_adv_tx_timeout(void);

/**
 * @brief Diagnostic indication for sw host time timeout or 
 *        hw RX_DR irq lost during advertising or slave receiving.
 */
void hal_diag_event_rx_timeout(void);

/**
 * @brief Diagnostic indication for received CRC error packet during advertising or slave receiving.
 */
void hal_diag_event_rx_crcerr(void);

/**
 * @brief Diagnostic indication for received non-empty packet during slave connection.
 */
void hal_diag_conn_rx_nonempty(void);

/**
 * @brief Diagnostic indication for transmit non-empty packet during slave connection.
 */
void hal_diag_conn_tx_nonempty(void);

/**
 * @brief Diagnostic indication for skip rx once by scheduler.
 */
void hal_diag_sch_skip_rx(void);

/**
 * @brief Diagnostic indication for skip sleep once by scheduler.
 */
void hal_diag_sch_skip_sleep(void);

/**
 * @brief Diagnostic indication for busy wait for wakeup by scheduler.
 */
void hal_diag_sch_wait_wakeup(void);

/**
 * @brief Diagnostic indication for user interrupt which aborts latency.
 */
void hal_diag_sch_abort_latency(void);

/**
 * @brief Diagnostic indication for critical error.
 */
void hal_diag_assert(bool cond);


/********************************  SPI��غ���  *******************************/

/**
 * @brief SPI master module initialization for CI231X transceiver.
 *        ��ʼ��SPI����
 * @note SPI����Ϊģʽ0/MSB, Ҫ����Ч���ʸ���2Mbps��
 */
void hal_spim_init(void);

/**
 * @brief The API of access CI231X transceiver via SPI master.
 * @note  SPI����Ч��Ҫ�����ܸߣ�SPI�ֽ�֮���޿��У�����ǰ��׼������С��
 *
 * @param [in]     cmd     - Command in one byte to be sent.
 *        [in]     tx_data - Argument/data in tx_len byte to be sent.
 *        [in]     tx_len -  The length to be sent.
 *        [in/out] rx_data - Data in rx_len byte to be received.
 *        [in]     rx_len  - The length to be received.
 */
void hal_spim_transfer(uint8_t cmd,
		       const uint8_t *tx_data, uint8_t tx_len,
		       uint8_t *rx_data, uint8_t rx_len);


/******************************  ͨ��ADC��غ���  *****************************/

/**
 * @brief GPADC module initialization for battery and adkey etc.
 *        ��ʼ��ϵͳADC�� ���ڲɼ���ص�����
 */
void hal_adc_init(void);

/**
 * @brief Get the battery voltage.
 *
 * @return the battery in percentage. [0, 100]
 */
uint8_t hal_adc_get_battery(void);


/******************************  �������غ���  ******************************/

/**
 * @brief Get a random for BLE SMP protocol.
 *
 * @return a random in 4-byte.
 */
uint32_t hal_rng_get_word(void);


/***************************  ����ʧ�Դ洢��غ���  ***************************/

/**
 * @brief NVM(Flash/OTP) module initialization for config and MAC.
 * @note If the API is executed in flash to erase/program flash,
 *       please implement it in RAM function.
 *
 * @param [in] addr  - The address to be operated(erase/program/read).
 *        [in] len   - The length.
 *        [in] buf   - The pointer to memory buffer for program/read.
 *        [in] op    - Operation type.
 */
void hal_flash_op(uint32_t addr, uint32_t len, uint8_t *buf, uint8_t op);

#endif /* #ifndef __EXE_HAL_H__ */

