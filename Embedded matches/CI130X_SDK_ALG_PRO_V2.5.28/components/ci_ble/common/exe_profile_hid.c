/*============================================================================*/
/* @file exe_profile_hid.c
 * @brief EXE Profile layer, including DIS, BAS, HIDS, OTA etc.
 * @author onmicro
 * @date 2020/02
 */

#include <stdint.h>
#include <stdbool.h>
#include "ble_gatts.h"
#include "exe_hal.h"
#include "exe_api.h"
#include "exe_app.h"
#include "svn_rev.h"
#include "user_config.h"
#include "sdk_default_config.h"
#include "rf_msg_deal.h"

/* Profile built-in support. */


#if(PROFILE_CIAS_BLE)
uint16_t cias_service_uuid =  0;                        
#endif

#define PROFILE_SIG_GATT        1
#define PROFILE_SIG_DIS         0
#define PROFILE_SIG_BAS         0 //Redmi4X
#define PROFILE_SIG_HIDS        0
#define PROFILE_VENDOR_DATA     0
#define PROFILE_VENDOR_GAMEPAD  0
#define PROFILE_VENDOR_24G      0
#define PROFILE_VENDOR_OTA      0

/* HID device type built-in support. */
#define HID_PRODUCT_SHUTTER     0
#define HID_PRODUCT_MOUSE       0
#define HID_PRODUCT_KEYBOARD    0
#define HID_PRODUCT_JOYSTICK    0
#if (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_KEYBOARD)
#undef HID_PRODUCT_SHUTTER
#define HID_PRODUCT_SHUTTER     1
#elif (APP_GAP_APPEARANCE == BLE_APPEARANCE_HID_MOUSE)
#undef HID_PRODUCT_MOUSE
#define HID_PRODUCT_MOUSE       1
#else
#undef HID_PRODUCT_MOUSE
#define HID_PRODUCT_MOUSE       1
#endif
/* Vendor Report support for iOS. */
#define HID_REPORT_VENDOR_IN    0
#define HID_REPORT_VENDOR_OUT   1


///HID input/output report ID.
#define HID_REPORT_ID_MOUSE_INPUT                       1   /* Mouse input report ID */
#define HID_REPORT_ID_CONSUME_CONTROL_INPUT             2   /* Consumer Control input report ID */
#define HID_REPORT_ID_KEYBOARD_INPUT                    3   /* Keyboard input report ID */
#define HID_REPORT_ID_JOYSTICK_INPUT                    4   /* Joystick input report ID */
#define HID_REPORT_ID_KEYBOARD_OUT                      0   /* Keyboard/LED output report ID */
#define HID_REPORT_ID_FEATURE                           0   /* Feature report ID */
#define HID_REPORT_ID_VENDOR_IO                         17  /* Vendor-defined report ID */

/** @name Report Type values
 * @anchor BLE_HIDS_REPORT_TYPE @{
 */
// Report Type values
#define BLE_HIDS_REP_TYPE_INPUT                 1
#define BLE_HIDS_REP_TYPE_OUTPUT                2
#define BLE_HIDS_REP_TYPE_FEATURE               3
/** @} */

/**
 * @brief Abbr. in ATT database.
 * PS:  Primary Service, its value ={ServiceUUID}
 *  CD:  Char Declaration, its value ={Char Properties, Char Value Handle, Char UUID}
 *   CV:   Char Value
 *   CCCD: Client Char Config Descriptor, its value CCC=[notification | indication]
 *   RRD:  Report Reference Descriptor,   its value RR={Report ID, Report Type[in|out]}
 */
typedef enum
{
  ATT_HANDLE_BEGIN = 0,

  /* Generic Access. */
  ATT_HANDLE_GAP_PS,            //1800
    ATT_HANDLE_GAP_DEVNAME_CD,    //2A00: Read
    ATT_HANDLE_GAP_DEVNAME_CV,      //utf8s
	ATT_HANDLE_GAP_APPEARANCE_CD, //2A01: Read
	ATT_HANDLE_GAP_APPEARANCE_CV,
	ATT_HANDLE_GAP_PPCP_CD,       //2A04: Read
    ATT_HANDLE_GAP_PPCP_CV,

#if (PROFILE_SIG_GATT)
  /* Generic Attribute. */
  ATT_HANDLE_GATT_PS,	        //1801
    ATT_HANDLE_GATT_SCC_CD,	      //2A05: Indicate
	ATT_HANDLE_GATT_SCC_CV,
	ATT_HANDLE_GATT_SCC_CCCD,     //2902: CCC
#endif

#if (PROFILE_SIG_DIS)
  /* Device Information. */
  ATT_HANDLE_DIS_PS,            //180A
    ATT_HANDLE_DIS_PNP_CD,        //2A50: Read
    ATT_HANDLE_DIS_PNP_CV,
#endif

#if (PROFILE_SIG_BAS)
  /* Battery. */
  ATT_HANDLE_BAS_PS,            //1812
    ATT_HANDLE_BAS_BAT_LVL_CD,    //2A19: Read | Notify
    ATT_HANDLE_BAS_BAT_LVL_CV,
    ATT_HANDLE_BAS_BAT_LVL_CCCD,  //2902: CCC
#endif

#if (PROFILE_SIG_HIDS)
  /* Human Interface Device. */
  ATT_HANDLE_HID_PS,            //1812
    ATT_HANDLE_HID_PROTOCOL_MODE_CD,    //2A4E: Read | Write_noRSP
    ATT_HANDLE_HID_PROTOCOL_MODE_CV,

#if (HID_PRODUCT_SHUTTER)
    ATT_HANDLE_HID_CONSUMER_REPORT_CD,    //2A4D: Read | Notify
    ATT_HANDLE_HID_CONSUMER_REPORT_CV,
    ATT_HANDLE_HID_CONSUMER_REPORT_CCCD,    //2902: CCC
    ATT_HANDLE_HID_CONSUMER_REPORT_RRD,     //2908: ReportID + ReportType=in
#endif

#if (HID_PRODUCT_SHUTTER || HID_PRODUCT_KEYBOARD)
    ATT_HANDLE_HID_KEYBOARD_REPORT_IN_CD, //2A4D: Read | Notify
    ATT_HANDLE_HID_KEYBOARD_REPORT_IN_CV,
    ATT_HANDLE_HID_KEYBOARD_REPORT_IN_CCD,  //2902: CCC
    ATT_HANDLE_HID_KEYBOARD_REPORT_IN_RRD,  //2908: ReportID + ReportType=in
#endif

#if (HID_PRODUCT_MOUSE)
    ATT_HANDLE_HID_MOUSE_REPORT_CD,       //2A4D: Read | Notify
    ATT_HANDLE_HID_MOUSE_REPORT_CV,
    ATT_HANDLE_HID_MOUSE_REPORT_CCD,        //2902: CCC
    ATT_HANDLE_HID_MOUSE_REPORT_RRD,        //2908: ReportID + ReportType=in
#endif

#if (HID_PRODUCT_KEYBOARD)
    ATT_HANDLE_HID_KEYBOARD_REPORT_OUT_CD,//2A4D: Read | Write | Write_noRSP
    ATT_HANDLE_HID_KEYBOARD_REPORT_OUT_CV,
    ATT_HANDLE_HID_KEYBOARD_REPORT_OUT_RRD, //2908: ReportID + ReportType=out
#endif

#if (HID_PRODUCT_JOYSTICK)
    ATT_HANDLE_HID_JOYSTICK_REPORT_CD,    //2A4D: Read | Notify
    ATT_HANDLE_HID_JOYSTICK_REPORT_CV,
    ATT_HANDLE_HID_JOYSTICK_REPORT_CCCD,    //2902: CCC
    ATT_HANDLE_HID_JOYSTICK_REPORT_RRD,     //2908: ReportID + ReportType=in
#endif

#if (HID_REPORT_VENDOR_IN)
    ATT_HANDLE_HID_VENDOR_REPORT_IN_CD,     //2A4D: Read | Notify
    ATT_HANDLE_HID_VENDOR_REPORT_IN_CV,
    ATT_HANDLE_HID_VENDOR_REPORT_IN_CCD,      //2902: CCC
    ATT_HANDLE_HID_VENDOR_REPORT_IN_RRD,      //2908: ReportID + ReportType=in
#endif
#if (HID_REPORT_VENDOR_OUT)
    ATT_HANDLE_HID_VENDOR_REPORT_OUT_CD,    //2A4D: Read | Write | Write_noRSP
    ATT_HANDLE_HID_VENDOR_REPORT_OUT_CV,
    ATT_HANDLE_HID_VENDOR_REPORT_OUT_RRD,     //2908: ReportID + ReportType=out
#endif

    ATT_HANDLE_HID_REPORT_MAP_CD,         //2A4B: Read
    ATT_HANDLE_HID_REPORT_MAP_CV,           //USB HID Usage Tables

    ATT_HANDLE_HID_INFORMATION_CD,        //2A4A: Read
    ATT_HANDLE_HID_INFORMATION_CV,          //USB HID version,country,flags

    ATT_HANDLE_HID_CONTROL_POINT_CD,      //2A4C: Write_noRSP
    ATT_HANDLE_HID_CONTROL_POINT_CV,
#endif /* PROFILE_SIG_HIDS */

#if (PROFILE_VENDOR_GAMEPAD)
  /* Vendor GamePad Service. */
  ATT_HANDLE_VENDOR_GAMEPAD_PS, //UUID128
    ATT_HANDLE_GAMEPAD_INPUT_CD,  //Vendor: Read | Notify
    ATT_HANDLE_GAMEPAD_INPUT_CV,
    ATT_HANDLE_GAMEPAD_INPUT_CCCD,//2902: CCC

    ATT_HANDLE_GAMEPAD_CMDOUT_CD, //Vendor: Write
    ATT_HANDLE_GAMEPAD_CMDOUT_CV,
    ATT_HANDLE_GAMEPAD_DATOUT_CD, //Vendor: Write
    ATT_HANDLE_GAMEPAD_DATOUT_CV,
#endif

#if (PROFILE_VENDOR_24G)
  /* Vendor 2.4G Service. */
  ATT_HANDLE_VENDOR_24G_PS, //UUID128
    ATT_HANDLE_24G_TXRX_CD,  //FF01: Read | Write_noRSP | Notify
    ATT_HANDLE_24G_TXRX_CV,
    ATT_HANDLE_24G_TXRX_CCCD,//2902: CCC
#endif

#if (PROFILE_VENDOR_OTA)
  /* Vendor OTA Service. */
  ATT_HANDLE_VENDOR_OTA_PS, //UUID128
    ATT_HANDLE_OTA_RW_CD,     //Vendor: Read | Write_noRSP | Notify
    ATT_HANDLE_OTA_RW_CV,
    ATT_HANDLE_OTA_RW_CCCD,   //2902: CCC
#endif

#if (PROFILE_CIAS_BLE)
  /* Human Interface Device. */
  ATT_HANDLE_CIAS_PS,           

  ATT_HANDLE_CIAS_WRITE_CD,   
  ATT_HANDLE_CIAS_WRITE_CV,        

  ATT_HANDLE_CIAS_NOTIFY_CD,   
  ATT_HANDLE_CIAS_NOTIFY_CV,
  ATT_HANDLE_CIAS_NOTIFY_CCCD,    
 
#endif
  ATT_HANDLE_END,
} att_handles_set_t;

/* Service UUIDs. */
static const uint16_t _gap_service_uuid = BLE_UUID_GAP;                    //0x1800
static const uint16_t _gatt_service_uuid = BLE_UUID_GATT;                  //0x1801
static const uint16_t _dis_service_uuid = BLE_UUID_DEVICE_INFORMATION_SERVICE;     //0x180a
static const uint16_t _bas_service_uuid = BLE_UUID_BATTERY_SERVICE;                //0x180f
static const uint16_t _hids_service_uuid = BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE; //0x1812

/* Properites. */
static const uint8_t _properties_read = BLE_CHAR_PROP_READ;
static const uint8_t _properties_write = BLE_CHAR_PROP_WRITE;
static const uint8_t _properties_indicate = BLE_CHAR_PROP_INDICATE;
static const uint8_t _properties_writenak = BLE_CHAR_PROP_WRITE_WITHOUT_RSP;
static const uint8_t _properties_read_notify = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_NOTIFY;
static const uint8_t _properties_read_writenak = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE_WITHOUT_RSP;
static const uint8_t _properties_read_write_writenak = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE | BLE_CHAR_PROP_WRITE_WITHOUT_RSP;
static const uint8_t _properties_read_write = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE;
static const uint8_t _properties_read_writenak_notify = BLE_CHAR_PROP_READ | BLE_CHAR_PROP_WRITE_WITHOUT_RSP | BLE_CHAR_PROP_NOTIFY;

/* GAP & GATT */
static const uint16_t _gap_appearance_value = APP_GAP_APPEARANCE;
#if (PROFILE_SIG_GATT)
static uint16_t _gatt_service_changed_value[4] = {0};
static uint8_t _gatt_service_changed_ccc_value[2] = {0};
#endif

uint8_t profile_unified_ccc_value[2];

#if (PROFILE_SIG_HIDS)

// Protocol Mode values
#define PROTOCOL_MODE_BOOT               0x00                        /**< Boot Protocol Mode. */
#define PROTOCOL_MODE_REPORT             0x01                        /**< Report Protocol Mode. */
static uint8_t _hid_protocol_mode_value = PROTOCOL_MODE_REPORT;

static uint8_t _hid_control_point_value;

// Information Flags
#define HID_INFO_FLAG_REMOTE_WAKE_MSK           0x01
#define HID_INFO_FLAG_NORMALLY_CONNECTABLE_MSK  0x02
static const uint8_t _hid_information_value[] =
{
  U16_LO(0x0111), U16_HI(0x0111),             // bcdHID (USB HID version)
  0x00,                                       // bCountryCode
  0x01                                        // Flags
};

#if (HID_PRODUCT_SHUTTER || HID_PRODUCT_KEYBOARD)
static const uint8_t _hid_keyboard_in_report_ref_value[2] ={HID_REPORT_ID_KEYBOARD_INPUT, BLE_HIDS_REP_TYPE_INPUT};
#endif
///u8ControlKey, u8KeyCode
uint8_t hid_keyboard_in_report_value[2] = {0};

#if (HID_PRODUCT_KEYBOARD)
static const uint8_t _hid_keyboard_out_report_ref_value[2] = {HID_REPORT_ID_KEYBOARD_INPUT, BLE_HIDS_REP_TYPE_OUTPUT};
#endif
uint8_t hid_keyboard_out_report_value;

#if (HID_PRODUCT_SHUTTER)
static const uint8_t _hid_consumer_report_ref_value[2] = {HID_REPORT_ID_CONSUME_CONTROL_INPUT, BLE_HIDS_REP_TYPE_INPUT};
#endif
///u16Bitmap
uint8_t hid_consumer_report_value[2]; 

#if (HID_PRODUCT_MOUSE)
static const uint8_t _hid_mouse_report_ref_value[2] = {HID_REPORT_ID_MOUSE_INPUT, BLE_HIDS_REP_TYPE_INPUT};
#endif
///u8Button, s8X, s8Y, s8Wheel
uint8_t hid_mouse_report_value[4];

#if (HID_PRODUCT_JOYSTICK)
static const uint8_t _hid_joystick_report_ref_value[2] = {HID_REPORT_ID_JOYSTICK_INPUT, BLE_HIDS_REP_TYPE_INPUT};
#endif
///s8X, s8Y, s8Z, s8RZ, u8HatSwitch, u16Button, s18MouseX, s8MouseY
uint8_t hid_joystick_report_value[9];

#if (HID_REPORT_VENDOR_IN)
static const uint8_t _hid_vendor_in_report_ref_value[2] = {HID_REPORT_ID_VENDOR_IO, BLE_HIDS_REP_TYPE_INPUT};
uint8_t hid_vendor_report_value[19];
#endif
#if (HID_REPORT_VENDOR_OUT)
static const uint8_t _hid_vendor_out_report_ref_value[2] = {HID_REPORT_ID_VENDOR_IO, BLE_HIDS_REP_TYPE_OUTPUT};
#endif
#if !(HID_REPORT_VENDOR_IN) && (HID_REPORT_VENDOR_OUT)
uint8_t hid_vendor_report_value[1];
#endif

/* iOS和Android使用不同的reportMap，后者支持自定义的Report，是前者的超集. */
#if (HID_PRODUCT_SHUTTER)
#define REPORT_MAP_SHUTTER \
  0x05, 0x0C, /* Usage Page (Consumer) */ \
  0x09, 0x01, /* Usage (Consume Control) */ \
  0xA1, 0x01, /* Collection (Application) */ \
   0x85, HID_REPORT_ID_CONSUME_CONTROL_INPUT,  /* Report ID (2) */ \
   0x09, 0xE9,  /* Usage (Volume Increment) */ \
   0x09, 0xEA,  /* Usage (Volume Decrement) */ \
   0x09, 0xE2,  /* Usage (Mute) */ \
   0x09, 0x30,  /* Usage (Power) */ \
   0x15, 0x01,  /* Logical minimum (1) */ \
   0x25, 0x0C,  /* Logical maximum (12) */ \
   0x75, 0x10,  /* Report Size (16) */ \
   0x95, 0x01,  /* Report Count (1) */ \
   0x81, 0x00,  /* Input (Data,Array,Absolute,Bit Field): Power,Mute,Vol+,Vol- */ \
  0xC0,    /* End Collection */ \
  \
  0x05, 0x01, /* Usage Page (Generic Desktop) */ \
  0x09, 0x06, /* Usage (Keyboard) */ \
  0xA1, 0x01, /* Collection (Application) */ \
   0x85, HID_REPORT_ID_KEYBOARD_INPUT,  /* Report ID (3) */ \
   0x05, 0x07,  /* Usage Page (Keyboard) */ \
   0x19, 0xE0,  /* Usage Minimum (Keyboard Left Control) */ \
   0x29, 0xE7,  /* Usage Maximum (Keyboard Right Control) */ \
   0x15, 0x00,  /* Logical minimum (0) */ \
   0x25, 0x01,  /* Logical maximum (1) */ \
   0x75, 0x01,  /* Report Size (1) */ \
   0x95, 0x08,  /* Report Count (8) */ \
   0x81, 0x02,  /* Input (Data,Value,Absolute,Bit Field): Keyboard Right GUI,Alt,Shift,Control; Left GUI,Alt,Shift,Control */ \
   0x75, 0x08,  /* Report Size (8) */ \
   0x95, 0x01,  /* Report Count (1) */ \
   0x15, 0x00,  /* Logical minimum (0) */ \
   0x25, 0xF4,  /* Logical maximum (244) */ \
   0x05, 0x07,  /* Usage Page (Keyboard) */ \
   0x19, 0x00,  /* Usage Minimum (No event indicated) */ \
   0x29, 0xF4,  /* Usage Maximum (Reserved) */ \
   0x81, 0x00,  /* Input (Data,Array,Absolute,Bit Field): Keyboard keycode */ \
  0xC0,    /* End Collection */
#define REPORT_MAP_SHUTTER_SIZE 68
#else
#define REPORT_MAP_SHUTTER
#define REPORT_MAP_SHUTTER_SIZE 0
#endif /* #if (HID_PRODUCT_SHUTTER) */

#if (HID_PRODUCT_MOUSE)
#define REPORT_MAP_MOUSE \
  0x05, 0x01,  /* Usage Page (Generic Desktop) */ \
  0x09, 0x02,  /* Usage (Mouse) */ \
  0xA1, 0x01,  /* Collection (Application) */ \
   0x85, HID_REPORT_ID_MOUSE_INPUT,  /* Report Id (1) */ \
   0x09, 0x01,  /*	Usage (Pointer) */ \
   0xA1, 0x00,  /*	Collection (Physical) */ \
   0x05, 0x09,  /*	  Usage Page (Buttons) */ \
   0x19, 0x01,  /*	  Usage Minimum (01) - Button 1 */ \
   0x29, 0x03,  /*	  Usage Maximum (03) - Button 3 */ \
   0x15, 0x00,  /*	  Logical Minimum (0) */ \
   0x25, 0x01,  /*	  Logical Maximum (1) */ \
   0x75, 0x01,  /*	  Report Size (1) */ \
   0x95, 0x03,  /*	  Report Count (3) */ \
   0x81, 0x02,  /*	  Input (Data, Variable, Absolute) - Button2,1,0 */ \
   0x75, 0x05,  /*	  Report Size (5) */ \
   0x95, 0x01,  /*	  Report Count (1) */ \
   0x81, 0x01,  /*	  Input (Constant) - Padding or Reserved bits */ \
   0x05, 0x01,  /*	  Usage Page (Generic Desktop) */ \
   0x09, 0x30,  /*	  Usage (X) */ \
   0x09, 0x31,  /*	  Usage (Y) */ \
   0x09, 0x38,  /*	  Usage (Wheel) */ \
   0x15, 0x81,  /*	  Logical Minimum (-127) */ \
   0x25, 0x7F,  /*	  Logical Maximum (127) */ \
   0x75, 0x08,  /*	  Report Size (8) */ \
   0x95, 0x03,  /*	  Report Count (3) */ \
   0x81, 0x06,  /*	  Input (Data, Variable, Relative) - X,Y,Wheel */ \
  0xC0, 	   /*	End Collection */ \
  0xC0, 	   /* End Collection */
#define REPORT_MAP_MOUSE_SIZE 54
#else
#define REPORT_MAP_MOUSE
#define REPORT_MAP_MOUSE_SIZE 0
#endif /* #if (HID_PRODUCT_MOUSE) */

#if (HID_PRODUCT_KEYBOARD)
#define REPORT_MAP_KEYBOARD
#define REPORT_MAP_KEYBOARD_SIZE 0
#else
#define REPORT_MAP_KEYBOARD
#define REPORT_MAP_KEYBOARD_SIZE 0
#endif /* #if (HID_PRODUCT_KEYBOARD) */

#if (HID_PRODUCT_JOYSTICK)
#define REPORT_MAP_JOYSTICK
#define REPORT_MAP_JOYSTICK_SIZE 0
#else
#define REPORT_MAP_JOYSTICK
#define REPORT_MAP_JOYSTICK_SIZE 0
#endif /* #if (HID_PRODUCT_JOYSTICK) */

/* iOS allow no vendor usage in ReportMap after ReportID17 is declared in ReportReference. */
#if (HID_REPORT_VENDOR_IN) && (HID_REPORT_VENDOR_OUT)
/* iOS dislikes single ReportID in ReportMap. */
#define REPORT_MAP_VENDOR \
  0x06, 0x43,0xFF,  /* Usage Page (Vendor-defined 0xFF43) */ \
  0x0A, 0x02,0x02,  /* Usage (Vendor-defined 0x0202) */ \
  0xA1, 0x01,       /* Collection (Application) */ \
   0x85, HID_REPORT_ID_VENDOR_IO,  /* Report Id (17) */ \
   0x75, 0x08,      /*	  Report Size (8) */ \
   0x95, 0x13,      /*	  Report Count (19) */ \
   0x15, 0x00,      /*	  Logical Minimum (0) */ \
   0x26, 0xFF,0x00, /*	  Logical Maximum (255) */  \
   0x09, 0x02,      /*    Usage (Vendor-defined 0x0002) */ \
   0x81, 0x00,      /*	  Input (Data, Array, Absolute) */ \
   0x09, 0x02,      /*    Usage (Vendor-defined 0x0002) */ \
   0x91, 0x00,      /*	  Output (Data, Array, Absolute) */ \
  0xC0, 	   /* End Collection */
#define REPORT_MAP_VENDOR_SIZE 28
#else
#define REPORT_MAP_VENDOR
#define REPORT_MAP_VENDOR_SIZE 0
#endif
static const uint8_t _hid_report_map_value[] = {
  REPORT_MAP_SHUTTER
  REPORT_MAP_MOUSE
  REPORT_MAP_KEYBOARD
  REPORT_MAP_JOYSTICK
};



#endif /* #if (PROFILE_SIG_HIDS) */

#if (PROFILE_SIG_DIS)
#define USB_IF_VID            12994
#if defined(SVN_REV_NUM)
#define FW_VERSION_ID         SVN_REV_NUM
#else
#define FW_VERSION_ID         4268
#endif
static const uint8_t _dis_pnp_value[] = {
  0x02,
  USB_IF_VID & 0xff, USB_IF_VID >> 8, /* LSB */
  0x20, 0x62,
  FW_VERSION_ID & 0xff, FW_VERSION_ID >> 8, /* LSB */
};
#endif

uint8_t bas_bat_lvl_value = 100;

#if (PROFILE_VENDOR_GAMEPAD)
static const uint8_t _vendor_gamepad_service_uuid[16] = {0XAB,0X89,0X67,0X45,0X23,0X01,0X88,0X88,0x66,0x66,0x11,0x11,0x01,0x00,0x68,0x91};
#endif
uint8_t vendor_gamepad_report_value[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t vendor_gamepad_cmdout_value[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t vendor_gamepad_dataout_value[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


#if (PROFILE_VENDOR_OTA)
static const uint8_t _vendor_ota_service_uuid[16] = {0x11,0x19,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};
#endif
uint8_t vendor_ota_data_value[20];

#if (PROFILE_VENDOR_24G)
static const uint8_t _vendor_24g_service_uuid[2] = {0x00, 0xFF};
#endif
uint8_t vendor_24g_txrx_value[20];

/* UUID indexes for smaller code size. */
#define _UUID_IDX_PS                    0
#define _UUID_IDX_CD                    3
#define _UUID_IDX_CCCD                  4
#define _UUID_IDX_RRD                   6
#define _UUID_IDX_CHAR_GAP_DEVNAME      8
#define _UUID_IDX_CHAR_GAP_APPEARANCE   9
#define _UUID_IDX_CHAR_GAP_PPCP         10
#define _UUID_IDX_CHAR_GATT_SC          11
#define _UUID_IDX_CHAR_FF00             14
#define _UUID_IDX_CHAR_FF01             15
#define _UUID_IDX_CHAR_BAS_BAT          16
#define _UUID_IDX_CHAR_HID_INFO         17
#define _UUID_IDX_CHAR_HID_RMAP         18
#define _UUID_IDX_CHAR_HID_CPOINT       19
#define _UUID_IDX_CHAR_HID_REPORT       20
#define _UUID_IDX_CHAR_HID_PMODE        21
#define _UUID_IDX_CHAR_DIS_PNP          22

#if(PROFILE_CIAS_BLE)
#define _UUID_IDX_CIAS_WRITE            23
#define _UUID_IDX_CIAS_NOTIFY           24
#endif

uint16_t profile_uuids_set_sig[] = {
  //0
  BLE_UUID_SERVICE_PRIMARY,    //0x2800
  BLE_UUID_SERVICE_SECONDARY,  //0x2801
  BLE_UUID_SERVICE_INCLUDE,    //0x2802
  BLE_UUID_CHARACTERISTIC,     //0x2803
  BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG, //0x2902
  BLE_UUID_DESCRIPTOR_SERVER_CHAR_CONFIG, //0x2903
  BLE_UUID_REPORT_REF_DESCR,              //0x2908
  0,

  //8
  BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME,      //0x2A00
  BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE,       //0x2A01
  BLE_UUID_GAP_CHARACTERISTIC_PPCP,             //0x2A04
  BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED, //0x2A05
  0,
  0,
  0xFF00,
  0xFF01,

  //16
  BLE_UUID_BATTERY_LEVEL_CHAR,     //0x2A19
  BLE_UUID_HID_INFORMATION_CHAR,   //0x2A4A
  BLE_UUID_REPORT_MAP_CHAR,        //0x2A4B
  BLE_UUID_HID_CONTROL_POINT_CHAR, //0x2A4C
  BLE_UUID_REPORT_CHAR,            //0x2A4D
  BLE_UUID_PROTOCOL_MODE_CHAR,     //0x2A4E
  BLE_UUID_PNP_ID_CHAR,            //0x2A50
  //23
#if (PROFILE_CIAS_BLE)  
  0,                  
  0,            
#endif           
};

#define _UUID128_IDX_CHAR_GP_IN         0
#define _UUID128_IDX_CHAR_GP_CMDOUT     1
#define _UUID128_IDX_CHAR_GP_DATOUT     2
#define _UUID128_IDX_CHAR_OTA_DATA      3
const uint8_t profile_uuids_set_vendor[][16] = {
  {0XAB,0X89,0X67,0X45,0X23,0X01,0X88,0X88,0x66,0x66,0x11,0x11,0x03,0x00,0x68,0x91},
  {0XAB,0X89,0X67,0X45,0X23,0X01,0X88,0X88,0x66,0x66,0x11,0x11,0x02,0x00,0x68,0x91},
  {0XAB,0X89,0X67,0X45,0X23,0X01,0X88,0X88,0x66,0x66,0x11,0x11,0x04,0x00,0x68,0x91},
  {0x12,0x2B,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00},
};


const exe_att_ent_t exe_gtbl_gatt_database[] =
{
  /* The total number of ATT handlers. */
  {ATT_HANDLE_END-1, 0, 0, 0, 0},

  /* Generic Access PS: DevName CD,CV; Appearance CD,CV; PPCP CD,CV. */
  {7, 2, 2,                    _UUID_IDX_PS, (uint8_t*)(&_gap_service_uuid)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, EXE_ADV_DEV_NAME_LEN, _UUID_IDX_CHAR_GAP_DEVNAME, (uint8_t*)(exe_gbuf_adv_ind+EXE_ADV_DEV_NAME_OFF)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, 2,                    _UUID_IDX_CHAR_GAP_APPEARANCE, (uint8_t*)(&_gap_appearance_value)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, 8,                    _UUID_IDX_CHAR_GAP_PPCP, (uint8_t*)(&exe_gu16_gap_ppcp_value[0])},

#if (PROFILE_SIG_GATT)
  /* Generic Attribute PS: SCC CD,CV,CCCD. */
  {4, 2, 2,                    _UUID_IDX_PS, (uint8_t*)(&_gatt_service_uuid)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_indicate)},
  {0, 2, 4,                    _UUID_IDX_CHAR_GATT_SC, (uint8_t*)(_gatt_service_changed_value)},
  {0, 2, 2,                    _UUID_IDX_CCCD, (uint8_t*)(_gatt_service_changed_ccc_value)},
#endif

#if (PROFILE_SIG_DIS)
  /* Device Information PS: PnP CD,CV. */
  {3, 2, 2,                    _UUID_IDX_PS, (uint8_t*)(&_dis_service_uuid)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, sizeof(_dis_pnp_value),    _UUID_IDX_CHAR_DIS_PNP, (uint8_t*)(_dis_pnp_value)},
#endif

#if (PROFILE_SIG_BAS)
  /* Battery PS: BatteryLevel's CD,CV,CCCD. */
  {4, 2, 2,	 _UUID_IDX_PS, (uint8_t*)(&_bas_service_uuid)},
  {0, 2, 1,	 _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, 1,	 _UUID_IDX_CHAR_BAS_BAT, (uint8_t*)(&bas_bat_lvl_value)},
  {0, 2, 2,	 _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
#endif

#if (PROFILE_SIG_HIDS)
  /* Human Interface Device PS: ProtocolMode CD,CV; */
  {ATT_HANDLE_HID_CONTROL_POINT_CV - ATT_HANDLE_HID_PS + 1,
      2, 2,                    _UUID_IDX_PS, (uint8_t*)(&_hids_service_uuid)},
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_writenak)},
  {0, 2, sizeof(_hid_protocol_mode_value), _UUID_IDX_CHAR_HID_PMODE, (uint8_t*)(&_hid_protocol_mode_value)},

#if (HID_PRODUCT_SHUTTER)
  /* ConsumerControl's CD,CV,CCCD,RRD */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, 2,                    _UUID_IDX_CHAR_HID_REPORT, (uint8_t*)(&hid_consumer_report_value)},
  {0, 2, 2,                    _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
  {0, 2, 2,                    _UUID_IDX_RRD, (uint8_t*)(_hid_consumer_report_ref_value)},
#endif

#if (HID_PRODUCT_SHUTTER || HID_PRODUCT_KEYBOARD)
  /* KeyboardInput's CD,CV,CCCD,RRD */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, sizeof(hid_keyboard_in_report_value),  _UUID_IDX_CHAR_HID_REPORT, (uint8_t*)(hid_keyboard_in_report_value)},
  {0, 2, 2,                    _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
  {0, 2, 2,                    _UUID_IDX_RRD, (uint8_t*)(_hid_keyboard_in_report_ref_value)},
  
#endif

#if (HID_PRODUCT_MOUSE)
  /* Mouse's CD,CV,CCCD,RRD */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, sizeof(hid_mouse_report_value), _UUID_IDX_CHAR_HID_REPORT, (uint8_t*)(&hid_mouse_report_value)},
  {0, 2, 2,                    _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
  {0, 2, 2,                    _UUID_IDX_RRD, (uint8_t*)(_hid_mouse_report_ref_value)},
#endif

#if (HID_PRODUCT_KEYBOARD)
  /* KeyboardOutput's CD,CV,RRD */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_write_writenak)},
  {0, 2, sizeof(hid_keyboard_out_report_value), _UUID_IDX_CHAR_HID_REPORT, (uint8_t*)(&hid_keyboard_out_report_value)},
  {0, 2, 2,                    _UUID_IDX_RRD, (uint8_t*)(_hid_keyboard_out_report_ref_value)},
#endif

#if (HID_PRODUCT_JOYSTICK)
  /* Joystick's CD,CV,CCCD,RRD */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, sizeof(hid_joystick_report_value), _UUID_IDX_CHAR_HID_REPORT, (uint8_t*)(hid_joystick_report_value)},
  {0, 2, 2,                    _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
  {0, 2, 2,                    _UUID_IDX_RRD, (uint8_t*)(_hid_joystick_report_ref_value)},
#endif

#if (HID_REPORT_VENDOR_IN)
  /* VendorInput's CD,CV,CCCD,RRD */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, sizeof(hid_vendor_report_value), _UUID_IDX_CHAR_HID_REPORT, (uint8_t*)(&hid_vendor_report_value)},
  {0, 2, 2,                    _UUID_IDX_CCCD, (uint8_t*)(&profile_unified_ccc_value)},
  {0, 2, 2,                    _UUID_IDX_RRD, (uint8_t*)(_hid_vendor_in_report_ref_value)},
#endif
#if (HID_REPORT_VENDOR_OUT)
  /* VendorOutput's CD,CV,RRD */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read_write_writenak)},
  {0, 2, sizeof(hid_vendor_report_value), _UUID_IDX_CHAR_HID_REPORT, (uint8_t*)(&hid_vendor_report_value)},
  {0, 2, 2,                    _UUID_IDX_RRD, (uint8_t*)(_hid_vendor_out_report_ref_value)},
#endif

  /* ReportMap's CD,CV */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  /* Use hid_get_report_map_size() to get the true size on ATT_Find_Information_Request. */
  {0, 2, sizeof(_hid_report_map_value), _UUID_IDX_CHAR_HID_RMAP, (uint8_t*)(_hid_report_map_value)},

  /* HID Information's CD,CV */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_read)},
  {0, 2, sizeof(_hid_information_value), _UUID_IDX_CHAR_HID_INFO, (uint8_t*)(_hid_information_value)},

  /* HID ControlPoint's CD,CV */
  {0, 2, 1,                    _UUID_IDX_CD, (uint8_t*)(&_properties_writenak)},
  {0, 2, sizeof(_hid_control_point_value), _UUID_IDX_CHAR_HID_CPOINT, (uint8_t*)(&_hid_control_point_value)},
#endif /* PROFILE_SIG_HIDS */

#if (PROFILE_VENDOR_GAMEPAD)
  /* Vendor GamePad PS: Input's CD,CV,CCCD; CmdOut's CD,CV; DataOut's CD,CV */
  {8, 2, 16, _UUID_IDX_PS, (uint8_t*)(_vendor_gamepad_service_uuid)},
  {0, 2, 1,  _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0,16, sizeof(vendor_gamepad_report_value), _UUID128_IDX_CHAR_GP_IN, (uint8_t*)(vendor_gamepad_report_value)},
  {0, 2, 2,  _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
  {0, 2, 1,  _UUID_IDX_CD, (uint8_t*)(&_properties_write)},
  {0,16, sizeof(vendor_gamepad_cmdout_value), _UUID128_IDX_CHAR_GP_CMDOUT, (uint8_t*)(vendor_gamepad_cmdout_value)},
  {0, 2, 1,  _UUID_IDX_CD, (uint8_t*)(&_properties_write)},
  {0,16, sizeof(vendor_gamepad_dataout_value), _UUID128_IDX_CHAR_GP_DATOUT, (uint8_t*)(vendor_gamepad_dataout_value)},
#endif

#if (PROFILE_VENDOR_24G)
  /* Vendor 2.4G PS: TxRx's CD,CV,CCCD. */
  {4, 2, 2,  _UUID_IDX_PS, (uint8_t*)(_vendor_24g_service_uuid)},
  {0, 2, 1,  _UUID_IDX_CD, (uint8_t*)(&_properties_read_writenak_notify)},
  {0, 2, sizeof(vendor_24g_txrx_value), _UUID_IDX_CHAR_FF01, (uint8_t*)(vendor_24g_txrx_value)},
  {0, 2, 2,  _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
#endif

#if (PROFILE_VENDOR_OTA)
  /* Vendor OTA PS: RW's CD,CV,CCCD. */
  {4, 2, 16, _UUID_IDX_PS, (uint8_t*)(&_vendor_ota_service_uuid)},
  {0, 2, 1,	 _UUID_IDX_CD, (uint8_t*)(&_properties_read_writenak_notify)},
  {0,16, 20, _UUID128_IDX_CHAR_OTA_DATA, vendor_ota_data_value},
  {0, 2, 2,  _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
#endif

#if (PROFILE_CIAS_BLE)
  /* cias's CD,CV,CCCD. */
  {6, 2, 2,	 _UUID_IDX_PS, (uint8_t*)(&cias_service_uuid)},

  {0, 2, 1,	 _UUID_IDX_CD, (uint8_t*)(&_properties_writenak)},
  {0, 2, RF_RX_TX_MAX_LEN, _UUID_IDX_CIAS_WRITE, (uint8_t*)(&rf_recv_data)},

  {0, 2, 1,  _UUID_IDX_CD, (uint8_t*)(&_properties_read_notify)},
  {0, 2, RF_RX_TX_MAX_LEN, _UUID_IDX_CIAS_NOTIFY, (uint8_t*)(&rf_send_data)},
  {0, 2, 2, _UUID_IDX_CCCD, (uint8_t*)(profile_unified_ccc_value)},
#endif
};

/**
 * @brief 根据对方手机类型来使用HID用到的reportMap.
 *
 * @return reportMap.
 */
const uint8_t *hid_get_report_map(void)
{
#if (PROFILE_SIG_HIDS)
  return _hid_report_map_value;
#endif
  return 0;
}

/**
 * @brief 根据对方手机类型来使用HID用到的reportMap的大小.
 *
 * @return reportMap's size.
 */
int hid_get_report_map_size(void)
{
#if (PROFILE_SIG_HIDS)
  if (exe_gu8_phone_type == 2/*Android*/)
  { return sizeof(_hid_report_map_value); }
  else
  { return REPORT_MAP_SHUTTER_SIZE + REPORT_MAP_MOUSE_SIZE + REPORT_MAP_KEYBOARD_SIZE + REPORT_MAP_VENDOR_SIZE; }
#endif
    return 0;
}

/**
 * @brief 设置HID设备notify可用性。
 * @note API, 为提高兼容性，允许用户设置。remove later.
 *
 * @param [in] flag - true to enable notify; false to disable.
 */
void hid_notification_enable(uint8_t flag)
{
  profile_unified_ccc_value[0] = flag;
}

/**
 * @brief 得到HID设备notify可用性。
 *
 * @return CCC value.
 */
uint8_t hid_notification_is_enabled(void)
{
  return profile_unified_ccc_value[0];
}

bool hid_tx_notification(hid_data_type_t data_type)
{
  bool ret;
  switch (data_type) 
  {
#if (PROFILE_SIG_HIDS)
#if (HID_PRODUCT_SHUTTER || HID_PRODUCT_KEYBOARD)
  case HID_DATA_KEYBOARD:
    ret = exe_att_tx_notification(ATT_HANDLE_HID_KEYBOARD_REPORT_IN_CV, hid_keyboard_in_report_value, sizeof(hid_keyboard_in_report_value));
    break;
#endif

#if (HID_PRODUCT_SHUTTER)
  case HID_DATA_GAMEPAD:
    ret = exe_att_tx_notification(ATT_HANDLE_HID_CONSUMER_REPORT_CV, hid_consumer_report_value, sizeof(hid_consumer_report_value));
    break;
#endif

#if (HID_PRODUCT_MOUSE)
  case HID_DATA_MOUSE:
    ret = exe_att_tx_notification(ATT_HANDLE_HID_MOUSE_REPORT_CV, hid_mouse_report_value, sizeof(hid_mouse_report_value));
    break;
#endif

#if (HID_PRODUCT_JOYSTICK)
  case HID_DATA_JOYSTIC:
    ret = exe_att_tx_notification(ATT_HANDLE_HID_JOYSTICK_REPORT_CV, hid_joystick_report_value, sizeof(hid_joystick_report_value));
    break;
#endif
#endif
  
  default:
    return false;
  }
  return ret;
}

/**
 * @brief 通过notify句柄发送蓝牙设备协议数据到手机APP
 *需要发送的参数里面，ATT_HANDLE_CIAS_NOTIFY_CV为定义好的notify句柄，cias_iot_send为发送缓冲区，最后为数据长度
 * 
 */
bool cias_tx_notification()
{
  bool ret;
 // mprintf("ble send data:%d \r\n",rf_send_len);
  ret = exe_att_tx_notification(ATT_HANDLE_CIAS_NOTIFY_CV, rf_send_data, rf_send_len);
  return ret;
}