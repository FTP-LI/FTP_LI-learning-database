/*============================================================================*/
/* @file exe_app_main.c
 * @brief EXE shutter product main entry.
 * @author onmicro
 * @date 2020/02
 */

#include <stdint.h>
#include "exe_hal.h"
#include "exe_api.h"
#include "exe_app.h"
#include "ble_user_config.h"

#include <ci130x_gpio.h>
#include "exe_hal_pm.h"
#include "ci_ble_spi.h"
#include "ci130x_dpmu.h"
#include "ci130x_timer.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "ci_log.h"
#include "ci130x_timer.h"
#include "user_config.h"
#include "sdk_default_config.h"
#include "rf_msg_deal.h"
#include "ci_flash_data_info.h"
bool rf_recv_msg = false;
bool rf_send_msg = false;
volatile uint8_t rf_recv_len;
volatile uint8_t rf_send_len;
uint8_t rf_recv_data[RF_RX_TX_MAX_LEN] = {0};
uint8_t rf_send_data[RF_RX_TX_MAX_LEN] = {0};

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint32_t app_tick_led;

static uint32_t app_tick_scan_input;

static uint8_t app_led_toggle_status;

#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
static uint32_t app_tick_swift;
uint32_t app_100ms_swift;
#endif

uint32_t app_tick_connected;


/*********************************************************************
 * GLOBAL VARIABLES
 */

/* 用户层直接指定的本设备的Public BDA(mac). */
uint8_t exe_gbuf_bda[6] = {
    0x33,
    0x9f,
    0x20,
    0x62,
    0xbf,
    0x01,
};

uint8_t exe_gbuf_sn[6] = {0};

/* 用户直接设置的ADV_IND包，含包头和CRC. */
uint8_t exe_gbuf_adv_ind[42] = {
    ///.header
    0x00,
    37,
    ///.AdvA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    /// Flags: BLE limited discoverable mode and BR/EDR not supported
    0x02,
    0x01,
    0x06,
    /// incomplete list of service class UUIDs: (0x1812, 0x180F)
    // 0x03, 0x02, 0xAF, 0x30,
    0x07,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00,//设备类型
    0x00,
    0x00,//校验值
    /// Apperance
    // 0x03, 0x19, APP_GAP_APPEARANCE&0x0ff, (APP_GAP_APPEARANCE>>8)&0xff,
  
    0x07,
    0x09,
    'C',
    'I',
    '_',
    'B',
    'L',
    'E',  //广播名字
    /// RFstar BLE Light's MANUFACTURER_ID
    // 2, 1, 6, 3, 2, -80, -1, 9, -1, 83, -85, -38, 26, 31, -54, -25, -125,
    0,  // CRC24
    0,
    0,
};

/* 用户直接设置的SCAN_RSP包，含包头和CRC. */
uint8_t exe_gbuf_scan_rsp[13] = {
///.header
#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
    0x40 /* TxAdd=1 Random Address */ |
#endif
    0x04,
    0x06,
    ///.AdvA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    ///.ScanRspData[31]
    /**/
    /// CRC24
    0,
    0,
    0,
};

/* 用户直接设置的ADV_NONCONN_IND长包，含包头最长2+255字节. */
uint8_t exe_gbuf255_adv_nonconn_ind[2 + 255] = {
    ///.header
    0x02, 6 + 20 + 6 + 9 + 63 + 27 + 27,
    ///.AdvA
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /// Flags: BLE limited discoverable mode and BR/EDR not supported
    0x02, 0x01, 0x06,
    /// Apperance
    0x03, 0x19, BLE_APPEARANCE_GENERIC_THERMOMETER & 0x0ff, (BLE_APPEARANCE_GENERIC_THERMOMETER >> 8) & 0xff,
    /// Complete Local Name in 11 bytes always.
    12, 0x09,
    'H', 'I', 'D', '_', 'H', 'S', '6', '2', '2', '0', ' ',
    /// Complete list of service class UUIDs: Hilink, Eddystone
    5, 0x03, 0xEE, 0xFD, 0xAA, 0xFE,
    /// Huawei Hilink: Service UUID 0xFDEE
    8, 0x16, 0xEE, 0xFD, 0x01, 0x01, 0x01, 0x02, 0xC8,
    /// Google Eddystone: Service UUID 0xFEAA
    23, 0x16, 0xAA, 0xFE, 0x00, 0xC8, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x00, 0x00, /* Service Data, UID=0x00, TX Power, NID, BID, rsv */
    20, 0x16, 0xAA, 0xFE, 0x10, 0xC8, 0x01, 'o', 'n', 'm', 'i', 'c', 'r', 'o', '.', 'c', 'o', 'm', '.', 'c', 'n',                                 /* Service Data, URL=0x10, Tx Power, prefix+url  */
    17, 0x16, 0xAA, 0xFE, 0x20, 0x00, 0x0C, 0xE4, 0x1B, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x88,                                     /* Service Data, TLM=0x20, version, vbat_1mV, temp_8d8, AdvCnt, uptime_100ms */
    /// Manufacturer Specific Data: iBeacon
    26, 0xFF, 0x4C, 0x00, 0x02, 0x15, 0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, 0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61, 0x11, 0x11, 0x02, 0x00, 0xC8, /* TX Power */
    /// Manufacturer Specific Data: AltBeacon
    26, 0xFF, 0x18, 0x01, 0xAC, 0xBE, 0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, 0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61, 0x11, 0x11, 0x02, 0x00, 0xC8, /* TX Power */
};

/* 兼容性参数: 连接事件n次后自动使能通知(ATT Notify)，53(P30Pro)。*/
uint8_t exe_gu8_gap_conn_inst = 60;

/* 设备首选的连接参数：IntervalMin, IntervalMax, SlaveLatency, SupervisionTimeout.
   @ref ble_l2cap_signaling_connection_parameters_update_request_t */
#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_KEYBOARD)
/* shutter: IntervalMin=20*1.25=25ms IntervalMax=40*1.25=50ms Latency=4 Timeout=100*10ms=1sec */
uint16_t exe_gu16_gap_ppcp_value[4] = {20, 40, 4, 100};
#elif (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
/* mouse:   IntervalMin=6*1.25=7.5ms IntervalMax=6*1.25=7.5ms Latency=44 Timeout=216*10ms=2s */
uint16_t exe_gu16_gap_ppcp_value[4] = {6, 6, 44, 216};
#else
/* meter:   IntervalMin=6*1.25=7.5ms IntervalMax=6*1.25=7.5ms Latency=4 Timeout=100*10ms=1sec */
uint16_t exe_gu16_gap_ppcp_value[4] = {6, 6, 4, 100};
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */

void user_init(void)
{

  /* 广播（两次广播event）间隔是20ms. */
  exe_gms_adv_interval = 20;
  /* 低功耗模式：连接态和广播态都是普通睡眠模式。*/
  exe_gbitmask_sleep_mode = PM_SUSPEND_SLEEP_CONN | PM_SUSPEND_SLEEP_ADV;

  exe_stk_init();

#if EXE_NDA_FEAT_SMP
  /* 拍照器应用总是需要SMP过程。*/
  exe_smp_enable_bond();
#endif /* EXE_NDA */

  app_tick_scan_input = app_tick_connected = app_tick_led = hal_sys_tick();

  /* 按任意键启动, 使能免配对OTA. */
  /*   if (bsp_button_get_status()) {
      exe_gbool_ota_start = true;
    } */
}

/**
 * @brief 蓝牙任务运行主函数，非载波模式下，使用while无线循环调用此函数
 *
 */
void app_execute_once(void)
{

  if (exe_stk_state != EXE_LINK_STATE_ADV /*0x0*/)
  {
    app_tx_notification();
  }
  exe_stk_engine();
}

/**
 * @brief 蓝牙中断处理函数；
 * 在中断处理函数中释放信号量，表示上一次蓝牙广播或者数据通讯完成，切换到蓝牙任务，继续运行蓝牙协议栈
 */
void rf_irq_handle(gpio_base_t base, int gpio_port_index)
{
  if (base = PB)
  {
    if (gpio_port_index == 0)
    {
     // mprintf("hal_rf_wakeup is called...port = %d \r\n", gpio_port_index);
    #if CIAS_BLE_SCAN_ENABLE
      scan_adv_callback();
    #else
      hal_pm_wakeup();
    #endif
    }
  }
}

/**
 * @brief 配置蓝牙芯片中断引脚，广播和连接态传输数据时，会一直触发中断
 *
 */
void rf_irq_pin_init(void)
{
  dpmu_set_io_reuse(CI231X_RF_IRQ_PIN_PAD, FIRST_FUNCTION);
  dpmu_set_io_direction(CI231X_RF_IRQ_PIN_PAD, DPMU_IO_DIRECTION_INPUT);
  dpmu_set_io_pull(CI231X_RF_IRQ_PIN_PAD, DPMU_IO_PULL_UP);
  gpio_set_input_mode(CI231X_RF_IRQ_PORT, CI231X_RF_IRQ_PIN);

  gpio_irq_trigger_config(CI231X_RF_IRQ_PORT, CI231X_RF_IRQ_PIN, down_edges_trigger); // 下降沿触发
  //__eclic_irq_set_vector(PB_IRQn,(int32_t)ble_irq_callback);         
	eclic_irq_enable(PB_IRQn);
}


/**
 * @brief 设备初始化-匹配启英泰伦小程序
 */
void user_dev_init(uint8_t dev_type_id, uint8_t dev_number_id, uint8_t config_type)
{
  
  exe_gbuf_adv_ind[14] = config_type;
  exe_gbuf_adv_ind[15] = dev_type_id;
  exe_gbuf_adv_ind[16] = dev_number_id;

  uint16_t crc_cal = crc16_ccitt(0, &exe_gbuf_adv_ind[13], 4);
  exe_gbuf_adv_ind[17] = crc_cal>>8;
  exe_gbuf_adv_ind[18] = crc_cal&0xFF;        //设备初始化厂商信息                                             

  uint8_t flash_unique_id[20];
  post_spic_read_unique_id((uint8_t*)flash_unique_id);
  memcpy(exe_gbuf_bda,&flash_unique_id[2], 6); //设备初始化MAC地址
}

/**
 * @brief 设备初始化广播名字
 * @param name 广播名字字符串
 */
bool ble_name_init(char* name)
{
  uint8_t len = strlen(name);
  if (len > BLE_NAME_MAX_LEN)
  {
    len = BLE_NAME_MAX_LEN;   //超过18，默认使用前18个字节作为name
  }
  exe_gbuf_adv_ind[19] = len + 1;
  memset(&exe_gbuf_adv_ind[21], 0, BLE_NAME_MAX_LEN);
  memcpy(&exe_gbuf_adv_ind[21], name, len);
  return true;
}

/**
 * @brief 设备初始化MAC地址,如果不调用，设备默认使用flashid作为mac地址
 * @param mac 广播名字字符串
 */
void ble_mac_init(uint8_t mac[6])
{
  memcpy(exe_gbuf_bda,mac, 6);
}

extern uint16_t cias_service_uuid;  
extern uint16_t profile_uuids_set_sig[];                   
/**
 * @brief 设备初ble设备服务uuid
 * @param service_uuid 主服务uuid
 * @param write_uuid   写服务uuid，用于手机向设备传输数据
 * @param service_uuid 通知服务uuid，用于设备向手机传输数据
 */
void ble_uuid_init(uint16_t service_uuid, uint16_t write_uuid, uint16_t notify_uuid)
{
  cias_service_uuid = service_uuid;  
  profile_uuids_set_sig[23] = write_uuid;  
  profile_uuids_set_sig[24] = notify_uuid;                    
}

/**
 * @brief 设备初始化广播数据
 * @param adv_data adv_data广播数据有效载荷
 */
bool ble_adv_data_init(char *adv_data, uint8_t adv_data_len)
{
  if (adv_data_len > 28)
  {
    return false;
  }
  
  memset(&exe_gbuf_adv_ind[11], 0, 28);
  memcpy(&exe_gbuf_adv_ind[11], adv_data, adv_data_len);
  return true;
}

/**
 * @brief 设置BLE名称
 */
void user_ble_name_init(void)
{
    char ble_name[BLE_NAME_MAX_LEN] = {0};
    post_read_flash(ble_name, FILECONFIG_SPIFLASH_START_ADDR + 12, BLE_NAME_MAX_LEN);
    ble_name_init(ble_name);    //使用生成固件时配置的硬件名称作为广播名字
}
extern volatile bool ble_msg_send_erro;
extern volatile bool ble_msg_send_flag;

void ble_msg_queue_deal()
{
  static uint32_t ble_msg_send_counter = 0;
  static uint16_t ble_msg_send_erro_counter = 0;
  if (ble_msg_send_flag)
  {
    ble_msg_send_counter ++;
    if (ble_msg_send_counter > 10)
    {
      ble_msg_send_counter = 0;
      ble_msg_send_erro_counter = 0;
      ble_msg_send_erro = true;
      mprintf("<<<<<<<<<<<<<<<<<<<<<ble_msg_send_flag error>>>>>>>>>>>>>>\r\n");
      ble_msg_send_flag = false;
    }
  }
  if (ble_msg_send_erro)
  {
    ble_msg_send_erro_counter ++;
    vTaskDelay(pdMS_TO_TICKS(10));
    if (ble_msg_send_erro_counter > 300 && !prompt_is_playing())
    {
      ble_msg_send_erro_counter = 0;
      ble_msg_send_erro = false;
    }
  }
  
}
/**
 * @brief ble协议栈运行入口函数
 */
extern uint8_t ble_mode;
extern uint8_t ble_ch;
//该任务用户不用关注

void ble_exec_loop(void)
{
  hal_pm_init();
  hal_rf_init();
  if (ble_mode)
  {
    ble_change_channel(ble_ch);
    ble_change_mode(CI231X_RF_Carrier_Mode);
    ci231x_rf_ce_high();
  }
  user_init();
  rf_irq_pin_init(); 
  vTaskDelay(pdMS_TO_TICKS(500));                  // 休眠启动广播，避免和语音播报任务冲突
  while (1)
  {
    if (!ble_mode)
    {
      app_execute_once();
      ble_msg_queue_deal();
    }
    else
    {
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}

void exe_app_exit(void)
{
  /* Dummy implementation w/o RTOS. */
}

bool ble_port_init()
{
  hal_pm_init();
  hal_rf_init();
  user_init();
  return 1;
}

rf_cb_funcs_t rf_cb_funcs;
/**
 * @brief 处理蓝牙/2.4G发送的消息
 * 将用户需要发送的数据复制到发送缓冲区，触发射频发送
 * @param send_data 需要发送的数据；
 * @param len       发送数据长度；
 */
void deal_rf_send_data(uint8_t* send_data, uint8_t len)
{
    if (len > RF_RX_TX_MAX_LEN)
    {
      mprintf("deal_rf_send_data len error\r\n");
      return;
    }
    memset(rf_send_data, 0, RF_RX_TX_MAX_LEN);
    memcpy(rf_send_data, send_data, len);
    rf_send_msg = true;
    rf_send_len = len;
}

void register_rf_callback(void *recv_fucn)
{
    rf_cb_funcs.rf_recv = recv_fucn;
    rf_cb_funcs.rf_send = deal_rf_send_data;
}


TaskHandle_t ble_task_handle = NULL;
/**
 * @brief AT指令配置成功后，删除并重启蓝牙协议栈任务，使配置的蓝牙芯片参数生效
 * 
 */
void 
reset_ble_task()
{
    vTaskDelete(ble_task_handle);
    vTaskDelay(100);
    xTaskCreate(ble_exec_loop,"ble_exec_loop",1024*2, NULL, 5, &ble_task_handle);
}
