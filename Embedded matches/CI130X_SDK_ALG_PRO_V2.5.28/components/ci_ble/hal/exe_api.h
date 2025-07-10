/*============================================================================*/
/* @file exe_api.h
 * @brief EXE stack public APIs.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_API_H__
#define __EXE_API_H__

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include "exe_pkt.h"
#include "user_config.h"
#include "sdk_default_config.h"
#define PROFILE_CIAS_BLE        1
/*********************************************************************
 * MACROS
 */

/**< Maximum ATT MTU. */
#define EXE_ATT_MTU_SIZE                23

/**< Maximum length of data (in bytes) that can be transmitted over ATT read/write/notification in exBLE. */
#define EXE_ATT_PAYLOAD_LEN_MAX         (EXE_ATT_MTU_SIZE - 3)


/*********************************************************************
 * TYPEDEFS
 */

/* 协议栈分广播态和连接态。*/
typedef enum {
  EXE_LINK_STATE_ADV = 0,
  EXE_LINK_STATE_CONN
} exe_stk_state_t;

/* 输入设备的就绪数据类型。*/
typedef enum
{
  HID_DATA_KEYBOARD = (0x1 << 0),
  HID_DATA_GAMEPAD = (0x1 << 1),
  HID_DATA_MOUSE = (0x1 << 2),
  HID_DATA_JOYSTIC = (0x1 << 3),
  HID_DATA_PEN = (0x1 << 4),  
  HID_DATA_URGENT = (0x1 << 7),
} hid_data_type_t;

/* ATT属性表项。*/
typedef struct {
  ///The number of Attribute handles in this Service: 0 means Char rather than Service.
  uint8_t  num_handle;
  ///The length of Attribute UUID: 2 is UUID16, 16 is UUID128.
  uint8_t  uuid_len;
  ///The length of Attribute Value.
  uint8_t  value_len;
  ///The index to UUID16 or UUID128: PS, CD, SCCD, CCCD; dis, bas, hids XXX; vendor
  uint8_t  uuid_idx;
  ///The pointer to Attribute Value: ServiceUUID for PS; {CharProp, CharValueHandle, CharUUID} for CD; SCC's Value; CCC value; DevName; Appearance; PPCP; PnP IDs; Bat level; HID Report Map; HID Report Value etc
  uint8_t *p_value;
} exe_att_ent_t;


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/* 协议栈内部状态：0-广播态  1-连接态  @ref exe_stk_state_t. */
extern exe_stk_state_t exe_stk_state;

/* true-OTA功能 false-HID功能 */
extern bool exe_gbool_ota_start;

/* 广播（两次广播event）间隔，单位ms. */
extern uint16_t exe_gms_adv_interval;

/* 用户层指定的本设备的Public BDA. */
extern uint8_t exe_gbuf_bda[6];

/* 用户设置的ADV_IND包，含包头和CRC. */
extern uint8_t exe_gbuf_adv_ind[];

/* 用户设置的SCAN_RSP包，含包头和CRC. */
extern uint8_t exe_gbuf_scan_rsp[];

/* 用户直接设置的ADV_NONCONN_IND长包，含包头最长2+255字节. */
extern uint8_t exe_gbuf255_adv_nonconn_ind[];

/* HID GATT属性表database。*/
extern const exe_att_ent_t exe_gtbl_gatt_database[];

/* 判断对方手机的OS类型: 0-unknown 1-RPA/iOS 2-Android/Windows */
extern uint8_t exe_gu8_phone_type;

/* 兼容性参数: 指定连接事件多少次后自动使能通知(ATT Notify)。*/
extern uint8_t exe_gu8_gap_conn_inst;

/* 设备首选的连接参数：IntervalMin, IntervalMax, SlaveLatency, SupervisionTimeout.
   @ref ble_l2cap_signaling_connection_parameters_update_request_t */
extern uint16_t exe_gu16_gap_ppcp_value[4];

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @brief 初始化EXE协议栈。
 */
void exe_stk_init(void);

/**
 * @brief 使能SMP配对。
 * @note 拍照器和键鼠需要与手机配对后绑定。
 *       数传可以不用使能绑定功能。
 */
void exe_smp_enable_bond(void);

/**
 * @brief 封装好的中断驱动的协议栈引擎：
 *        进入睡眠，由定时或按键唤醒后，       @ref exe_pm_sleep()
 *        接收master的连接数据(LLCP,ATT,SMP)， @ref exe_ll_conn_rx()
 *        或者广播自己。                       @ref exe_ll_adv_tx()
 * @note 在主循环中调用。
 */
void exe_stk_engine(void);

/**
 * @brief 得到发送队列是否已空。
 *
 * @return true for empty, false for not-empty.
 */
bool exe_stk_tx_queue_is_empty(void);

/**
 * @brief 得到发送队列是否已满。
 *
 * @return true for full, false for not-full.
 */
bool exe_stk_tx_queue_is_full(void);

/**
 * @brief 得到发送队列的已用空间（待发送包数）。
 *
 * @return [0,3]
 */
int exe_stk_tx_queue_get_used(void);

/**
 * @brief 发送广播包，以及回复手机的SCAN_REQ，或者处理CONNECT_IND准备连接。
 * @note 主循环里被协议栈引擎调用，用于广播态下发送广播。
 *
 * @param [in] mask - The advertising channel bitmask, 3-bit for channel 37,38,39.
 *                    1 means tx adv on this channel.
 *                    @ref ADV_CH_MAP_xxx.
 * @return NULL如果没有发送，!NULL如果收到CONNECT_IND.
 */
uint8_t *exe_ll_adv_tx(int mask);

/**
 * @brief 接收连接数据，然后处理SMP, LLCP, L2CAP。
 * @note 主循环里被协议栈引擎调用，用于连接态下保持连接。
 *
 * @param None.
 *
 * @return false没收到包，true收到包。
 */
bool exe_ll_conn_rx(void);

/**
 * @brief Slave主动中止连接。
 * @note 断开连接原因错误码0x13: Remote User Terminated Connection
 */
void exe_ll_terminate(void);

/**
 * @brief 发送指定类型的HID notification data至手机。
 * @note HID profile调用exe_att_tx_notification()来发送HID notification data.
 *
 * @param [in] data_type - Mouse/Keyboard/Gamepad.
 * @return true for success, false for no space in tx queue or notification is not enabled.
 */
bool hid_tx_notification(hid_data_type_t data_type);

/**
 * @brief 检查HID设备ATT notify是否使能。
 * @note ATT notify使能后，才能发送notification，以及发起最后起作用的连接参数更新。
 *
 * @return CCCD (Client Char Config Descriptor) value, i.e. [notify | indicate].
 */
uint8_t hid_notification_is_enabled(void);

/**
 * @brief 发送电池电量notification data至手机。
 * @note BAS profile调用exe_att_tx_notification()来发送该notification data.
 *
 * @param [in] level - Battery level in percentage [0~100].
 * @return true for success, false for no space in tx queue or notification is not enabled.
 */
bool bas_tx_notification(uint8_t level);

/**
 * @brief 发送蓝牙串口TX notification data至手机。
 * @note NUS profile调用exe_att_tx_notification()来发送该notification data.
 *
 * @param [in] p_data - ATT Value of notification.
 * @param [in] len    - The length of ATT value, maximum 20-byte.
 * @return true for success, false for no space in tx queue or notification is not enabled.
 */
bool nus_tx_notification(uint8_t *p_data, uint8_t len);

/**
 * @brief 向指定Attribute Handle发送notification data至手机。
 * @note 如果用户实现非HID的profile，可以调用这个API.
 *
 * @param [in] handle - ATT Handle.
 * @param [in] p_data - ATT Value of notification.
 * @param [in] len    - The length of ATT value, maximum 20-byte.
 * @return !0 for the actual length of ATT value, 0 for no space in tx queue.
 */
uint8_t exe_att_tx_notification(uint16_t handle, uint8_t *p_data, int len);

/**
 * @brief 设备侧主动发起连接参数更新。
 * @note 仅仅在连接态下调用才起作用。
 *
 * @param [in] interval_min - Defines minimum value for the connection interval in 1.25ms.
 *        [in] interval_max - Defines maximum value for the connection interval in 1.25ms.
 *        [in] latency      - Defines the slave latency parameter.
 *        [in] timeout      - Defines connection timeout parameter in 10ms.
 * @return true for success, false for no space in tx queue.
 */
bool exe_l2cap_update_conn_para(uint16_t interval_min, uint16_t interval_max, uint16_t latency, uint16_t timeout);

/**
 * @brief 设备侧主动发起基于LE Credit的L2CAP连接。
 * @note 仅仅在连接态下调用才起作用。
 * @note 仅仅用于BQB PTS认证。
 *
 * @param [in] psm  - LE Protocol/Service Multiplexer: Object Transfer Service 0x0025.
 *        [in] scid - Source Channel ID: dynamically allocated in [0x0040-0xFFFF].
 * @return true for success, false for no space in tx queue.
 */
bool exe_l2cap_create_le_conn(uint16_t psm, uint16_t scid);

/**
 * @brief 禁止latency，以提高连接可靠性和数据带宽。
 * @note API, called by ota_att_handler() etc.
 */
void exe_gap_disable_latency(void);

/**
 * @brief 允许latency，以减少射频收发的次数，降低功耗。
 * @note 该API只是允许latency，射频收发次数是否真的减少还取决别的因素。
 */
void exe_gap_enable_latency(void);

/**
 * @brief 进入可被发现的配对模式。
 * @note API, 一般用于从连接态强行进入配对模式.
 */
void exe_gap_discoverable(void);

/**
 * @brief 获得用户指定的与连接事件大致对齐的绝对时间点。
 * @note 该时间点可传给接口exe_pm_sleep()，以实现用户定时器。
 *
 * @param [in] delta - 用户指定的时间点偏移，单位bletick。
 *
 * @return 连接事件大致对齐的绝对时间点，单位bletick。
 */
uint32_t exe_ll_conn_align_bletick(uint32_t delta);

/**
 * @brief 进入睡眠，等待定时中断唤醒，或按键唤醒。
 * @note 主循环里被协议栈引擎调用，用于进入广播或者连接事件之间的低功耗状态。
 *
 * @param [in] bletick_wakeup_app - 用户指定的唤醒时间点，单位bletick；
 *                                  传入0的话由协议栈内部自动维护唤醒点。
 *
 * @return None
 */
void exe_pm_sleep(uint32_t bletick_wakeup_app);

/**
 * @brief 为OnMicro私有实现的Broadcaster/Observer做准备。
 */
void exe_ll_adv_proprietary_init(void);

/**
 * @brief 扫描者接收广播包，并尝试发送指定的广播包。
 * @note OnMicro私有实现的Observer，可用于AC-DC供电的传感器模块侧。
 * @note 支持exe_ll_scan_rx2tx_wl()，接收负载最长37字节，可支持白名单。
 *
 * @param [in]  mask     - 40-bit的广播通道掩码。
 * @param [out] p_pkt_rx - 指向含2-字节包头的缓冲用于接收广播包，不能为NULL。
 * @param [in]  p_pkt_tx - 指向含2-字节包头的缓冲用于发送广播包，可以为NULL。
 * @param [in]  p_wl     - 指向白名单，2个过滤地址，即12-字节，可以为NULL。
 * @return 0xff   - 如果没有收到广播包
 *         [0,39] - 收到广播包的通道索引。
 */
uint8_t exe_ll_scan_rx2tx(uint64_t mask, uint8_t *p_pkt_rx, uint8_t *p_pkt_tx);
uint8_t exe_ll_scan_rx2tx_wl(uint64_t mask, uint8_t *p_pkt_rx, uint8_t *p_pkt_tx, uint8_t *p_wl);

/**
 * @brief 广播者发送广播包，并尝试接收指定的广播包。
 * @note OnMicro私有实现的Broadcaster，可用于电池供电的NB-IoT模块侧。
 *
 * @param [in]  mask     - 40-bit的广播通道掩码。
 * @param [out] p_pkt_tx - 指向含2-字节包头的缓冲用于发送广播包，不能为NULL。
 * @param [in]  p_pkt_rx - 指向含2-字节包头的缓冲用于接收广播包，可以为NULL。
 * @return 0xff   - 如果没有收到广播包
 *         [0,39] - 收到广播包的通道索引。
 */
uint8_t exe_ll_adv_tx2rx(uint64_t mask, uint8_t *p_pkt_tx, uint8_t *p_pkt_rx);

/**
 * @brief 用户层事件回调函数：显示SCAN_REQ的ScanA地址。
 *
 * @param [in] p_xpkt_req - SCAN_REQ包，含xpkt_len。
 */
void app_cb_received_scan_req(exe_adv_pkt_t *p_xpkt_req);

/**
 * @brief 用户层事件回调函数：指示已建立连接。
 *
 * @param [in] p_xpkt_ind - CONNECT_IND w/ xpkt_len.
 */
 void app_cb_received_connect_ind(exe_adv_pkt_t *p_xpkt_ind);

/**
 * @brief 用户层事件回调函数：指示连接断开，对方主动断开或者连接超时。
 *
 */
void app_cb_disconnected(void);

/**
 * @brief 用户层事件回调函数：指示配对完成。
 *
 * @param [in] p_ltk - Pointer to link key in little endian.
 */
void app_cb_bonded(uint8_t *p_ltk);

/**
 * @brief 用户层事件回调函数：指示手机的控制器版本。
 *
 * @param [in] p_xpkt - EXE LL控制数据包，含xpkt_len的。
 */
void app_cb_received_version_ind(exe_llcp_pkt_t *p_xpkt);

/**
 * @brief 用户层事件回调函数：记录最后的连接参数。
 *
 * @param [in] p_xpkt - EXE LL控制数据包，含xpkt_len的。
 */
void app_cb_received_update_ind(exe_llcp_pkt_t *p_xpkt);

/**
 * @brief 用户层事件回调函数：ATT read event。
 *
 * @param [in] p_xpkt_req - ATT read request.
 * @return true for success; false for failure.
 */
bool app_cb_att_read(exe_data_pkt_t *p_xpkt_req);

/**
 * @brief 用户层事件回调函数：ATT write event，可实现OTA.
 *
 * @param [in] p_xpkt_req - ATT write request.
 * @return true for success; false for failure.
 */
bool app_cb_att_write(exe_data_pkt_t *p_xpkt_req);

/**
 * @brief 用户层事件回调函数：用户准备RSP的内容，用以回复未知ATT REQ。
 *
 * @param [out] p_xpkt_rsp - ATT response.
 */
void app_cb_att_response(exe_data_pkt_t *p_xpkt_rsp);
void app_tx_notification(void);
void ble_exec_loop(void);
bool cias_tx_notification();

typedef struct {
	void (*rf_recv)(uint8_t* recv_data, uint8_t len);
	void (*rf_send)(uint8_t* send_data, uint8_t len);
} rf_cb_funcs_t;

extern bool rf_recv_msg;                    //接收数据标记
extern bool rf_send_msg;                    //发送数据标记
extern volatile uint8_t rf_recv_len;        //接收数据长度
extern volatile uint8_t rf_send_len;        //发送数据长度
extern uint8_t rf_recv_data[RF_RX_TX_MAX_LEN];    //接收缓冲区
extern uint8_t rf_send_data[RF_RX_TX_MAX_LEN];    //发送缓冲区
void deal_rf_send_data(uint8_t* send_data, uint8_t len);
void ble_heart_task(void);
void user_dev_init(uint8_t main_type_id, uint8_t sub_type_id, uint8_t config_type);
void ble_uuid_init(uint16_t service_uuid, uint16_t write_uuid, uint16_t notify_uuid);
bool ble_name_init(char* name);
void ble_mac_init(uint8_t mac[6]);
bool ble_adv_data_init(char *adv_data, uint8_t adv_data_len);
bool ble_port_init(void);
void user_ble_name_init(void);

#define NVDATA_ID_BLE_MAC               0x70000001
#define NVDATA_ID_BLE_NAME              0x80000001
#define NVDATA_ID_BLE_PWR               0x90000001
#define NVDATA_ID_BLE_XTAL              0x90000004
#define NVDATA_ID_BLE_MODE              0x90000008
#define NVDATA_ID_BLE_CH                0x90000010
#define NVDATA_ID_BLE_SN                0x90000014

#define DEFAULT_MAC          "FF:FF:FF:FF:FF:FF"
#define DEFAULT_NAME         "CI_BLE"
#define DEFAULT_PWR          7
#define DEFAULT_XTAL         8 

bool updata_ble_mac(uint8_t *recv_data, uint8_t recv_len);
bool updata_ble_name(uint8_t *recv_data, uint8_t recv_len);
bool updata_ble_pwr(uint8_t recv_data);
bool updata_ble_xtal(uint8_t* recv_data, uint8_t recv_len);
bool updata_ble_mode(uint8_t recv_data);
bool updata_ble_ch(uint8_t* recv_data, uint8_t recv_len);
bool updata_ble_sn(uint8_t* recv_data, uint8_t recv_len);

void product_recv_task(void);
void at_msg_task(void);

//蓝牙收发数据的加密函数，pack_data为待加/解密数据，len为数据总长度
void cias_crypto_data(uint8_t* pack_data, uint8_t len); 

#endif /* #ifndef __EXE_API_H__ */


