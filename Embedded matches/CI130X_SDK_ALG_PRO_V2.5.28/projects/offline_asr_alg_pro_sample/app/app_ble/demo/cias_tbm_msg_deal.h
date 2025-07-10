#ifndef __CIAS_TBM_MSG_DEAL_H__
#define __CIAS_TBM_MSG_DEAL_H__


//cmd_id
#define TURN_OFF                1001
#define TURN_ON                 1002
#define CLOSE_CHILD_LOCK        1003//关闭童锁
#define OPEN_CHILD_LOCK         1004//打开童锁
#define LEFT_STOP_FETCH_WATER   1005//左壶停止取水
#define STOP_INSULATION         1006//停止保温
#define STOP_COOLING            1007//停止制冷
#define LEFT_FETCH_WATER        1008//左壶取水
#define OPEN_INSULATION         1009//打开保温
#define OPEN_COOLING            1010//打开制冷
#define BOILING_WATER_OFF       1011//烧水关闭
#define RIGHT_STOP_FETCH_WATER  1012//右壶停止取水
#define STOP_FETCH_WATER        1013//停止取水
#define AUTO_WATER_BOILING      1014//自动烧水
#define RIGHT_FETCH_WATER       1015//右壶取水
#define KETTLE_HEATING          1016//水壶加热
#define TEMPERATURE_40          1017//温度调到四十度
#define TEMPERATURE_45          1018
#define TEMPERATURE_50          1019
#define TEMPERATURE_55          1020
#define TEMPERATURE_60          1021
#define TEMPERATURE_65          1022
#define TEMPERATURE_70          1023
#define TEMPERATURE_75          1024
#define TEMPERATURE_80          1025
#define TEMPERATURE_85          1026
#define TEMPERATURE_90          1027
#define TEMPERATURE_95          1028
#define TEMPERATURE_100         1029




#define TBM_POWER                0x01    //开关
#define TBM_CHILD                0x02    //童锁
#define TBM_LIEFT                0x03    //左壶
#define TBM_RIGHT                0x04    //右壶
#define TBM_TEMPERATURE          0x05    //调温
#define TBM_VOICE                0x06    //音量调节


#define DEV_TEMPERATURE_MAX               30
#define DEV_TEMPERATURE_MIN               16


typedef struct 
{
    uint8_t power;
    uint8_t child;
    uint8_t left_water;
    uint8_t left_heatperservation;
    uint8_t left_refrigeration;
    uint8_t right_autoBoilingWater;
    uint8_t right_water;
    uint8_t right_heat;
    uint8_t right_currentTmp;

}tbm_dev_t;


#endif // __CIAS_TBM_MSG_DEAL_H__