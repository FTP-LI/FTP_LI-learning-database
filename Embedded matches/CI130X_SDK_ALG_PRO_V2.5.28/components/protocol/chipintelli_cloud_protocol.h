#ifndef __CHIPINTELLI_CLOUD_PROTOCOL_H__

#include "user_config.h"
#include <stdint.h>
#if CLOUD_UART_PROTOCOL_EN
//云端数据协议
typedef struct 
{
    uint16_t cmd_id;               //命令词ID
    int8_t  protocol_buf[24];      //命令词发送协议
    uint8_t protocol_len;          //协议长度
    uint8_t play_type;             //主动播报
}CloudCmdProtocolTypedef;

#if USE_VPR 
typedef enum
{
    VPR_REG_SUCCESS = 0,          //声纹注册成功
    VPR_REG_ERR,                  //注册失败
    VPR_ASR_SUCCESS,              //声纹识别成功
    VPR_ASR_ERR,                  //识别失败
    VPR_STATE_MAX,              
}VPRStateEnum_t;

//声纹注册+云端数据协议
typedef struct 
{ 
    uint8_t vpr_func_id;          //声纹功能id
    CloudCmdProtocolTypedef cloudCmdProtocol;
}VPRCloudCmdProtocolTypedef;
#endif  //USE_VGR

#if USE_WMAN_VPR
typedef enum
{
    VGR_ASR_MAN = 0,               //识别到男生
    VGR_ASR_WMAN,                  //识别到女生
    VGR_STATE_MAX,              
}VGRStateEnum_t;

//声纹注册+云端数据协议
typedef struct 
{ 
    uint8_t vgr_func_id;          //男生声纹识别id
    CloudCmdProtocolTypedef cloudCmdProtocol;
}VGRCloudCmdProtocolTypedef;
#endif  //USE_WMAN_VPR

#if USE_SED_CRY
typedef enum
{
    CRY_CHECKED_SUCCESS = 0,          //检测到事件
    CRY_STATE_MAX,              
}SedStateEnum_t;
//哭声注册+云端数据协议
typedef struct 
{ 
    uint8_t cry_func_id;            //哭声检测功能id
    CloudCmdProtocolTypedef cloudCmdProtocol;
}SedCryCloudCmdProtocolTypedef;
#endif
#if USE_SED_SNORE
//鼾声注册+云端数据协议
typedef enum
{
    SNORE_CHECKED_SUCCESS = 0,          //检测到事件
    SNORE_STATE_MAX,              
}SedStateEnum_t;
typedef struct 
{ 
    uint8_t snore_func_id;          //鼾声检测功能id
    CloudCmdProtocolTypedef cloudCmdProtocol;
}SedSnoreCloudCmdProtocolTypedef;
#endif

#if USE_AI_DOA
typedef enum
{
    DOA_CHECKED_ANGLE_0 = 0,          //检测到0度
    DOA_CHECKED_ANGLE_10,             //检测到10度
    DOA_CHECKED_ANGLE_20,          
    DOA_CHECKED_ANGLE_30,         
    DOA_CHECKED_ANGLE_40,          
    DOA_CHECKED_ANGLE_50,         
    DOA_CHECKED_ANGLE_60, 
    DOA_CHECKED_ANGLE_70,         
    DOA_CHECKED_ANGLE_80,          
    DOA_CHECKED_ANGLE_90,         
    DOA_CHECKED_ANGLE_100, 
    DOA_CHECKED_ANGLE_110,         
    DOA_CHECKED_ANGLE_120,          
    DOA_CHECKED_ANGLE_130,         
    DOA_CHECKED_ANGLE_140, 
    DOA_CHECKED_ANGLE_150,         
    DOA_CHECKED_ANGLE_160,          
    DOA_CHECKED_ANGLE_170,         
    DOA_CHECKED_ANGLE_180,          //检测到180度    
    DOA_STATE_MAX,              
}DOAStateEnum_t;
//DOA+云端数据协议
typedef struct 
{ 
    uint8_t doa_angle_id;          //角度ID
    CloudCmdProtocolTypedef cloudCmdProtocol;
}DOACloudCmdProtocolTypedef;
#endif


void cloud_uart_send_buf(char *pbuf, int len);
void vpr_cloud_cmd_init_call(void);
void vgr_cloud_cmd_init_call(void);
void cry_cloud_cmd_init_call(void);
void snore_cloud_cmd_init_call(void);
void doa_cloud_cmd_init_call(void);
#endif  //__CHIPINTELLI_CLOUD_PROTOCOL_H__
#endif
