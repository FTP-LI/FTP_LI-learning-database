#ifndef __CIAS_WARMER_MSG_H__
#define __CIAS_WARMER_MSG_H__

//cmd_id
#define TURN_OFF               1001
#define TURN_ON                1002
#define SHAKE_OFF              1003//关闭摇头	
#define SHAKE_ON               1004//打开摇头
#define SHAKE_LR_OFF           1005//关闭左右摇头	
#define SHAKE_LR_ON            1006//打开左右摇头	
#define SHAKE_HD_OFF           1007//关闭上下摇头	
#define SHAKE_HD_ON            1008//打开上下摇头	
#define ANION_OFF              1009//关闭负离子	
#define ANION_ON               1010//开负离子	
#define ENERGY_OFF             1011//关闭节能	
#define ENERGY_ON              1012//打开节能	
#define SLEEP_OFF              1013//关闭睡眠	
#define SLEEP_ON               1014//打开睡眠	
#define SCREEN_OFF             1015//关闭显示	
#define SCREEN_ON              1016//打开显示	
#define LIGHT_OFF              1017//关闭氛围灯	
#define LIGHT_ON               1018//打开氛围灯	
#define LOW_ON                 1019//打开低档	
#define MIDDLE_ON              1020//打开中档	
#define HIGH_ON                1021//打开高档	
#define DRY_OFF                1022//关闭干衣模式	
#define DRY_ON                 1023//打开干衣模式	
#define DISINFECTION_OFF       1024//关闭消毒	
#define DISINFECTION_ON        1025//打开消毒	
#define TIMING_OFF             1026//关闭定时	
#define TIMMING_1H             1027//定时一小时	
#define TIMMING_2H             1028//定时二小时
#define TIMMING_3H             1029//定时三小时	
#define TIMMING_4H             1030//定时四小时	
#define TIMMING_5H             1031//定时五小时	
#define TIMMING_6H             1032//定时六小时
#define TIMMING_7H             1033//定时七小时	
#define TIMMING_8H             1034//定时八小时	
#define TIMMING_9H             1035//定时九小时	
#define TIMMING_10H            1036//定时十小时	
#define TIMMING_11H            1037//定时十一小时	
#define TIMMING_12H            1038//定时十二小时	
#define TIMMING_HARF           1039//定时半小时
#define TEMPERATURE_15         1040
#define TEMPERATURE_16         1041
#define TEMPERATURE_17         1042
#define TEMPERATURE_18         1043
#define TEMPERATURE_19         1044
#define TEMPERATURE_20         1045
#define TEMPERATURE_21         1046
#define TEMPERATURE_22         1047
#define TEMPERATURE_23         1048
#define TEMPERATURE_24         1049
#define TEMPERATURE_25         1050
#define TEMPERATURE_26         1051
#define TEMPERATURE_27         1052
#define TEMPERATURE_28         1053
#define TEMPERATURE_29         1054
#define TEMPERATURE_30         1055
#define TEMPERATURE_31         1056
#define TEMPERATURE_32         1057
#define TEMPERATURE_33         1058
#define TEMPERATURE_34         1059
#define TEMPERATURE_35         1060
#define TEMPERATURE_36         1061
#define TEMPERATURE_37         1062
#define TEMPERATURE_38         1063
#define TEMPERATURE_39         1064
#define TEMPERATURE_40         1065
#define TEMPERATURE_41         1066
#define TEMPERATURE_42         1067
#define TEMPERATURE_43         1068
#define TEMPERATURE_44         1069
#define TEMPERATURE_45         1070
#define TEMPERATURE_RAISE      1071//升高温度	
#define TEMPERATURE_REDUCE     1072//降低温度	
#define TEMPERATURE_MAX        1073//最高温度	
#define TEMPERATURE_MIN        1074//最低温度	
#define HUMIDITY_OFF           1075//关闭加湿	
#define HUMIDITY_ON            1076//打开加湿	
#define HUMIDITY_40            1077//湿度四十	
#define HUMIDITY_45            1078
#define HUMIDITY_50            1079
#define HUMIDITY_55            1080
#define HUMIDITY_60            1081
#define HUMIDITY_65            1082
#define HUMIDITY_70            1083
#define HUMIDITY_75            1084
#define HUMIDITY_80            1085
#define HUMIDITY_RAISE         1086//升高湿度	
#define HUMIDITY_REDUCE        1087//降低湿度	
#define HUMIDITY_MAX           1088//最高湿度	
#define HUMIDITY_MIN           1089//最低湿度	
#define FLAME_OFF              1090//关闭火焰	
#define FLAME_ON               1091//打开火焰	
#define FLAME_COLORFUL         1092//七彩焰火	
#define FLAME_RED              1093//红色焰火	
#define FLAME_ORANGE           1094//橙色火焰	
#define FLAME_YELLOW           1095//黄色火焰	
#define FLAME_GREEN            1096//绿色火焰	
#define FLAME_CYAN             1097//青色焰火	
#define FLAME_BLUE             1098//蓝色火焰	
#define FLAME_PURPLE           1099//紫色火焰	

#define TIME_ON                10011//定时时间到

//function_id
#define WARMER_POWER                 0x01    //开关
#define WARMER_SHAKE                 0x02    //摇头
#define WARMER_ANION                 0x03    //负离子
#define WARMER_ENERGY                0x04    //节能
#define WARMER_SLEEP                 0x05    //睡眠
#define WARMER_SCREEN                0x06    //屏显
#define WARMER_LIGHT                 0x07    //氛围灯
#define WARMER_GEARS                 0x08    //档位
#define WARMER_DRY                   0x09    //干衣
#define WARMER_DISINFECTION          0x0A    //消毒
#define WARMER_TIMING                0x0B    //定时
#define WARMER_TEMPERATURE           0x0C    //温度
#define WARMER_HUMIDITY              0x0D    //湿度
#define WARMER_FLAME_SCREEN          0x0E    //屏显火焰

#define DEV_TEMPERATURE_MAX          45
#define DEV_TEMPERATURE_MIN          15
#define DEV_HUMIDITY_MAX             85
#define DEV_HUMIDITY_MIN             40

typedef struct
{
    uint8_t power;
    uint8_t shake;
    uint8_t anion;
    uint8_t energy;
    uint8_t sleep;
    uint8_t screen;
    uint8_t light;
    uint8_t gears;
    uint8_t dry;
    uint8_t disinfection;
    uint8_t timing;
    uint8_t tempreature;
    uint8_t humidity;
    uint8_t flame_screen;
}warmer_dev_t;




#endif