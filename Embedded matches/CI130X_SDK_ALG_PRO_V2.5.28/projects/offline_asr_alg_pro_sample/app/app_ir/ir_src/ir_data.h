#ifndef _IR_DATA_H
#define _IR_DATA_H

#include "stdbool.h"
#include "ir_remote_driver.h"

#define TEST_NUM  (0)
#define MAX_IR_CODE_LENGTH (100 + 4*TEST_NUM) //用于存储压缩后的编码的长度8*100
#define MAX_DATA_TYPE (31 - TEST_NUM)       //最大不能超过31


/* 是否开启串口功能 */
// #define USE_UART_COMMUNIT


typedef enum
{
    IR_START_RECEIVE_LEVEL = 0,       //接收红外数据,调整空调状态
    IR_STOP_RECEIVE_LEVEL,            //停止数据接收
    IR_START_AIR_KEY_MATCH,           //开始匹配空调
    IR_START_AIR_KEY_MATCH_FROM_UART, //开始匹配空调
    IR_STOP_AIR_KEY_MATCH,            //停止匹配空调
    IR_START_KEY_LEARN,               //开始学码
    IR_START_KEY_LEARN_FROM_UART,     //开始学码
    IR_STOP_KEY_LEARN,                //停止学码
    FIND_AIR_STATUS,                  //获取当前空调状态
    FIND_AIR_CODE_INDEX,              //获取当前匹配到的空调红外码
    SEND_TV_IR_LEARND,                //发送学习到的红外码数据
    SEND_AIR_IR_CMD,                  //根据命令发送空调红外码
    SEND_IR_LEVLE_TEST,               //发送红外数据,测试使用
    IR_DEAL_SERCH_AIR_CALLBACK,       //搜索空调CALLBACK
    IR_SERCH_AIR_BRAND,               //搜索空调品牌
    IR_STOP_SERCH_AIR_BRAND,          //停止搜索空调品牌
    IR_SERCH_AIR_INDEX,               //搜索空调型号
    IR_STOP_SERCH_AIR_INDEX,          //停止搜索空调型号
} ir_ctrl_cmd_t;

typedef enum __eAirSearchCbType
{
    SEARCH_CB_TYPE_ID,//已发码，返回ID
    SEARCH_CB_TYPE_AUTO_OVER_LOOP,//搜索一遍了，已退出
    SEARCH_CB_TYPE_ASR_STOP_LOOP,//语音识别发命令词退出
}eAirSearchCbType;



typedef struct
{
    int send_cnt;//每次发送一个空调，目前最低是3个
    int timeout_ms;//每隔多长时间发送一次，最低3000MS
    int (*ir_search_send_callback)(eAirSearchCbType CbType, int air_code_id);  //发送按键后调用回调，例如是否搜索一圈，告诉现在的ID是多少
} ir_search_ctl;


typedef struct
{
    unsigned short H_level; //高电平
    unsigned short L_level; //低电平
} tv_ir_level_data_t;


typedef struct
{
    tv_ir_level_data_t code_info[MAX_DATA_TYPE]; //对应的红外电平信息
    unsigned char data_code[MAX_IR_CODE_LENGTH]; //压缩后的编码
    unsigned int bits_length;                    //编码的bit长度
} IrDataCode, ir_data_t;

typedef enum eAir_Brand
{
    BRAND_LG = 0,       //LG
    BRAND_GREE,         //格力
    BRAND_MEDIA,        //美的
    BRAND_AUX,          //奥克斯
    BRAND_HAIER,        //海尔
    BRAND_CHANGHONG,    //长虹
    BRAND_CHIGO,        //志高
    BRAND_TCL,          //TCL
    BRAND_HISENSE,      //海信
    BRAND_PANASONIC,    //松下
    BRAND_GREE_MEDIA,   //格力-美的-奥克斯-海尔-长虹
    BRAND_HITACHI,      //日立
    BRAND_DAIKIN,       //大金
    BRAND_MITSUBISHI,   //三菱重工、三菱电机
    BRAND_MI,           //小米
    BRAND_WHIRLPOOL,    //惠而浦

    //20210525 添加
    BRAND_GALANZ,       //格兰仕
    BRAND_FUJITSU,      //富士通
    BRAND_SANSHUI,      //山水
    BRAND_YORK,         //约克
    BRAND_SKYWORTH,     //创维
    BRAND_SHINCO,       //新科
    BRAND_CHUNLAN,      //春兰
    BRAND_CHEBLO,       //樱花
    BRAND_SAMSUNG,      //三星
    BRAND_AUCMA,        //澳柯玛
    BRAND_XINFEI,       //新飞
    BRAND_TOSHIBA,      //东芝
    BRAND_SAMPO,        //声宝
    BRAND_YUETU,        //月兔
    BRAND_YAIR,         //扬子
    BRAND_AMOI,         //夏新
    BRAND_SHARP,        //夏普
    BRAND_KAJ,          //康佳
    BRAND_RONGSHIDA,    //荣事达
    BRAND_TOYO,
    BRAND_MAX,
}eAirBrand;


typedef struct
{
    ir_data_t *key_data;
    unsigned int file_data;
}stUartLearnData;

typedef enum
{
    IR_SEND_OK = 0,             //开始发送
    IR_SEND_ERR = -1,           //错误事件

} ir_send_event_t;

typedef void (*ir_send_callback_t)(ir_send_event_t event);


#pragma pack(1)

typedef struct
{
    unsigned char onoff : 2;       //开关状态 开关:0-1; 0->关, 1->开
    unsigned char timer_onoff : 2; //开关状态 开关:0-1; 0->关, 1->开
    unsigned char timer : 4;       //定时:0-15 小时
    unsigned char mode : 4;        //模式:0-4; 0->自动,1->制冷, 2->制热,3->抽湿, 4->送风
    unsigned char temperature : 8; //温度:00-10; 00->16度, 0F->31, 10->32度
    unsigned char fan_rate : 4;    //风量:0-3; 0->自动,1->低, 2->中, 3->高
    unsigned char fan_dir_ver : 2;     //风向:0-3 位置:0->关闭，1->上下扫风
    unsigned char fan_dir_hor : 2; //风向:0-3 位置:0->关闭，1->左右扫风
    unsigned char sleep : 4;       //睡眠:0-4 0->关闭，1->睡眠1（睡眠开）,2->睡眠2，3->睡眠3，4->睡眠4, 0xF 不支持

} normal_status_t;

typedef struct
{
    unsigned char air_clean_vaild : 1;    //空气清新 是否有效：0 无效， 1 有效
    unsigned char elec_heat_vaild : 1;    //电加热 是否有效：0 无效， 1 有效
    unsigned char save_power_vaild : 1;   //节能 是否有效：0 无效， 1 有效
    unsigned char health_mode_vaild : 1;  //健康 是否有效：0 无效， 1 有效
    unsigned char dig_display_vaild : 1;  //数显 是否有效：0 无效， 1 有效
    unsigned char mute_vaild : 1;         //静音 是否有效：0 无效， 1 有效
    unsigned char fan_powerful_vaild : 1; //强劲风 是否有效：0 无效， 1 有效
    unsigned char body_feel_vaild : 1;    //体感 是否有效：0 无效， 1 有效
    unsigned char fresh_fan_vaild : 1;    //新风 是否有效：0 无效， 1 有效
    unsigned char fan_to_man_vaild : 1;   //风对人 是否有效：0 无效， 1 有效

} special_function_t;

typedef struct
{
    special_function_t support_list; //特殊功能支持列表
    unsigned char air_clean : 2;     //空气清新 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char elec_heat : 2;     //电加热 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char save_power : 2;    //节能 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char health_mode : 2;   //健康 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char dig_display : 2;   //数显 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char mute : 2;          //静音 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char fan_powerful : 2;  //强劲风 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char body_feel : 2;     //体感 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char fresh_fan : 2;     //新风 0->3 0 关闭 ， 1 打开，  2->3 保留
    unsigned char fan_to_man : 2;    //风吹人 0->3 0 关闭 ， 1 打开，  2->3 保留

} special_status_t;

typedef struct
{
    normal_status_t normal_sta;   //通用状态
    special_status_t special_sta; //特殊状态

} airc_status_t;
#pragma pack()


/**
 * @brief 红外初始化
 *
 * @param pIrPinInfo 初始化参数
 * @return int RETURN_ERR 初始化失败，RETURN_OK 初始化成功
 */
int ir_init(stIrPinInfo *pIrPinInfo);

/**
 * @brief ir_data_learn_ctl
 *
 * @param cmd
 *  开始学码，val=NULL 默认超时时间，val!=NULL, 超时时间设置为该地址的数据
    开始匹配，val=NULL 默认超时时间，val!=NULL, 超时时间设置为该地址的数据
    结束，&0
    查询状态，&值
    查询IRCODE,&值
 * @param val
 * @return int
 */
int ir_data_learn_ctl(ir_ctrl_cmd_t cmd, int *val);

/**
 * @brief 学码发送红外
 *
 * @param key_id 按键ID
 * @return int RETURN_OK 发送成功  RETURN_ERR 发送失败
 */
int ir_Send_IR(ir_data_t* pIr_Key);

/*空调控制*/
/*
* 返回值：返回值是相应的控制命令
* 参数1:空调码库ID
* 参数2:空调控制命令
    打开空调    ： 5
    关闭空调    ： 6
    空调高速风 ： 7
    空调中速风 ： 8
    空调低速风 ： 9
    自动风速    ： 10
    停止扫风    ： 11
    开启扫风    ： 12
    十九度   ： 13
    二十度   ： 14
    二十一度    ： 15
    二十二度    ： 16
    二十三度    ： 17
    二十四度    ： 18
    二十五度    ： 19
    二十六度    ： 20
    二十七度    ： 21
    二十八度    ： 22
    二十九度    ： 23
    三十度   ： 24
    制冷模式    ： 25
    制热模式    ： 26
    送风模式    ： 27
    除湿模式    ： 28
    自动模式    ： 29
    十六度   ： 30
    十七度   ： 31
    十八度   ： 32
    上下扫风    ： 68
    左右扫风    ： 69
    停止上下扫风  ： 70
    停止左右扫风  ： 71
    升高温度    ： 72
    降低温度    ： 73
    增加风速    ： 74
    减小风速    ： 75

    关闭睡眠    :  200
    睡眠模式1   :  201
    睡眠模式2   :  202
    睡眠模式3   :  203
    打开空气清新   :  205
    关闭空气清新   :  204
    打开电加热    ： 102
    关闭电加热    ： 206
    打开节能    ：  103
    关闭节能    ：  208
    打开健康模式  ： 211
    关闭健康模式  ： 210
    打开数显  ： 213
    关闭数显  ： 212
    打开静音风  ： 215
    关闭静音风  ： 214
    打开强劲风  ： 217
    关闭强劲风  ： 216
    打开体感  ： 219
    关闭体感  ： 218
    打开新风  ： 221
    关闭新风  ： 220
*/

int ir_data_Air_Send_Ctl(int codeid, int CMD);

/**
 * @brief 发送数据到IR TASK
 *
 * @param cmd 命令ID
 * @param level 红外数据
 * @param level_len 数据长度
 * @return int RETURN_ERR  RETURN_OK
 */
int send_msg_to_ir_task(ir_ctrl_cmd_t cmd,unsigned short*level,int level_len, unsigned int file_data);


/*设置空调品牌功能
ID 0: 参考 eAirBrand
*/
int get_airc_brand_id(eAirBrand brand_num);




/**
 * NEC 码解码
 * nec_key :NEC码值，4byte
 * level_buf: 解码后的电平
 *
 * return 电平总数  RETURN_ERR input para error
 **/
int nec_decoder(unsigned char *nec_key, unsigned short *level_buf);

/**
 * @brief 发送NEC红外
 *
 * @param char 4字节NEC数据
 * @return int RETURN_OK 发送成功  RETURN_ERR 发送失败
 */
int send_nec_key(unsigned char *nec_key);


/**
 *由于unsigned short的最大值是65535，有的电平装不下，所以需要除以2*
 * @brief 发送红外电平 注意：电平数据需要除以 IR_DATA_DIV_COEFFICIENT,驱动发送时会自动还原数据
 *
 * @param level_buf 电平时间
 * @param length 电平个数
 * @return int RETURN_OK 发送成功  RETURN_ERR 发送失败
 */
int send_ir_level(unsigned short *level_buf, int length);

/**
 * @brief 获取当前空调状态
 *
 * @return airc_status_t* 空调状态
 */
normal_status_t *get_air_normal_status_val(void);

/**
 * @brief 保存空调状态
 *
 * @param new_state 最新空调状态
 */
void save_air_normal_status_val(normal_status_t *new_state);

/**
 * @brief 设置空调状态为默认状态
 *
 */
void set_air_status_default(void);

/**
 * @brief Set the air code index
 *
 * @param code_id
 */
void set_g_air_code_index(uint32_t code_id);

/***
* @brief compress ir level
* @param level_buf:input ir level buffer,由于unsigned short的最大值是65535，有的电平装不下，所以需要除以2*
* @param length:input ir level length
* @param p_ir_key:output compress data , not null
* @return int RETURN_OK ,RETURN_ERR
**/
int compress_ir_level(unsigned short *level_buf, int length, ir_data_t *p_ir_key);


/***
* @brief decompress ir level
* @param level_buf:output ir level buffer,由于unsigned short的最大值是65535，有的电平装不下，所以是除以2的*
* @param length:output ir level length
* @param p_ir_key:input compress data
* @return int RETURN_OK ,RETURN_ERR
**/
int decompress_ir_level(unsigned short *level_buf, int* length, ir_data_t *p_ir_key);



/*搜索码库控制接口，开始/结束*/
/*
cmd:
IR_SERCH_AIR_BRAND,               //搜索空调品牌
IR_STOP_SERCH_AIR_BRAND,          //停止搜索空调品牌
IR_SERCH_AIR_INDEX,               //搜索空调型号
IR_STOP_SERCH_AIR_INDEX,          //停止搜索空调型号

ctl:
int send_cnt;//每次发送一个空调，目前最低是3个
int timeout_ms;//每隔多长时间发送一次，最低3000MS
int (*ir_search_send_callback)(int overview, int air_code_id);  //发送按键后调用回调，例如是否搜索一圈，告诉现在的ID是多少

*/
int ir_data_search_ctl(ir_ctrl_cmd_t cmd, ir_search_ctl *ctl);
void save_air_special_status_val(special_status_t *new_state);
special_status_t *get_air_special_status_val(void);
#endif
