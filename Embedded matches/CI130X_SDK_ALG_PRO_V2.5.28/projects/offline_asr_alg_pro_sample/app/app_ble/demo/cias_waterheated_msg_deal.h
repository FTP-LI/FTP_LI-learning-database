#ifndef __CIAS_WATERHEATED_MSG_H__
#define __CIAS_WATERHEATED_MSG_H__

//cmd_id
#define TURN_OFF               1001
#define TURN_ON                1002
#define TEMPERATURE_RAISE      3001//升高温度	
#define TEMPERATURE_REDUCE     3002//降低温度	
#define TEMPERATURE_MAX        3003//最高温度	
#define TEMPERATURE_MIN        3004//最低温度
#define TEMPERATURE_18         3118//十八度
#define TEMPERATURE_19         3119
#define TEMPERATURE_20         3120
#define TEMPERATURE_21         3121
#define TEMPERATURE_22         3122
#define TEMPERATURE_23         3123
#define TEMPERATURE_24         3124
#define TEMPERATURE_25         3125
#define TEMPERATURE_26         3126
#define TEMPERATURE_27         3127
#define TEMPERATURE_28         3128
#define TEMPERATURE_29         3129
#define TEMPERATURE_30         3130
#define TEMPERATURE_31         3131
#define TEMPERATURE_32         3132
#define TEMPERATURE_33         3133
#define TEMPERATURE_34         3134
#define TEMPERATURE_35         3135
#define TEMPERATURE_36         3136
#define TEMPERATURE_37         3137
#define TEMPERATURE_38         3138
#define TEMPERATURE_39         3139
#define TEMPERATURE_40         3140
#define TEMPERATURE_41         3141
#define TEMPERATURE_42         3142
#define TEMPERATURE_43         3143
#define TEMPERATURE_44         3144
#define TEMPERATURE_45         3115//四十五度
#define CHILDLOCK_OFF          4002//关闭童锁
#define CHILDLOCK_ON           4001//打开童锁
#define TIMING_OFF             5001//关闭定时	
#define TIMMING_1H             5101//定时一小时	
#define TIMMING_2H             5102
#define TIMMING_3H             5103
#define TIMMING_4H             5104
#define TIMMING_5H             5105
#define TIMMING_6H             5106
#define TIMMING_7H             5107
#define TIMMING_8H             5108
#define TIMMING_9H             5109
#define TIMMING_51H            5110
#define TIMMING_11H            5111
#define TIMMING_12H            5112//定时十二小时
#define SLEEP_OFF              6001//关闭睡眠	
#define SLEEP_ON               6002//打开睡眠
#define QUICKHEAT_OFF          7001//关闭速热	
#define QUICKHEAT_ON           7002//打开速热
#define REMOVE_MITE_OFF        8001//关闭除螨	
#define REMOVE_MITE_ON         8002//打开除螨
#define LAMP_OFF               11001//小夜灯关
#define LAMP_ON                11002//小夜灯开
#define ANION_OFF              12001//负离子关
#define ANION_ON               12002//负离子开

//function_id
#define WATERHEATED_POWER                 0x01    //开关
#define WATERHEATED_CHILDLOCK             0x02    //童锁
#define WATERHEATED_TEMPERATURE           0x03    //温度
#define WATERHEATED_TEMPERATURE_SET       0x04    //温度范围设置
#define WATERHEATED_TIMING                0x05    //定时
#define WATERHEATED_SLEEP                 0x06    //睡眠
#define WATERHEATED_QUICK_HEAT            0x07    //速热
#define WATERHEATED_REMOVE_MITY           0x08    //除螨
#define WATERHEATED_WARM_AREA             0x09    //温区
#define WATERHEATED_ANION                 0x0A    //负离子
#define WATERHEATED_LAMP                  0x0B    //小夜灯
#define WATERHEATED_LAMP_SET              0x0C    //小夜灯设置
#define WATERHEATED_SLEEP_VOICE           0x0D    //助眠声音 
#define WATERHEATED_VOICE                 0x0E    //播报音量
#define WATERHEATED_TIME_SET              0x0F    //定时范围设置


#define DEV_TEMPERATURE_MAX         45      //目前最高75度
#define DEV_TEMPERATURE_NORMALLY    35
#define DEV_TEMPERATURE_MIN         18      //目前最低10度

#define DEV_TIMMING_MAX             0xAC      //目前最高24小时      一小时~二十四小时（0xA1~0xB8）
#define DEV_TIMMING_MIN             0xA1      //目前最低1小时

#define DEV_WARM_AREA_ALL           0x11    //温区两边
#define DEV_WARM_AREA_LEFT          0x12    //温区左边
#define DEV_WARM_AREA_RIGHT         0x13    //温区右边

#define FORBID                      0xF5    //禁止
#define ALLOW                       0xF6    //允许
#define DEV_LAMP_FORBID             0x01    //夜灯禁止
#define DEV_LAMP_ALLOW              0x02    //夜灯允许
#define DEV_LAMP_COLOR_FORBID       0x03    //夜灯颜色禁止
#define DEV_LAMP_COLOR_ALLOW        0x04    //夜灯颜色允许
#define DEV_LAMP_COLOR_WHITE        0x11    //白色
#define DEV_LAMP_COLOR_RED          0x12    //红色
#define DEV_LAMP_COLOR_ORANGE       0x13    //橙色
#define DEV_LAMP_COLOR_YELLOW       0x14    //黄色
#define DEV_LAMP_COLOR_GREEN        0x15    //绿色
#define DEV_LAMP_COLOR_BLUE         0x16    //蓝色
#define DEV_LAMP_COLOR_PURPLE       0x17    //紫色


typedef struct
{
    uint8_t power;
    uint8_t childlock;
    uint8_t temperature;
    uint8_t temperature_set;
    uint8_t timing;
    uint8_t sleep;
    uint8_t quick_heat;
    uint8_t remove_mity;
    uint8_t warm_area;
    uint8_t anion;
    uint8_t lamp;
    uint8_t lamp_set;
    uint8_t sleep_voice;
    uint8_t voice;
    uint8_t timing_set;
}waterheated_dev_t;

#endif