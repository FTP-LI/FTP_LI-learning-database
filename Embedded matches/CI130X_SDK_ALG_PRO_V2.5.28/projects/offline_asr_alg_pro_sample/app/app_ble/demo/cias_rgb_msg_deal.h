#ifndef __CIAS_RGB_MSG_H__
#define __CIAS_RGB_MSG_H__

//cmd_id
#define TURN_OFF               48//关灯控
#define TURN_ON                49//打灯控
#define RGB_BRIGHTNESS_UP      50//亮一点
#define RGB_BRIGHTNESS_DOWN    51//暗一点
#define RGB_BRIGHTNESS_MIX     52//最高亮度
#define RGB_BRIGHTNESS_MIN     53//最低亮度
#define RGB_BRIGHTNESS_MID     54//中等亮度
#define RGB_WHITE_COLOR        55//白色灯光
#define RGB_ORANGE_COLOR       56//橙色灯光
#define RGB_RED_COLOR          57//红色灯光
#define RGB_PURPLE_COLOR       58//紫色灯光
#define RGB_BLUE_COLOR         59//蓝色灯光
#define RGB_GREEN_COLOR        60//绿色灯光
#define RGB_YELLOW_COLOR       61//黄色灯光
#define RGB_MUSIC_MODE         62//音乐律动
#define TURN_OFF_TIMING        63//关闭定时
#define TIMMING_1H             64//定时一小时
#define TIMMING_2H             65//定时二小时
#define TIMMING_3H             66//定时三小时
#define TIMMING_4H             67//定时四小时
#define TIMMING_5H             68//定时五小时
#define TIMMING_6H             69//定时六小时
#define TIMMING_7H             70//定时七小时
#define TIMMING_8H             71//定时八小时
#define TIMMING_9H             72//定时九小时
#define TIMMING_10H            73//定时十小时
#define TIMMING_11H            74//定时十一小时
#define TIMMING_12H            75//定时十二小时
#define TIMMING_HARF           76//定时半小时

#define MAX_VOLUME             87//最大音量	
#define MIN_VOLUME             90//最小音量
#define VOLUME_UP              79//音量增大
#define VOLUME_DOWN            83//音量减小
#define TURN_ON_VOICE          93//开启语音
#define TURN_OFF_VOICE         96//关闭语音


//function_id
#define RGB_POWER                 0x01    //开关
#define RGB_BRIGHTNESS            0x02    //亮度
#define RGB_PROPERTY_MODE         0x03    //属性模式
#define RGB_TIMING                0x04    //定时
#define RGB_VOICE                 0x05    //音量

#define DEV_BRIGHTNESS_MAX        100
#define DEV_BRIGHTNESS_MID        50
#define DEV_BRIGHTNESS_MIN        10
#define DEV_VOICE_MAX             1
#define DEV_VOICE_MIN             7


typedef struct
{
    uint8_t power;      //uint8_t rgb_switch_status;
    int8_t  brightness; //int8_t duty_lasted;
    uint8_t red_value;
    uint8_t green_value;
    uint8_t blue_value;
    uint8_t e_value;
    uint8_t s_value;
    uint8_t property_mode;
    uint8_t timing;
}rgb_dev_t;




#endif