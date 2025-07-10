/*============================================================================*/
/* @file exe_stk_wrapper.c
 * @author onmicro
 * @date 2020/02
 */

#include <stdint.h>
#include "exe_hal.h"
#include "exe_hal_time.h"
#include "exe_api.h"
#include "exe_app.h"
#include "ci_log.h"
#include "rf_msg_deal.h"

/* These report value are declared in exe_profile_hid.c */
extern uint8_t hid_keyboard_in_report_value[];
extern uint8_t hid_consumer_report_value[2];
extern uint8_t hid_mouse_report_value[];
extern uint8_t hid_joystick_report_value[];

/* input event from hw input devices. @ref hid_data_type_t */
static uint8_t app_bitmap_hid_event;

#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_KEYBOARD)
/* the previous status of button press. */
static uint32_t button_value_bcup;
#endif

#if defined(HM1001_M0) && (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
#include "hal_mouse_bsp.h"
#endif

/* 用户设置的唤醒时间点。remove later. */
static uint32_t tick_app_wakeup;

/* 用户定义的蓝牙设备状态。remove later. */
app_device_status_t app_device_status;

#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_KEYBOARD)
static void _app_shutter_scan_button(void)
{
  uint32_t button_value = bsp_button_get_status();

  if (button_value == button_value_bcup)
  {
    return;
  }
  button_value_bcup = button_value;

  /* debounce完成，判断并通过notificaiton上报按键状态。*/
  if (button_value == 0)
  {
    app_bitmap_hid_event |= HID_DATA_GAMEPAD;
    hid_consumer_report_value[0] = 0x00;
    /* 上报Keyboard事件，有更好的手机兼容性。*/
    app_bitmap_hid_event |= HID_DATA_KEYBOARD;
    hid_keyboard_in_report_value[0] = 0x00;
    hid_keyboard_in_report_value[1] = 0x00;
  }
  else /* button_value != 0 */
  {
    /* Refer to the usages of HID Consume Control. */
    app_bitmap_hid_event |= HID_DATA_GAMEPAD;
    hid_consumer_report_value[0] = button_value;
    hid_consumer_report_value[1] = button_value >> 8;
    /* 上报Keyboard事件，有更好的手机兼容性。*/
    app_bitmap_hid_event |= HID_DATA_KEYBOARD;
    hid_keyboard_in_report_value[0] = 0x00; /* Control Key bitmap */
    hid_keyboard_in_report_value[1] = 0x28; /* keycode Enter */
  }
}
#endif

#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)

extern void mouse_move_circle_step(int R, int8_t *p_delta_x, int8_t *p_delta_y);
void app_mouse_scan(void)
{
  static bool button_press_flag = false;
  static bool first_connect_flag = false;
  static bool sleep_start_flag = false;
  static uint32_t app_sleep_tick = 0;
  
  /* 只有在加密连接（安全配对）完成且notification使能后，才扫描鼠标上报notification。*/
  if (!hid_notification_is_enabled() || !exe_gbool_link_is_encrypted)
    {first_connect_flag = false; return;}
  else {
    if(!first_connect_flag) { /* first connect, disable latency for better experience*/
      first_connect_flag = true;
      exe_gap_disable_latency();
    }
  }

#if APP_MOUSE_AUTO_CIRCLE
  /* Interference or too many LLCP traffic may result in tx queue full. */
  if (exe_stk_tx_queue_is_full()) return;

  app_bitmap_hid_event |= HID_DATA_MOUSE;
  hid_mouse_report_value[0] = 0x01; //u8Button
  mouse_move_circle_step(150, (int8_t *)&hid_mouse_report_value[1], (int8_t *)&hid_mouse_report_value[2]); //s8X, s8Y
  //hid_mouse_report_value[3] = bsp_wheel_get_data();

#else
  key_io_scan();
  key_data_l_0 = key_status;
  sensor_scan();
  key_process();
  wheel_data = bsp_wheel_get_data();

  if (0==(key_data_l&0x7) && (!button_press_flag) &&
      0==mouse_x && 0==mouse_y && 0==wheel_data) {
        if(!sleep_start_flag) {
            sleep_start_flag = true;
            app_sleep_tick = hal_sys_tick();
            exe_gap_enable_latency();
        }
        
        if (exe_time_elapsed_tick(app_sleep_tick, EXE_MS_TO_SYSTICK(1000*60))) {
            if (exe_time_elapsed_tick(app_sleep_tick, EXE_MS_TO_SYSTICK(1000*60*10)))
                hal_pm_sleep(1);
            else
               tick_app_wakeup = HAL_BLE_TICK_CURR() + EXE_MS_TO_BLETICK(200); 
        }
        else
        {
            tick_app_wakeup = HAL_BLE_TICK_CURR() + EXE_MS_TO_BLETICK(50); 
        }

        return;   
   }

  if(key_data_l&0x07)
      button_press_flag = true;
  else 
      button_press_flag = false;
  app_bitmap_hid_event |= HID_DATA_MOUSE;
  hid_mouse_report_value[0] = key_data_l&0x07; //u8Button
  hid_mouse_report_value[1] = 0-(mouse_x&0xFF);
  hid_mouse_report_value[2] = 0-(mouse_y&0xFF);
  hid_mouse_report_value[3] = (wheel_data&0xFF);
  mouse_x = mouse_y = wheel_data = 0;
  exe_gap_disable_latency();
  sleep_start_flag = false;
#endif //#if APP_MOUSE_AUTO_CIRCLE
}

#endif  //#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)

/**
 * @brief 扫描输入硬件，得到HID数据。将作为notify数据被发送到master。
 */
void app_scan_hwinput(void)
{
  /* Move from the caller of app_scan_hwinput(). */
  if (exe_gbool_ota_start) return;

#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_KEYBOARD)
  /* shutter */
  _app_shutter_scan_button();
#endif
}

/**
 * @brief 准备各种要发送的notify数据进入发送队列。
 * @note 在主循环中轮询调用。
 */
uint32_t app_tick_tx_notification = 0;
extern rf_cb_funcs_t rf_cb_funcs;
void app_tx_notification(void)
{
    if (hid_notification_is_enabled() == 0)
    {
      /* Notify properties被使能前，不能发送notification. */
      //return;
    }
    else
    {
      if (app_device_status == CONNECTED_DEVICE_STATUS)
      {
        /* Notify properties被使能后，才更新成设备首选的连接参数：min/max interval, latency, timeout。这样，手机才不会再次更新连接参数。*/
        /* interval=[10ms, 20ms], latency=4, timeout=6sec */
        //exe_l2cap_update_conn_para(8, 16, 4, 150*4);
        /* 申请连接参数更新成PPCP. */
        if (exe_l2cap_update_conn_para(exe_gu16_gap_ppcp_value[0], exe_gu16_gap_ppcp_value[1], exe_gu16_gap_ppcp_value[2], exe_gu16_gap_ppcp_value[3]))
          app_device_status = AFTER_CONNECTED_DEVICE_STATUS;
      } /* end if ((app_device_status == CONNECTED_DEVICE_STATUS) */
    }
#if(PROFILE_SIG_HIDS)
    if (app_bitmap_hid_event & HID_DATA_KEYBOARD)
    {
      if (hid_tx_notification(HID_DATA_KEYBOARD))
        app_bitmap_hid_event &= ~HID_DATA_KEYBOARD;
    } /* end if ((app_bitmap_hid_event & HID_DATA_KEYBOARD) */

    if (app_bitmap_hid_event & HID_DATA_GAMEPAD)
    {
      if (hid_tx_notification(HID_DATA_GAMEPAD))
        app_bitmap_hid_event &= ~HID_DATA_GAMEPAD;
    } /* end if ((app_bitmap_hid_event & HID_DATA_GAMEPAD) */

    if (app_bitmap_hid_event & HID_DATA_MOUSE)
    {
      if (hid_tx_notification(HID_DATA_MOUSE))
        app_bitmap_hid_event &= ~HID_DATA_MOUSE;
    } /* end if ((app_bitmap_hid_event & HID_DATA_MOUSE) */

    if (app_bitmap_hid_event & HID_DATA_JOYSTIC)
    {
      if (hid_tx_notification(HID_DATA_JOYSTIC))
        app_bitmap_hid_event &= ~HID_DATA_JOYSTIC;
    } /* end if ((app_bitmap_hid_event & HID_DATA_JOYSTIC) */
#endif
    
#if(PROFILE_CIAS_BLE)
  if (rf_recv_msg)
  {
    rf_cb_funcs.rf_recv(rf_recv_data, rf_recv_len);
    rf_recv_msg = false;
  } 
  else if (rf_send_msg)
  {
    cias_tx_notification();
    app_tick_tx_notification ++;
    if (app_tick_tx_notification >= 1)
    {
      app_tick_tx_notification = 0;
      rf_send_msg = false;
    }
  }
  
#endif

}

/**
 * @brief 中断驱动的引擎：
 *        进入睡眠，由定时或按键唤醒后，接收master的连接数据(LLCP,ATT,SMP)，或者广播自己。
 * @note 在主循环中调用。
 */
#define COON_MAX        5000000
uint32_t app_tick_start_connect;
bool start_conn = false;
bool status_connected = false;
bool status_exe_rx = false;
extern int32_t timer3_get_time_us();
void exe_stk_engine(void)
{
  exe_pm_sleep(tick_app_wakeup);
  //hal_pm_sleep(0);
  if (exe_stk_state != EXE_LINK_STATE_ADV)
  {
     /* Slave在连接态：先收后发。*/
    status_exe_rx = exe_ll_conn_rx();
    if(!start_conn)
    {
      start_conn = true;
      app_tick_start_connect = timer3_get_time_us();
    }
    if((timer3_get_time_us() - app_tick_start_connect) >= COON_MAX)
    {
      status_connected = true;
      exe_gap_disable_latency();
    }  
/*     if(status_connected && (status_exe_rx == 0))
    {
      exe_stk_state = EXE_LINK_STATE_ADV;
    } */
  } /* if (exe_stk_state != EXE_LINK_STATE_ADV) */
  else
  {
    start_conn = false;
    status_connected = false;
    exe_wakeup_src = 0/*clear*/;
    app_bitmap_hid_event = 0;
    
    /* 广播态：在3个广播频道上广播。*/
    //hal_gpio_init();
    exe_ll_adv_tx(ADV_CH_MAP_ALL/*0x7*/);
  } /* end if (exe_stk_state != EXE_LINK_STATE_ADV) */
  /* end of exe_stk_engine() */
}

