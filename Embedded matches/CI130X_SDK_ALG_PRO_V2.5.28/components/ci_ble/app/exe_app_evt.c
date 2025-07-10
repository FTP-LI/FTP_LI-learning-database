/*============================================================================*/
/* @file exe_app_evt.c
 * @brief EXE stack Events, in form of callback functions.
 * @author onmicro
 * @date 2020/02
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "exe_hal.h"
#include "exe_api.h"
#include "exe_app.h"
#include "ci_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "system_msg_deal.h"
#include "rf_msg_deal.h"

extern bool status_connected;
extern bool ota_att_handler(exe_data_pkt_t *p_xpkt_req);
extern app_device_status_t app_device_status;
extern uint32_t app_tick_connected;
extern uint32_t app_100ms_swift;

/* 用户层用于记录SCAN_REQ.ScanA. */
static uint8_t app_phone_bda[6];
/* 用户层用于记录对方手机的VersNr, CompId. */
static uint8_t app_phone_bt_version;
uint16_t app_phone_company_id;
/* 用户层用于记录对方手机的连接参数。*/
static uint16_t app_phone_ble_interval;
static uint16_t app_phone_ble_latency;
static uint16_t app_phone_ble_timeout;

/* 可用于电池电量监控。*/
uint32_t app_tick_bat;
uint8_t app_percent_bat_level = 100;

/* 可用于统计连接期间的空闲时间。*/
uint8_t app_stat_connection_idle;


xTimerHandle connect_timer = NULL; //定时器
uint8_t connect_timer_counter;
static connect_timer_callback()
{
  if (connect_timer_counter)
  {
    connect_timer_counter --;
    //mprintf("ble连接超时剩余:%d\r\n",connect_timer_counter);
  }
  else
  {
    xTimerStop(connect_timer,0);
    //mprintf("ble连接超时\r\n");
    reset_ble_task();
  }
}

void init_connect_timer()
{
  connect_timer = xTimerCreate("connect_timer", pdMS_TO_TICKS(1000),pdTRUE, (void *)0, connect_timer_callback);
}

/**
 * @brief 用户层ATT Read回调。
 *
 * @param [in] p_xpkt_req - ATT read request.
 * @return true for success; false for failure.
 */
bool app_cb_att_read(exe_data_pkt_t *p_xpkt_req)
{
  status_connected = false;
  //mprintf("[%10d] att read handle=%d\n", EXE_TIMESTAMP_MS(), p_xpkt_req->pdu.att.payload.read_req.Handle);
  return false;
}

/**
 * @brief 用户层ATT Write回调：这里实现OTA.
 *
 * @param [in] p_xpkt_req - ATT write request.
 * @return true for success; false for failure.
 */

bool app_cb_att_write(exe_data_pkt_t *p_xpkt_req)
{
  status_connected = false;
  uint8_t len = p_xpkt_req->l2cap.Length - 3/*sizeof opcode+handle*/;
  uint8_t *p_data = &p_xpkt_req->pdu.att.payload.write_req.Value[0];
  //mprintf("att write handle=%d:  %d-byte[%02x...%02x]\n", p_xpkt_req->pdu.att.payload.write_req.Handle, len,p_data[0], p_data[len]);
  if (p_xpkt_req->pdu.att.payload.write_req.Handle == 14)
  {
    rf_recv_msg = true;
    rf_recv_len = len;
  }
  
#if (BLE_CONNECT_TIMEOUT!=0)
  if(connect_timer_counter)
    xTimerStop(connect_timer, 0);
#endif // BLE_CONNECT_TIMEOUT
  
#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_GENERIC_THERMOMETER)
  if (p_xpkt_req->pdu.att.payload.write_req.Handle == 24/*ATT_HANDLE_UART_RX_CV*/) {
    /* FIXME: loopback NUS, i.e. received the data from phone and transmit the same to phone. */
    nus_tx_notification(p_data, len);

    /* Terminate the current link and restart exble. */
    if ((2 == len) && ('n' == p_data[0]) && ('b' == p_data[1])) {
      exe_ll_terminate();
      exe_app_exit();
    }
  }
#endif

#if 0
  if (p_xpkt_req->pdu.att.payload.write_req.Handle == 38)//0x39) /*ATT_HANDLE_OTA_RW_CV*/
  { ota_att_handler(p_xpkt_req); return true; }
  else
#endif
  { return false; }
}

/**
 * @brief 用户层回复未知ATT REQ的回调，用户准备RSP的内容。
 *
 * @param [out] p_xpkt_rsp - ATT response.
 */
void app_cb_att_response(exe_data_pkt_t *p_xpkt_rsp)
{
}

/**
 * @brief 用户层回调函数：指示已建立连接。
 *
 * @param [in] p_xpkt_ind - CONNECT_IND w/ xpkt_len.
 */
void app_cb_received_connect_ind(exe_adv_pkt_t *p_xpkt_ind)
{
  mprintf("app_cb_received_connect_ind connected\r\n");
  app_tick_bat = app_tick_connected = hal_sys_tick();
  app_stat_connection_idle = 0;
  app_percent_bat_level = 100;

  app_device_status = CONNECTED_DEVICE_STATUS/*4*/;

  app_phone_ble_interval = p_xpkt_ind->ble.payload.connect_ind.Interval * 5/4;
  app_phone_ble_latency  = p_xpkt_ind->ble.payload.connect_ind.Latency;
  app_phone_ble_timeout  = p_xpkt_ind->ble.payload.connect_ind.Timeout * 10;
  (void)app_phone_ble_interval;
  (void)app_phone_ble_latency;
  (void)app_phone_ble_timeout;
#if (BLE_CONNECT_TIMEOUT!=0)
  connect_timer_counter = BLE_CONNECT_TIMEOUT;
  xTimerStop(connect_timer, 0);
  xTimerStart(connect_timer, 0);
#endif // BLE_CONNECT_TIMEOUT
}

/**
 * @brief 用户层回调函数：显示SCAN_REQ的ScanA地址。
 *
 * @param [in] p_xpkt_req - SCAN_REQ包，含xpkt_len。
 */
void app_cb_received_scan_req(exe_adv_pkt_t *p_xpkt_req)
{
  memcpy(&app_phone_bda[0], &p_xpkt_req->ble.payload.scan_req.ScanA[0], 6);
}

/**
 * @brief 用户回调函数：收到master的LL控制数据包LLCP_VERSION_IND。
 *
 * @param [in] p_xpkt - EXE LL控制数据包，含xpkt_len的。
 */
void app_cb_received_version_ind(exe_llcp_pkt_t *p_xpkt)
{
  /* CompId
     0x000F: 'Broadcom Corporation',
     0x001D: 'Qualcomm',
     0x0046: 'MediaTek, Inc.',
     0x004C: 'Apple, Inc.',
     0x0075: 'Samsung Electronics Co. Ltd.',
     0x010F: 'HiSilicon Technologies Co., Ltd.',
     0x01EC: 'Spreadtrum Communications Shanghai Ltd',
     0x027D, 'HUAWEI Technologies Co. Ltd.',
   */
  //mprintf("app_cb_received_version_ind %x\r\n", p_xpkt->llcp.payload.version_ind.CompId);
  app_phone_company_id = p_xpkt->llcp.payload.version_ind.CompId;
  app_phone_bt_version = p_xpkt->llcp.payload.version_ind.VersNr;
  (void)app_phone_company_id;
  (void)app_phone_bt_version;
}

/**
 * @brief 用户回调函数：收到master的LL控制数据包LLCP_CONNECTION_UPDATE_IND。
 *
 * @param [in] p_xpkt - EXE LL控制数据包，含xpkt_len的。
 */
void app_cb_received_update_ind(exe_llcp_pkt_t *p_xpkt)
{
  //mprintf("app_cb_received_update_ind \r\n");
  app_phone_ble_interval = p_xpkt->llcp.payload.connection_update_ind.Interval * 5/4;
  app_phone_ble_latency  = p_xpkt->llcp.payload.connection_update_ind.Latency;
  app_phone_ble_timeout  = p_xpkt->llcp.payload.connection_update_ind.Timeout * 10;
  (void)app_phone_ble_interval;
  (void)app_phone_ble_latency;
  (void)app_phone_ble_timeout;
}

/**
 * @brief 用户层回调函数：指示连接断开。
 *
 */
extern uint32_t _pm_tick_wakeup;
void app_cb_disconnected(void)
{
  app_device_status = POWER_ON_DEVICE_STATUS/*0*/;
  mprintf("app_cb_disconnected\r\n");
  connect_timer_counter = 1;
  xTimerStop(connect_timer, 0);
  xTimerStart(connect_timer, 0); 
  _pm_tick_wakeup = 0;
  vTaskDelete(NULL);
}

/**
 * @brief 用户层回调函数：指示配对完成。
 */
void app_cb_bonded(uint8_t *p_ltk)
{
  int idx;
  mprintf("[%10d] bonded: ", EXE_TIMESTAMP_MS());
  for (idx = 0; idx < 16; idx++) {
    mprintf("%02x", p_ltk[15-idx]);
  }
  mprintf("\n");

}

/**
 * @brief 用户层回调：指示连接的手机操作系统类型。
 * @note 可不使用这个接口，而是根据连接手机的CompId。
 *
 * @param [in] phone_type - 0=Unknown
 *                          1=iOS/Android which supports Privacy
 *                          2=Android
 *                          3=iOS
 */
void app_cb_phone_os_ind(uint8_t phone_type)
{
}


