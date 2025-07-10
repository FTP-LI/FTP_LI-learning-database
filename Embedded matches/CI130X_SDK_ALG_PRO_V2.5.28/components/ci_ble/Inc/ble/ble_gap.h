/*============================================================================*/
/* @file ble_gap.h
 * @author luwei
 * @date 2020/02
 */

#ifndef __BLE_GAP_H__
#define __BLE_GAP_H__

/** @defgroup BLE_UUID_VALUES Assigned Values for BLE UUIDs
 * @{ */
/* Generic UUIDs, applicable to all services */
#define BLE_UUID_UNKNOWN                              0x0000 /**< Reserved UUID. */
#define BLE_UUID_SERVICE_PRIMARY                      0x2800 /**< Primary Service. */
#define BLE_UUID_SERVICE_SECONDARY                    0x2801 /**< Secondary Service. */
#define BLE_UUID_SERVICE_INCLUDE                      0x2802 /**< Include. */
#define BLE_UUID_CHARACTERISTIC                       0x2803 /**< Characteristic. */
#define BLE_UUID_DESCRIPTOR_CHAR_EXT_PROP             0x2900 /**< Characteristic Extended Properties Descriptor. */
#define BLE_UUID_DESCRIPTOR_CHAR_USER_DESC            0x2901 /**< Characteristic User Description Descriptor. */
#define BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG        0x2902 /**< Client Characteristic Configuration Descriptor. */
#define BLE_UUID_DESCRIPTOR_SERVER_CHAR_CONFIG        0x2903 /**< Server Characteristic Configuration Descriptor. */
#define BLE_UUID_DESCRIPTOR_CHAR_PRESENTATION_FORMAT  0x2904 /**< Characteristic Presentation Format Descriptor. */
#define BLE_UUID_DESCRIPTOR_CHAR_AGGREGATE_FORMAT     0x2905 /**< Characteristic Aggregate Format Descriptor. */
/* GATT specific UUIDs */
#define BLE_UUID_GATT                                 0x1801 /**< Generic Attribute Profile. */
#define BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED  0x2A05 /**< Service Changed Characteristic. */
/* GAP specific UUIDs */
#define BLE_UUID_GAP                                  0x1800 /**< Generic Access Profile. */
#define BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME       0x2A00 /**< Device Name Characteristic. */
#define BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE        0x2A01 /**< Appearance Characteristic. */
#define BLE_UUID_GAP_CHARACTERISTIC_RECONN_ADDR       0x2A03 /**< Reconnection Address Characteristic. */
#define BLE_UUID_GAP_CHARACTERISTIC_PPCP              0x2A04 /**< Peripheral Preferred Connection Parameters Characteristic. */
#define BLE_UUID_GAP_CHARACTERISTIC_CAR               0x2AA6 /**< Central Address Resolution Characteristic. */
#define BLE_UUID_GAP_CHARACTERISTIC_RPA_ONLY          0x2AC9 /**< Resolvable Private Address Only Characteristic. */
/** @} */

/** @defgroup BLE_APPEARANCES Bluetooth Appearance values
 *  @spec 
 * @{ */
#define BLE_APPEARANCE_UNKNOWN                                0 /**< Unknown. */
#define BLE_APPEARANCE_GENERIC_PHONE                         64 /**< Generic Phone. */
#define BLE_APPEARANCE_GENERIC_COMPUTER                     128 /**< Generic Computer. */
#define BLE_APPEARANCE_GENERIC_WATCH                        192 /**< Generic Watch. */
#define BLE_APPEARANCE_WATCH_SPORTS_WATCH                   193 /**< Watch: Sports Watch. */
#define BLE_APPEARANCE_GENERIC_CLOCK                        256 /**< Generic Clock. */
#define BLE_APPEARANCE_GENERIC_DISPLAY                      320 /**< Generic Display. */
#define BLE_APPEARANCE_GENERIC_REMOTE_CONTROL               384 /**< Generic Remote Control. */
#define BLE_APPEARANCE_GENERIC_EYE_GLASSES                  448 /**< Generic Eye-glasses. */
#define BLE_APPEARANCE_GENERIC_TAG                          512 /**< Generic Tag. */
#define BLE_APPEARANCE_GENERIC_KEYRING                      576 /**< Generic Keyring. */
#define BLE_APPEARANCE_GENERIC_MEDIA_PLAYER                 640 /**< Generic Media Player. */
#define BLE_APPEARANCE_GENERIC_BARCODE_SCANNER              704 /**< Generic Barcode Scanner. */
#define BLE_APPEARANCE_GENERIC_THERMOMETER                  768 /**< Generic Thermometer. */
#define BLE_APPEARANCE_THERMOMETER_EAR                      769 /**< Thermometer: Ear. */
#define BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR            832 /**< Generic Heart rate Sensor. */
#define BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT    833 /**< Heart Rate Sensor: Heart Rate Belt. */
#define BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE               896 /**< Generic Blood Pressure. */
#define BLE_APPEARANCE_BLOOD_PRESSURE_ARM                   897 /**< Blood Pressure: Arm. */
#define BLE_APPEARANCE_BLOOD_PRESSURE_WRIST                 898 /**< Blood Pressure: Wrist. */
#define BLE_APPEARANCE_GENERIC_HID                          960 /**< Human Interface Device (HID). */
#define BLE_APPEARANCE_HID_KEYBOARD                         961 /**< Keyboard (HID Subtype). */
#define BLE_APPEARANCE_HID_MOUSE                            962 /**< Mouse (HID Subtype). */
#define BLE_APPEARANCE_HID_JOYSTICK                         963 /**< Joystick (HID Subtype). */
#define BLE_APPEARANCE_HID_GAMEPAD                          964 /**< Gamepad (HID Subtype). */
#define BLE_APPEARANCE_HID_DIGITIZERSUBTYPE                 965 /**< Digitizer Tablet (HID Subtype). */
#define BLE_APPEARANCE_HID_CARD_READER                      966 /**< Card Reader (HID Subtype). */
#define BLE_APPEARANCE_HID_DIGITAL_PEN                      967 /**< Digital Pen (HID Subtype). */
#define BLE_APPEARANCE_HID_BARCODE                          968 /**< Barcode Scanner (HID Subtype). */
#define BLE_APPEARANCE_GENERIC_GLUCOSE_METER               1024 /**< Generic Glucose Meter. */
#define BLE_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR      1088 /**< Generic Running Walking Sensor. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE      1089 /**< Running Walking Sensor: In-Shoe. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE      1090 /**< Running Walking Sensor: On-Shoe. */
#define BLE_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP       1091 /**< Running Walking Sensor: On-Hip. */
#define BLE_APPEARANCE_GENERIC_CYCLING                     1152 /**< Generic Cycling. */
#define BLE_APPEARANCE_CYCLING_CYCLING_COMPUTER            1153 /**< Cycling: Cycling Computer. */
#define BLE_APPEARANCE_CYCLING_SPEED_SENSOR                1154 /**< Cycling: Speed Sensor. */
#define BLE_APPEARANCE_CYCLING_CADENCE_SENSOR              1155 /**< Cycling: Cadence Sensor. */
#define BLE_APPEARANCE_CYCLING_POWER_SENSOR                1156 /**< Cycling: Power Sensor. */
#define BLE_APPEARANCE_CYCLING_SPEED_CADENCE_SENSOR        1157 /**< Cycling: Speed and Cadence Sensor. */
#define BLE_APPEARANCE_GENERIC_PULSE_OXIMETER              3136 /**< Generic Pulse Oximeter. */
#define BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP            3137 /**< Fingertip (Pulse Oximeter subtype). */
#define BLE_APPEARANCE_PULSE_OXIMETER_WRIST_WORN           3138 /**< Wrist Worn(Pulse Oximeter subtype). */
#define BLE_APPEARANCE_GENERIC_WEIGHT_SCALE                3200 /**< Generic Weight Scale. */
#define BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT          5184 /**< Generic Outdoor Sports Activity. */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_DISP         5185 /**< Location Display Device (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_DISP 5186 /**< Location and Navigation Display Device (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_POD          5187 /**< Location Pod (Outdoor Sports Activity subtype). */
#define BLE_APPEARANCE_OUTDOOR_SPORTS_ACT_LOC_AND_NAV_POD  5188 /**< Location and Navigation Pod (Outdoor Sports Activity subtype). */
/** @} */

#endif /* #ifndef __BLE_GAP_H__ */

