
/**
 * @file voice_print_recognition.c
 * @brief 声纹识别模块
 * @version 1.0.0
 * @date 2023-03-08
 *
 * @copyright Copyright (c) 2023 Chipintelli Technology Co., Ltd.
 *
 */

#include "FreeRTOS.h"
#include "timers.h"
#include "system_msg_deal.h"
#include "voice_print_recognition.h"
#include "ci_nvdata_manage.h"
#include "status_share.h"
#include "ci_flash_data_info.h"
#include "vp_nn_cmpt_port_host.h"
#include "remote_api_for_host.h"
#include "user_config.h"

#if USE_VPR
// 声纹模块状态枚举
typedef enum
{
    VPR_STATUS_REC,         // 识别状态
    VPR_STATUS_REG,         // 注册状态
    VPR_STATUS_IDLE,        // 空闲状态
}vpr_status_t;


typedef struct
{
    vpr_status_t status;                // 模块状态
    vpr_callback_t callback_func;       // 回调函数
    xTimerHandle reg_timer;             // 用于注册超时计数的TIMER
    uint8_t record_times;               //注册时重复输入次数
    uint8_t vpt_number;                 // 模板数量
}vpr_info_t;


static int asr_start_frm = 0;
static int asr_valid_frm = 0;
void get_asr_start_end_frm(int start,int valid)
{
    asr_start_frm = start;
    asr_valid_frm = valid;
}


static int vpr_tmp_init();
static uint8_t vpr_tmp_add();
static int vpr_tmp_del(int index);
static int vpr_tmp_delete_all();
static int vpr_tmp_get(int index, uint8_t* vpt_data_buffer, uint32_t vpt_data_buf_len);
static int vpr_tmp_get_number();
static void vpr_reg_timeout_callback(TimerHandle_t xTimer);

int8_t g_voice_print_rec_index = -1; //当前被识别用户id
vpr_info_t vpr_info = {0};             //识别信息模版
uint32_t vpr_info_id[MAX_VP_TEMPLATE_NUM*VP_REC_TIMES] = {0};  //用于存储注册用户id

uint8_t vpt_data_temp[VPT_SIZE] = {0};                   //用于每次从npu取出声纹特征数据
//uint8_t vpt_data_register[VP_REC_TIMES*VPT_SIZE] = {0};  //用于注册时候存储同一个用户的模版数据缓存
uint8_t *vpt_data_register = NULL;
uint8_t vpt_data_cmp[VPT_SIZE] = {0};       //用于识别时候从flash取出同一个用户的模版数据缓存
/**
 * @brief 初始化声纹模块
 * @para callback_func 函数指针，用于传入事件回调函数。事件回调函数用于对外通知声纹识别相关事件，比如"模板注册成功"、"模板注册失败"、"声纹匹配成功"。
 * @return 0:初始化成功; 非0:错误号。 
 */
int vpr_init(vpr_callback_t callback_func)
{
    REMOTE_CALL(vp_nn_cmpt_cfg(VP_USE_FRM_LEN/10,VP_CMPT_SKIP_NUM));
    mprintf("VP_USE_FRM_LEN = %d\n",VP_USE_FRM_LEN);
    extern void get_ci_vpr_model_addr(void);
    get_ci_vpr_model_addr();
    vpr_info.status = VPR_STATUS_IDLE;
    vpr_info.callback_func = callback_func;
    vpr_info.reg_timer = xTimerCreate("vpr_reg_timout", pdMS_TO_TICKS(MAX_VP_REG_TIME*1000),
        pdFALSE, (void *)0, vpr_reg_timeout_callback);
    vpr_tmp_init();     // 模板管理初始化
    return 0;
}

/**
 * @brief 开始注册声纹模板
 * @return 0:启动声纹注册成功; -1:启动声纹注册失败。
 */
int vpr_start_regist()
{
    mprintf("vpr_start_regist\n");
    if (vpr_info.vpt_number < MAX_VP_TEMPLATE_NUM)
    {
        vpr_info.status = VPR_STATUS_REG;
        vpr_info.record_times = 0;
        xTimerStart(vpr_info.reg_timer, 0);
        if (vpt_data_register == NULL)
            vpt_data_register = pvPortMalloc(VP_REC_TIMES*VPT_SIZE);
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief 停止注册声纹模板
 * @return 0:没有错误; 非0:错误号。
 */
int vpr_stop_regist()
{
    return 0;
}

/**
 * @brief 删除指定的声纹模板
 * @return 0:没有错误; -1:指定的模板号非有效模板(未注册或者已被删除)。
 */
int vpr_delete()
{
    return vpr_tmp_del(g_voice_print_rec_index);
}

/**
 * @brief 删除所有声纹模板
 * @return 0:没有错误; 非0:错误号。
 */
int vpr_clear()
{
    vpr_tmp_delete_all();
    return 0;
} 
/* float get_vpr_max_onetmp(float *vpt_data_temp,float *vp_buffer_identify)
{
    float cos_distance,cos_distance_max = 0;
    for (size_t i = 0; i < VP_REC_TIMES; i++)
    {
        cosin_metric(&vpt_data_temp[i*VPT_SIZE],vp_buffer_identify,&cos_distance);
        mprintf("cos_distance%d:%d\t",i,(int)(cos_distance*1000));
        if (cos_distance > cos_distance_max)
        {
            cos_distance_max = cos_distance;
        }
    }
    return cos_distance_max;
} */
uint8_t get_vpr_max(float *cos_distance)
{
    int index,i,k=0;
    float cos_distance_temp =  0;
    *cos_distance = 0;
    for (k=0;k<VP_SLIDE_TIME_PER_CMPT;k++)
    {
        bool ret = vp_nn_cmpt(VP_CMPT_TYPE_IDENTIFY,(float *)vpt_data_temp,100,k,asr_start_frm,asr_valid_frm);

        for(int i = 0;i < MAX_VP_TEMPLATE_NUM*VP_REC_TIMES;i++)
        {
            if (vpr_info_id[i] !=0)
            {
                vpr_tmp_get(vpr_info_id[i], vpt_data_cmp, VPT_SIZE);
                cosin_metric(vpt_data_cmp,vpt_data_temp,&cos_distance_temp);
                mprintf("%d,%d: %d; ", i,k,(int)(cos_distance_temp*1000));
                if (cos_distance_temp > *cos_distance)
                {
                    *cos_distance = cos_distance_temp;
                    index = i;
                }
            }
        }
        mprintf("\r\n");
    }
    return index;
}

void vpr_registe()
{
    float score;
    int index = 0;
    bool register_fail = false;
    vp_nn_cmpt(VP_CMPT_TYPE_REG, (float *)vpt_data_temp, 100, 0, asr_start_frm, asr_valid_frm);
    if (vpr_info.record_times == 0)
    {
        xTimerReset(vpr_info.reg_timer,0);
        memset(vpt_data_register,0,VP_REC_TIMES*VPT_SIZE);
        memcpy(vpt_data_register, vpt_data_temp, VPT_SIZE);
        vpr_info.record_times ++;
    }
    else
    {
        cosin_metric(vpt_data_temp, vpt_data_register, &score); // 第二次跟第一次比 
        mprintf("第%d次重采样!>>>>>>>score:%d\n",vpr_info.record_times,(int)(score*1000)); 
        if (score > VP_THR_FOR_SAME_MATCH)
        {
            xTimerReset(vpr_info.reg_timer,0);
            memcpy(&vpt_data_register[vpr_info.record_times*VPT_SIZE], vpt_data_temp, VPT_SIZE);
            vpr_info.record_times ++;
        }
        else
        {
            register_fail = true;
            xTimerStop(vpr_info.reg_timer, 0);
            if (vpr_info.vpt_number != 0)
                vpr_info.status = VPR_STATUS_REC;  //有模版，继续等待识别
            else
                vpr_info.status = VPR_STATUS_IDLE;  //没有模版，等待注册
            if (vpt_data_register)
            {
                vPortFree(vpt_data_register);
                vpt_data_register = NULL;
            }
            vpr_info.callback_func(vpr_reg_resample_failed, 1);
        }
        
    }

    if (vpr_info.record_times >= VP_REC_TIMES)
    {
        index = vpr_tmp_add(vpt_data_register, VPT_SIZE*VP_REC_TIMES);
        vpr_info.status = VPR_STATUS_REC;
        xTimerStop(vpr_info.reg_timer, 0);
        vpr_info.callback_func(vpr_reg_successed, index);
        if (vpt_data_register)
        {
            vPortFree(vpt_data_register);
            vpt_data_register = NULL;
        }
            
    }
    else if (!register_fail)
        vpr_info.callback_func(vpr_reg_resample, index);

}

/**
 * @brief 执行一次声纹识别
 * @return 0:没有错误; 非0:错误号。
 */
int vpr_run_one_recognition()
{
    static float cos_distance=0;
    int index = 0;
    switch(vpr_info.status)
    {
    case VPR_STATUS_REG:
        if (vpr_info.vpt_number)
        {
            get_vpr_max(&cos_distance);
            mprintf("max socre %d\n", (int)(cos_distance*1000));
            if (cos_distance > VP_THR_FOR_SAME_MATCH)  
            {
                xTimerStop(vpr_info.reg_timer, 0);
                vpr_info.status = VPR_STATUS_REC;
                vpr_info.callback_func(vpr_reg_resample_failed, index);
                break;
            }
        }
        vpr_registe();
        break;

    case VPR_STATUS_REC:
        //mprintf("vpr_run_one_recognition number %d\n",vpr_info.vpt_number);
        if (vpr_info.vpt_number)
        {
            index = get_vpr_max(&cos_distance);// 查找最大分数的人以及最大分数
            mprintf("id %x max score %d\n", vpr_info_id[index],(int)(cos_distance*1000));
            index = index/VP_REC_TIMES;
            if (cos_distance > VP_THR_FOR_MATCH)
            {
                if (vpr_info.callback_func)
                {
                    vpr_info.callback_func(vpr_rec_successed, index);
                }
                //mprintf("vpr rst: %d cos_distance :\n", index, cos_distance);
            }
            else
            {
                if (vpr_info.callback_func)
                {
                    vpr_info.callback_func(vpr_rec_failed, 0);
                }
            }
        }
        break;
    case VPR_STATUS_IDLE:
        break;
        
    default:
        break;
    }
    return 0;
}

void vpr_reg_timeout_callback(TimerHandle_t xTimer)
{
    if (vpr_info.vpt_number != 0)
        vpr_info.status = VPR_STATUS_REC;  //有模版，继续等待识别
    else
        vpr_info.status = VPR_STATUS_IDLE;  //没有模版，等待注册
    
    if (vpr_info.callback_func)
    {
        vpr_info.callback_func(vpr_reg_failed, vpr_get_current_reg_id());
    }
    if (vpt_data_register)
    {
        vPortFree(vpt_data_register);
        vpt_data_register = NULL;
    }
    mprintf("vrp reg timeout!\n");
}

//////////////////template manage//////////////////////
int vpr_tmp_init()
{
    uint8_t num = 0;
    uint16_t real_len;
    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VP_NUMBER, sizeof(num), &num, &real_len))
    {
        num = 0;
        cinv_item_init(NVDATA_ID_VP_NUMBER, sizeof(num), &num);
    }
    vpr_info.vpt_number = num;
    if (vpr_info.vpt_number == 0)//未注册模版
    {
        cinv_item_init(NVDATA_ID_VP_INFO, MAX_VP_TEMPLATE_NUM*VP_REC_TIMES*4, vpr_info_id);
    }
    else
    {
        cinv_item_read(NVDATA_ID_VP_INFO, MAX_VP_TEMPLATE_NUM*VP_REC_TIMES*4, vpr_info_id, &real_len);
        mprintf("vpr_info rigster number:%d\r\n",num);
        for (size_t i = 0; i < MAX_VP_TEMPLATE_NUM*VP_REC_TIMES; i++)
        {
            if (vpr_info_id[i] != 0)
                mprintf("vpr_info_id : %x\r\n",vpr_info_id[i]);
        }
        vpr_info.status = VPR_STATUS_REC;
    }
}

uint8_t vpr_tmp_add(uint8_t * vpt_data, uint32_t vpt_data_len)
{
    uint8_t i;
    vpr_info.vpt_number ++; 
    for (i = 0; i < vpr_info.vpt_number; i++)
    {
        if (vpr_info_id[i*VP_REC_TIMES] == 0)
            break;
    }
    
    for (size_t k = 0; k < VP_REC_TIMES; k++)
    {
        uint32_t save_id = NVDATA_ID_VP_MODE + i*VP_REC_TIMES + k;
        mprintf("vpr_tmp_add save_id:%x\r\n",save_id);
        if (CINV_OPER_SUCCESS != cinv_item_write(save_id, VPT_SIZE, &vpt_data[k*VPT_SIZE]))  //存储模版
        {
            cinv_item_init(save_id, VPT_SIZE, &vpt_data[k*VPT_SIZE]);
        }
        
        vpr_info_id[i*VP_REC_TIMES + k] = save_id;  //存储用户id
    }

    if (CINV_OPER_SUCCESS != cinv_item_write(NVDATA_ID_VP_INFO, MAX_VP_TEMPLATE_NUM*VP_REC_TIMES*4, vpr_info_id))
    {
        cinv_item_init(NVDATA_ID_VP_INFO, MAX_VP_TEMPLATE_NUM*VP_REC_TIMES*4, vpr_info_id);    
    }
       //存储总用户数
    if (CINV_OPER_SUCCESS != cinv_item_write(NVDATA_ID_VP_NUMBER, sizeof(vpr_info.vpt_number), &(vpr_info.vpt_number)))
    {
        cinv_item_init(NVDATA_ID_VP_NUMBER, sizeof(vpr_info.vpt_number), &(vpr_info.vpt_number));  
    }
    return i;
}
//获取当前注册ID
int vpr_get_current_reg_id(void)
{
    uint8_t i = 0; 
    for (i = 0; i < vpr_info.vpt_number; i++)
    {
        if (vpr_info_id[i*VP_REC_TIMES] == 0)
            break;
    }
    return i;
}
int vpr_tmp_del(int index)
{
    if (index < MAX_VP_TEMPLATE_NUM && index != -1)
    {
        for (size_t i = 0; i < VP_REC_TIMES; i++)
        {
            mprintf("vpr_tmp_del %x\r\n",vpr_info_id[index*VP_REC_TIMES + i]);
            if(CINV_OPER_SUCCESS == cinv_item_delete(vpr_info_id[index*VP_REC_TIMES + i]))
            {
                vpr_info_id[index*VP_REC_TIMES + i] = 0;
            }
        }
        cinv_item_write(NVDATA_ID_VP_INFO, MAX_VP_TEMPLATE_NUM*VP_REC_TIMES*4, vpr_info_id);
        vpr_info.vpt_number -= 1;
        cinv_item_write(NVDATA_ID_VP_NUMBER, sizeof(vpr_info.vpt_number), &(vpr_info.vpt_number));
        if (vpr_info.vpt_number == 0)
            vpr_info.status = VPR_STATUS_IDLE;
        g_voice_print_rec_index = -1;
        return 0;
    }
    return -1;
}

int vpr_tmp_delete_all()
{
    for (int i = 0;i < VP_REC_TIMES*MAX_VP_TEMPLATE_NUM;i++)
    {
        if (vpr_info_id[i] != 0)
        {
            cinv_item_delete(vpr_info_id[i]);
            mprintf("vpr_tmp_delete %x\r\n",vpr_info_id[i]);
        }
    }
    vpr_info.vpt_number = 0;
    cinv_item_write(NVDATA_ID_VP_NUMBER, sizeof(vpr_info.vpt_number), &(vpr_info.vpt_number));
    memset(vpr_info_id, 0, VP_REC_TIMES*MAX_VP_TEMPLATE_NUM*4);
    cinv_item_write(NVDATA_ID_VP_INFO, VP_REC_TIMES*MAX_VP_TEMPLATE_NUM*4, vpr_info_id);
    vpr_info.status = VPR_STATUS_IDLE;
    g_voice_print_rec_index = -1;
    return 0;
}

int vpr_tmp_get(int save_id, uint8_t* vpt_data_buffer, uint32_t vpt_data_buf_len)
{
    uint16_t real_len = 0;
    
    if (CINV_OPER_SUCCESS != cinv_item_read(save_id, vpt_data_buf_len, vpt_data_buffer, &real_len))
    {
        return -1;
    }
    return 0;
}

int vpr_tmp_get_number()
{
    int ret = -1;
    uint32_t num;
    uint16_t real_len;
    if (CINV_OPER_SUCCESS == cinv_item_read(NVDATA_ID_VP_NUMBER, sizeof(num), &num, &real_len))
    {
        ret = num;
    }
    return ret;
}

#if CLOUD_UART_PROTOCOL_EN
#include "chipintelli_cloud_protocol.h"
extern VPRCloudCmdProtocolTypedef gVPRCloudCmdProtocol[MAX_VP_TEMPLATE_NUM][VPR_STATE_MAX];
void vpr_callback(vpr_callback_rst_t rst, int rec_index)
{
    if(rec_index >= MAX_VP_TEMPLATE_NUM)
    {
        ci_logerr(CI_LOG_ERROR, "rec_index error\n");
        return;
    }
    switch(rst)
    {
        case vpr_reg_successed:
            mprintf("voice print registe successed:%x\n", rec_index);
            #if CLOUD_CFG_PLAY_EN
                if(gVPRCloudCmdProtocol[rec_index][VPR_REG_SUCCESS].cloudCmdProtocol.play_type == 0)  //主动播报
                    prompt_play_by_cmd_id(gVPRCloudCmdProtocol[rec_index][VPR_REG_SUCCESS].cloudCmdProtocol.cmd_id, -1, default_play_done_callback, true);
            #endif 
            #if CLOUD_CFG_UART_SEND_EN
                cloud_uart_send_buf(gVPRCloudCmdProtocol[rec_index][VPR_REG_SUCCESS].cloudCmdProtocol.protocol_buf, gVPRCloudCmdProtocol[rec_index][VPR_REG_SUCCESS].cloudCmdProtocol.protocol_len);
            #endif
            break;
        case vpr_reg_failed:
            mprintf("voice print registe failed:%x\n", rec_index);
            #if CLOUD_CFG_PLAY_EN
                if(gVPRCloudCmdProtocol[rec_index][VPR_REG_ERR].cloudCmdProtocol.play_type == 0)  //主动播报
                    prompt_play_by_cmd_id(gVPRCloudCmdProtocol[rec_index][VPR_REG_ERR].cloudCmdProtocol.cmd_id, -1, default_play_done_callback, true);
            #endif
            #if CLOUD_CFG_UART_SEND_EN
                cloud_uart_send_buf(gVPRCloudCmdProtocol[rec_index][VPR_REG_ERR].cloudCmdProtocol.protocol_buf, gVPRCloudCmdProtocol[rec_index][VPR_REG_ERR].cloudCmdProtocol.protocol_len);
            #endif
            break;
        case vpr_rec_successed:
            mprintf("voice print recognition successed:%x\n", rec_index);
            g_voice_print_rec_index = rec_index;
            #if CLOUD_CFG_PLAY_EN
                if(gVPRCloudCmdProtocol[rec_index][VPR_ASR_SUCCESS].cloudCmdProtocol.play_type == 0)  //主动播报
                    prompt_play_by_cmd_id(gVPRCloudCmdProtocol[rec_index][VPR_ASR_SUCCESS].cloudCmdProtocol.cmd_id, -1, default_play_done_callback, true);
            #endif    
            #if CLOUD_CFG_UART_SEND_EN
                cloud_uart_send_buf(gVPRCloudCmdProtocol[rec_index][VPR_ASR_SUCCESS].cloudCmdProtocol.protocol_buf, gVPRCloudCmdProtocol[rec_index][VPR_ASR_SUCCESS].cloudCmdProtocol.protocol_len);
            #endif
            break;
        case vpr_rec_failed:
            mprintf("voice print recognition failed:%x\n", rec_index);
            g_voice_print_rec_index = -1;
            #if CLOUD_CFG_PLAY_EN
                if(gVPRCloudCmdProtocol[rec_index][VPR_ASR_ERR].cloudCmdProtocol.play_type == 0)  //主动播报
                    prompt_play_by_cmd_id(gVPRCloudCmdProtocol[rec_index][VPR_ASR_ERR].cloudCmdProtocol.cmd_id, -1, default_play_done_callback, true);
             #endif   
            #if CLOUD_CFG_UART_SEND_EN
                cloud_uart_send_buf(gVPRCloudCmdProtocol[rec_index][VPR_ASR_ERR].cloudCmdProtocol.protocol_buf, gVPRCloudCmdProtocol[rec_index][VPR_ASR_ERR].cloudCmdProtocol.protocol_len);
            #endif
            break;
        case vpr_reg_resample:
            prompt_play_by_cmd_id(VPR_REG_RESAMPLE,-1,default_play_done_callback,true);
            mprintf("voice print recognition resample\n");
            break;
        case vpr_reg_resample_failed:
            prompt_play_by_cmd_id(VPR_REG_RESAMPLE_FAILED,rec_index,default_play_done_callback,true);
            mprintf("voice print recognition resample failed\n");
            break;
        default:
            break;
    }
}
#else
void vpr_callback(vpr_callback_rst_t rst, int rec_index)
{
    switch(rst)
    {
        case vpr_reg_successed:
            prompt_play_by_cmd_id(VPR_REG_SUCCESSED,-1,default_play_done_callback,true);
            mprintf("voice print registe successed:%x\n", rec_index);
            break;
        case vpr_reg_failed:
            prompt_play_by_cmd_id(VPR_REG_FAILED,-1,default_play_done_callback,true);
            mprintf("voice print registe failed:%x\n", rec_index);
            break;
        case vpr_rec_successed:
            prompt_play_by_cmd_id(VPR_REC_SUCCESSED,-1,default_play_done_callback,true);
            mprintf("voice print recognition successed:%x\n", rec_index);
            g_voice_print_rec_index = rec_index;
            break;
        case vpr_rec_failed:
            prompt_play_by_cmd_id(VPR_REC_FAILED,-1,default_play_done_callback,true);
            g_voice_print_rec_index = -1;
            mprintf("voice print recognition failed:%x\n", rec_index);
            break;
        case vpr_reg_resample:
            prompt_play_by_cmd_id(VPR_REG_RESAMPLE,-1,default_play_done_callback,true);
            mprintf("voice print recognition resample\n");
            break;
        case vpr_reg_resample_failed:
            prompt_play_by_cmd_id(VPR_REG_RESAMPLE_FAILED,rec_index,default_play_done_callback,true);
            mprintf("voice print recognition resample failed\n");
            break;
        default:
            break;
    }
}
#endif
uint8_t vpr_get_status()
{
    return vpr_info.status;
}
#endif