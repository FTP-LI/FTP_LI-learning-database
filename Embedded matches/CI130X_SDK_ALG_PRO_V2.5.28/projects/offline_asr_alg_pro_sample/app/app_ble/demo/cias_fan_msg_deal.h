#ifndef __CIAS_FAN_MSG_H__
#define __CIAS_FAN_MSG_H__

//cmd_id
#define TURN_OFF               1001
#define TURN_ON                1002
#define SPEED_LOW              1003//一档风
#define SPEED_MIDDLE           1004//二档风
#define SPEED_HIGH             1005//三档风
#define SPEED_RAISE            1006//风速增大
#define SPEED_REDUCE           1007//风速减小
#define SPEED_MAX              1008//风速最大	
#define SPEED_MIN              1009//风速最小
#define SHAKE_OFF              1010//关闭摇头	
#define SHAKE_ON               1011//打开摇头	
#define SHAKE_LR_OFF           1012//关闭左右摇头	
#define SHAKE_LR_ON            1013//打开左右摇头	
#define SHAKE_HD_OFF           1014//关闭上下摇头	
#define SHAKE_HD_ON            1015//打开上下摇头	
#define NORMAL_OFF             1016//关闭正常风
#define NORMAL_ON              1017//打开正风
#define SLEEP_OFF              1018//关闭睡眠	
#define SLEEP_ON               1019//打开睡眠风	
#define NATURAL_OFF            1020//关闭自然风	
#define NATURAL_ON             1021//打开自然风	
#define ANION_OFF              1022//关闭负离子
#define ANION_ON               1023//开负离子	
#define TIMING_OFF             1024//关闭定时	
#define TIMMING_1H             1025//定时一小时	
#define TIMMING_2H             1026//定时二小时
#define TIMMING_3H             1027//定时三小时	
#define TIMMING_4H             1028//定时四小时	
#define TIMMING_5H             1029//定时五小时	
#define TIMMING_6H             1030//定时六小时
#define TIMMING_7H             1031//定时七小时	
#define TIMMING_8H             1032//定时八小时	
#define TIMMING_9H             1033//定时九小时	
#define TIMMING_10H            1034//定时十小时	
#define TIMMING_11H            1035//定时十一小时	
#define TIMMING_12H            1036//定时十二小时	
#define TIMMING_HARF           1037//定时半小时


//function_id
#define FAN_POWER              0x01    //开关
#define FAN_SPEED              0x02    //风速
#define FAN_SHAKE              0x03    //风向
#define FAN_MODE               0x04    //模式
#define FAN_ANION              0x07    //负离子
#define FAN_TIMING             0x08    //定时
#define FAN_SPEAKER            0x09    //播报
#define FAN_ASR                0x0A    //语音识别
#define FAN_TIMING_SET         0x0B    //定时范围设置
#define FAN_SPEED_SET          0x0C    //风速范围设置
#define FAN_MODE_SET           0x0D    //模式设置

#define DEV_SPEED_MAX          3       //最大风档位
#define DEV_SPEED_MIN          1       //最小风档位
#define DEV_TIMING_MAX         0xAC    //最大定时
#define DEV_TIMING_MIN         0xA1    //最小定时

#define FAN_MODE_NORMAL        0x03    //正常模式
#define FAN_MODE_SLEEP         0x04    //睡眠设置
#define FAN_MODE_NATURAL       0x05    //自然设置

typedef struct
{
    uint8_t power;
    uint8_t speed;
    uint8_t shake;
    uint8_t mode;
    uint8_t none1;      //暂留
    uint8_t none2;      //暂留
    uint8_t anion;
    uint8_t timing;
}fan_dev_t;

#endif