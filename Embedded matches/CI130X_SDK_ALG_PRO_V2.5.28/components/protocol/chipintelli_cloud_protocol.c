#include "chipintelli_cloud_protocol.h"
#include "ci130x_system.h"

#if CLOUD_UART_PROTOCOL_EN
void cloud_uart_send_buf(char *pbuf, int len)
{
    while(len--)
        UartPollingSenddata((UART_TypeDef *)CLOUD_CFG_UART_PORT, *pbuf++);
}

#if USE_VPR  
VPRCloudCmdProtocolTypedef gVPRCloudCmdProtocol[MAX_VP_TEMPLATE_NUM][VPR_STATE_MAX] = {0};
void vpr_cloud_cmd_param_init(const VPRCloudCmdProtocolTypedef **pVPRCloudCmdProtocol)
{
    memcpy((void *)gVPRCloudCmdProtocol, (void **)pVPRCloudCmdProtocol, sizeof(VPRCloudCmdProtocolTypedef)*MAX_VP_TEMPLATE_NUM*VPR_STATE_MAX);
}
void vpr_cloud_cmd_init_call(void)
{
    VPRCloudCmdProtocolTypedef mVPRCloudCmdProtocol[MAX_VP_TEMPLATE_NUM][VPR_STATE_MAX] = {
        {
            {VPR_REG_SUCCESS, {10004, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册成功
            {VPR_REG_ERR,     {10005, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册失败
            {VPR_ASR_SUCCESS, {10007, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //识别成功
            {VPR_ASR_ERR,     {1, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},         //识别失败只有一个播报音和协议
        },
        {
            {VPR_REG_SUCCESS, {10004, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册成功
            {VPR_REG_ERR,     {10005, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册失败
            {VPR_ASR_SUCCESS, {10007, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //识别成功
            {},
        },
        {
            {VPR_REG_SUCCESS, {10004, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册成功
            {VPR_REG_ERR,     {10005, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册失败
            {VPR_ASR_SUCCESS, {10007, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //识别成功
            {},
        },
        {
            {VPR_REG_SUCCESS, {10004, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册成功
            {VPR_REG_ERR,     {10005, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //注册失败
            {VPR_ASR_SUCCESS, {10007, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //识别成功
            {},
        },
    };
    vpr_cloud_cmd_param_init(mVPRCloudCmdProtocol);
}
#endif   //USE_VPR

#if USE_WMAN_VPR  
VGRCloudCmdProtocolTypedef gVGRCloudCmdProtocol[VGR_STATE_MAX] = {0};
void vgr_cloud_cmd_param_init(const VGRCloudCmdProtocolTypedef *pVGRCloudCmdProtocol)
{
    memcpy((void *)gVGRCloudCmdProtocol, (void *)pVGRCloudCmdProtocol, sizeof(VGRCloudCmdProtocolTypedef)*VGR_STATE_MAX);
}
void vgr_cloud_cmd_init_call(void)
{
    VGRCloudCmdProtocolTypedef mVGRCloudCmdProtocol[VGR_STATE_MAX] = {
        {VGR_ASR_MAN, {10012, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},      //识别到男生
        {VGR_ASR_WMAN, {10013, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}},     //识别到女生
    };
    vgr_cloud_cmd_param_init(mVGRCloudCmdProtocol);
}
#endif   //USE_WMAN_VPR


#if USE_SED_CRY  
SedCryCloudCmdProtocolTypedef gSedCryCloudCmdProtocol = {0};
void cry_cloud_cmd_param_init(const SedCryCloudCmdProtocolTypedef sedCryCloudCmdProtocol)
{
    memcpy((void *)&gSedCryCloudCmdProtocol, (void *)&sedCryCloudCmdProtocol, sizeof(SedCryCloudCmdProtocolTypedef));
}
void cry_cloud_cmd_init_call(void)
{
    SedCryCloudCmdProtocolTypedef mSedCryCloudCmdProtocol = {
        CRY_CHECKED_SUCCESS, {10010, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}     //监测到哭声
    };
    cry_cloud_cmd_param_init(mSedCryCloudCmdProtocol);
}
#endif   //USE_SED_CRY

#if USE_SED_SNORE  
SedSnoreCloudCmdProtocolTypedef gSedSnoreCloudCmdProtocol = {0};
void snore_cloud_cmd_param_init(const SedSnoreCloudCmdProtocolTypedef sedSnoreCloudCmdProtocol)
{
    memcpy((void *)&gSedSnoreCloudCmdProtocol, (void *)&sedSnoreCloudCmdProtocol, sizeof(SedSnoreCloudCmdProtocolTypedef));
}
void snore_cloud_cmd_init_call(void)
{
    SedSnoreCloudCmdProtocolTypedef mSedSnoreCloudCmdProtocol = {
        SNORE_CHECKED_SUCCESS, {10011, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 0}     //监测到鼾声
    };
    snore_cloud_cmd_param_init(mSedSnoreCloudCmdProtocol);
}
#endif   //USE_SED_SNORE

#if USE_AI_DOA  
DOACloudCmdProtocolTypedef gDOACloudCmdProtocol[DOA_STATE_MAX] = {0};
void doa_cloud_cmd_param_init(const DOACloudCmdProtocolTypedef *doaCloudCmdProtocol)
{
    memcpy((void *)gDOACloudCmdProtocol, (void *)doaCloudCmdProtocol, sizeof(DOACloudCmdProtocolTypedef)*DOA_STATE_MAX);
}
void doa_cloud_cmd_init_call(void)
{
    DOACloudCmdProtocolTypedef mDOACloudCmdProtocol[DOA_STATE_MAX] = {
        {DOA_CHECKED_ANGLE_0, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},     //识别到0度
        {DOA_CHECKED_ANGLE_10, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},    //识别到10度
        {DOA_CHECKED_ANGLE_20, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_30, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_40, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_50, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_60, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_70, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_80, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_90, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_100, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_110, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_120, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_130, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_140, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_150, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_160, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_170, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
        {DOA_CHECKED_ANGLE_180, {10003, "\xA5\xFA\xe0\x81\xB\x60\x2B\xFB", 8, 1}},
    };
    doa_cloud_cmd_param_init(mDOACloudCmdProtocol);
}
#endif   //USE_AI_DOA
#endif   //CLOUD_UART_PROTOCOL_EN