#ifndef __CIAS_AIRCONDITION_MSG_H__
#define __CIAS_AIRCONDITION_MSG_H__

//cmd_id
#define TURN_OFF               1001
#define TURN_ON                1002
#define MODE_COOL              1003//制冷模式
#define MODE_HEAT              1004//制热模式
#define MODE_FAN               1005//送风模式
#define MODE_DEHUMIDIFY        1006//除湿模式
#define MODE_AUTO              1007//自动模式
#define SWING_LR               1008//左右扫风
#define SWING_HD               1009//上下扫风
#define SWING_AUTO             1010//自动扫风
#define SWING_LOW              1011//低速风
#define SWING_MIDDLE           1012//中速风
#define SWING_HIGH             1013//高速风
#define SWING_VERY_HIGH        1014//强劲风
#define TEMPERATURE_16         1015
#define TEMPERATURE_17         1016
#define TEMPERATURE_18         1017
#define TEMPERATURE_19         1018
#define TEMPERATURE_20         1019
#define TEMPERATURE_21         1020
#define TEMPERATURE_22         1021
#define TEMPERATURE_23         1022
#define TEMPERATURE_24         1023
#define TEMPERATURE_25         1024
#define TEMPERATURE_26         1025
#define TEMPERATURE_27         1026
#define TEMPERATURE_28         1027
#define TEMPERATURE_29         1028
#define TEMPERATURE_30         1029
#define TEMPERATURE_RAISE      1030//升高温度	
#define TEMPERATURE_REDUCE     1031//降低温度	
#define TEMPERATURE_MAX        1032//最高温度	
#define TEMPERATURE_MIN        1033//最低温度	
#define LIGHT_OFF              1034//关闭灯光
#define LIGHT_ON               1035//打开灯光
#define HEALTH_OFF             1036//关闭健康
#define HEALTH_ON              1037//打开健康
#define SLEEP_OFF              1038//关闭睡眠	
#define SLEEP_ON               1039//打开睡眠	




//function_id
#define AIRCONDITION_POWER                0x01    //开关
#define AIRCONDITION_MODE                 0x02    //模式
#define AIRCONDITION_TEMPERATURE          0x04    //温度
#define AIRCONDITION_LIGHT                0x05    //灯光
#define AIRCONDITION_HEALTH               0x06    //健康
#define AIRCONDITION_SLEEP                0x07    //睡眠
#define AIRCONDITION_SPEED                0x09    //风速
#define AIRCONDITION_STRONG               0x0A    //强劲风
#define AIRCONDITION_SWING                0x0B    //风向

#define DEV_TEMPERATURE_MAX               30
#define DEV_TEMPERATURE_MIN               16

typedef struct
{
    uint8_t power;
    uint8_t mode;
    uint8_t none;           //留空
    uint8_t tempreature;
    uint8_t light;
    uint8_t health;
    uint8_t sleep;
    uint8_t speaker;
    uint8_t speed;
    uint8_t strong;
    uint8_t swing;
    uint8_t timing;
}aircondition_dev_t;

#endif