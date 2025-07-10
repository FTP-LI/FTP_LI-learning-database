#ifndef __CIAS_HEATTABLE_MSG_H__
#define __CIAS_HEATTABLE_MSG_H__

//cmd_id
#define TURN_OFF               1001
#define TURN_ON                1002
#define FRONT_GEAR_OFF         1003//前取暖关
#define FRONT_GEAR_ON          1004//前取暖开
#define FRONT_GEAR_1           1005//前取暖一档
#define FRONT_GEAR_2           1006//前取暖二档
#define FRONT_GEAR_3           1007//前取暖三档
#define FRONT_GEAR_4           1008//前取暖四档	
#define FRONT_GEAR_5           1009//前取暖五档
#define BACK_GEAR_OFF          1010//后取暖关
#define BACK_GEAR_ON           1011//后取暖开
#define BACK_GEAR_1            1012//后取暖一档
#define BACK_GEAR_2            1013//后取暖二档
#define BACK_GEAR_3            1014//后取暖三档
#define BACK_GEAR_4            1015//后取暖四档	
#define BACK_GEAR_5            1016//后取暖五档
#define LEFT_GEAR_OFF          1017//左取暖关
#define LEFT_GEAR_ON           1018//左取暖开
#define LEFT_GEAR_1            1019//左取暖一档
#define LEFT_GEAR_2            1020//左取暖二档
#define LEFT_GEAR_3            1021//左取暖三档
#define LEFT_GEAR_4            1022//左取暖四档	
#define LEFT_GEAR_5            1023//左取暖五档
#define RIGHT_GEAR_OFF         1024//右取暖关
#define RIGHT_GEAR_ON          1025//右取暖开
#define RIGHT_GEAR_1           1026//右取暖一档
#define RIGHT_GEAR_2           1027//右取暖二档
#define RIGHT_GEAR_3           1028//右取暖三档
#define RIGHT_GEAR_4           1029//右取暖四档	
#define RIGHT_GEAR_5           1030//右取暖五档
#define DOWN_GEAR_OFF          1031//下层取暖关
#define DOWN_GEAR_ON           1032//下层取暖开
#define DOWN_GEAR_1            1033//下层取暖一档
#define DOWN_GEAR_2            1034//下层取暖二档
#define DOWN_GEAR_3            1035//下层取暖三档
#define DOWN_GEAR_4            1036//下层取暖四档	
#define DOWN_GEAR_5            1037//下层取暖五档
#define WARMER_GEAR_OFF        1038//保暖关
#define WARMER_GEAR_ON         1039//保暖开
#define WARMER_GEAR_1          1040//保暖一档
#define WARMER_GEAR_2          1041//保暖二档
#define WARMER_GEAR_3          1042//保暖三档
#define TEMPERATURE_40         1043//温度四十	
#define TEMPERATURE_45         1044
#define TEMPERATURE_50         1045
#define TEMPERATURE_55         1046
#define TEMPERATURE_60         1047
#define TEMPERATURE_65         1048
#define TEMPERATURE_70         1049
#define TEMPERATURE_RAISE      1050//升高温度	
#define TEMPERATURE_REDUCE     1051//降低温度	
#define TEMPERATURE_MAX        1052//最高温度	
#define TEMPERATURE_MIN        1053//最低温度
#define DESKTOP_PAUSED         1054//桌面停止
#define DESKTOP_RAISE          1055//桌面升高	
#define DESKTOP_REDUCE         1056//桌面降低
#define MID_DESKTOP_PAUSED     1057//中层停止
#define MID_DESKTOP_RAISE      1058//中层升高	
#define MID_DESKTOP_REDUCE     1059//中层降低
#define DOWN_DESKTOP_PAUSED    1060//下层停止
#define DOWN_DESKTOP_RAISE     1061//下层升高	
#define DOWN_DESKTOP_REDUCE    1062//下层降低
#define ALL_GEAR_1             1063//取暖一档
#define ALL_GEAR_2             1064//取暖二档
#define ALL_GEAR_3             1065//取暖三档
#define ALL_GEAR_4             1066//取暖四档	
#define ALL_GEAR_5             1067//取暖五档

#define VOICE_MAX              2000//最大音量	
#define VOICE_MIN              2001//最小音量
#define VOICE_UP               2002//音量增大	
#define VOICE_DOWN             2003//音量减小	
#define VOICE_ON               2004//开启语音	
#define VOICE_OFF              2005//关闭语音

//function_id
#define HEATTABLE_POWER     0x01    //开关
#define HEATTABLE_FRONT     0x02    //前取暖
#define HEATTABLE_BACK      0x03    //后取暖
#define HEATTABLE_LEFT      0x04    //左取暖
#define HEATTABLE_RIGHT     0x05    //右取暖
#define HEATTABLE_DOWN      0x06    //下取暖
#define HEATTABLE_WARMER    0x07    //保暖
#define HEATTABLE_TEMPERATURE 0x08    //温度
#define HEATTABLE_DESKTOP   0x09    //调高
#define HEATTABLE_ALL       0x0A    //全控取暖
#define HEATTABLE_VOICE     0x0B    //音量

#define DEV_TEMPERATURE_MAX    70
#define DEV_TEMPERATURE_MIN    40

#define DEV_GEAR_MAX           0x15
#define DEV_GEAR_MIN           0x11

typedef struct
{
    uint8_t power;
    uint8_t front_gear;
    uint8_t back_gear;
    uint8_t left_gear;
    uint8_t right_gear;
    uint8_t down_gear;
    uint8_t warmer_gear;
    uint8_t tempreature;
    uint8_t desktop;
    uint8_t timing;
}heattable_dev_t;

#endif