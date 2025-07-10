/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-03-08 16:31:30
 * @LastEditTime: 2023-05-15 14:47:46
 * @Description: 音频上传
 */
#include "ci_log.h"
#include "status_share.h"
#include "cias_network_msg_protocol.h"
#include "cias_network_msg_send_task.h"
#include "cias_uart_protocol.h"
#include "cias_voice_upload.h"
#include "FreeRTOS.h"
#include "audio_play_decoder.h"
#if AUDIO_COMPRESS_SPEEX_ENABLE
#include "ci110x_speex.h"
#include "sb_celp.h"
#include "nb_celp.h"
extern  SpeexMode *cias_speex_wb_mode;
extern const SpeexSBMode sb_wb_mode;
extern void *sb_encoder_init(const SpeexMode *m);
extern void sb_encoder_destroy(void *state);
extern int sb_decode(void *state, SpeexBits *bits, void *vout);
extern int sb_encoder_ctl(void *state, int request, void *ptr);
extern int sb_decoder_ctl(void *state, int request, void *ptr);
extern int sb_encode(void *state, void *vin, SpeexBits *bits);
extern void *sb_decoder_init(const SpeexMode *m);
extern void sb_decoder_destroy(void *state);
static ci_speex_t *ci_speex_hander = NULL;
#endif //AUDIO_COMPRESS_SPEEX_ENABLE

#if VOICE_UPLOAD_BY_UART

#define PLAYING_COUNT_NUM (20)
#define VAD_ON_COUNT_NUM (40)
#define PCM_MSG_QUEUE_NUM 6//3
#define SPEEX_NEED_PCM_LEN 320
volatile int32_t vad_on_count = 0;

QueueHandle_t pcm_speex_deal_queue = NULL;

volatile bool voice_upload_vad_enable = false;
extern int get_heap_bytes_remaining_size(void);

typedef enum
{
    ESTVAD_IDLE = 0, /*!<vad的状态处于IDLE状态    */
    ESTVAD_START,    /*!<vad的状态处于START状态   */
    ESTVAD_ON,       /*!<vad的状态处于ON状态      */
    ESTVAD_END,      /*!<vad的状态处于END状态     */
} estvad_state_t;

static bool voice_upload_enable_t = false;

static bool singe_upload_voicedata = true; //voice 单轮上传
static bool first_play_stated = true;
static int32_t first_play_frame_count = 0; //唤醒播报后基数（20帧音频）
void voice_upload_enable(void)
{
    voice_upload_enable_t = true;
    cias_send_cmd(WAKE_UP, NO_WAKEUP_FILL_DATA);
    vad_on_count = 0;
    singe_upload_voicedata = true;
    first_play_stated = true;
    first_play_frame_count = 0;
    mprintf("(start) upload numbs********************(%d)\n",vad_on_count);
    
}

void voice_upload_disable(void)
{
    cias_send_cmd(PCM_FINISH, NO_WAKEUP_FILL_DATA); //remove by yjd
    voice_upload_enable_t = false;
    mprintf("(end_%d) upload numbs********************(%d)\n",__LINE__,vad_on_count);
    vad_on_count = 0;
    // play_send_pcm_finish();
}

extern volatile bool asr_reseult_wakup_falg;
static bool voice_upload_tt(void)
{
    /* 根据VAD标记判断是否结束上传 */
    bool ret = false;

    status_t wakeup_state = ciss_get(CI_SS_WAKING_UP_STATE); //唤醒词状态获取//唤醒词状态

    status_t voiceup_vad_state_flag = ciss_get(CI_SS_VAD_STATE); // asr_vad 获取vad_start时刻

   // status_t playing_state = ciss_get(CI_SS_PLAY_STATE); //获取播放状态


    static bool start_counting = false;
    

    // if(wifi_current_state_get()!= NETWORK_CONNECTED_STATE)
    // {
    //     return false;
    // }
    if ((CI_SS_WAKEUPED == wakeup_state && singe_upload_voicedata))    
    {

        if (first_play_stated)
        {
            mprintf("CI_SS_PLAY_STATE_PLAYING\r\n");
            first_play_stated = false;
            start_counting = true;
            //vTaskDelay(pdMS_TO_TICKS(500));
            //return false;
        }
        if (start_counting)
        {
            //ci_loginfo(LOG_VOICE_UPLOAD,"first_play_frame_count(%d)\r\n",first_play_frame_count);
            first_play_frame_count++;
            if (first_play_frame_count < PLAYING_COUNT_NUM)
            {
                ret = false;
            }
            else
            {
                ret = true;
            }
        }

        if ((CI_SS_VAD_ON == voiceup_vad_state_flag) || (CI_SS_VAD_START == voiceup_vad_state_flag))
        {
            vad_on_count++;
            ret = true;
        }
        else if (CI_SS_VAD_END == voiceup_vad_state_flag) //过滤短音频，40帧长度的忽略
        {
            if (vad_on_count > VAD_ON_COUNT_NUM)
            {
                ret = false;
                // mprintf("voiceup_vad_state_flag********************(%d)\n",vad_on_count);
                mprintf("not nn vad end...\r\n");
                if(!asr_reseult_wakup_falg)
                    voice_upload_disable();
                start_counting = false;
                first_play_frame_count = 0;
                singe_upload_voicedata = false;
            }
            vad_on_count = 0;
        }
    }
    if(voice_upload_enable_t == false)
        ret = false;
    if (CI_SS_WAKEUPED != wakeup_state)
    {
        singe_upload_voicedata = true;
        first_play_stated = true;
    }

    //mprintf("1--wakeup_state = %d, singe_upload_voicedata = %d, ret = %d\r\n", wakeup_state, singe_upload_voicedata, ret);
    return ret;
}

//32K数据抽取成16k数据，
int decrease_sampling_rate(short *addr, short *obj_addr, int frame_len_size)//frame_len_size=640
{
    int frame_len_num = frame_len_size/2;
    for(int i = 0;i < frame_len_num/2; ++i)//256
    {
        
        obj_addr[i] = addr[2*i];

    }
    
    return frame_len_num;
}

static short tmp_addr1[320] = {0};
static int8_t dst_addr1[SPEEX_NEED_PCM_LEN] = {0};

/************************************** speex *******************************************/
static int8_t src_addr[SPEEX_NEED_PCM_LEN * 2] = {0};
static int8_t dst_addr[64] = {0};
#if VOICE_PLAY_BY_UART
bool free_m4a_buffer = true;
#endif
void speex_deal_task(void *p_arg)
{
    int8_t index = 0;
    int32_t encode_t = SPEEX_NEED_PCM_LEN;
    BaseType_t ret = pdPASS;
    memset(src_addr, 0, sizeof(src_addr));
    //mprintf("xx-1Heap left: %dKB\n", get_heap_bytes_remaining_size() / 1024);
    #if AUDIO_COMPRESS_SPEEX_ENABLE
    SpeexMode *speex_wb_mode_new = pvPortMalloc(sizeof(SpeexMode));
    if (speex_wb_mode_new != NULL)
    {
        speex_wb_mode_new->mode = &sb_wb_mode;
        speex_wb_mode_new->query = wb_mode_query;
        speex_wb_mode_new->modeName = pvPortMalloc(strlen("wideband (sub-band CELP)"));
        memcpy(speex_wb_mode_new->modeName, "wideband (sub-band CELP)", strlen("wideband (sub-band CELP)"));
        speex_wb_mode_new->modeID = 1;
        speex_wb_mode_new->bitstream_version = 4;
        speex_wb_mode_new->enc_init = sb_encoder_init;
        speex_wb_mode_new->enc_destroy = sb_encoder_destroy;
        speex_wb_mode_new->enc = sb_encode;
        speex_wb_mode_new->enc_ctl = sb_encoder_ctl;
        cias_speex_wb_mode = speex_wb_mode_new;
    }
    else
    {
        mprintf("speex_wb_mode_new malloc error");
        return NULL;
    }
    ci_speex_hander = ci_speex_create(); //初始化speex
    if (NULL == ci_speex_hander)
    {
        return;
    }
    ci_speex_hander->ci_speex_mode = CI_SPEEX_INT;
    #endif
    status_t playing_state;
    while (1)
    {
        voice_upload_vad_enable = voice_upload_tt(); //支持语音上传
        if( (ciss_get(CI_SS_NNVAD_STATE) == CI_SS_VAD_END) && voice_upload_enable_t)
        {
            mprintf("nn vad end...\r\n");
            if(!asr_reseult_wakup_falg)
                voice_upload_disable();
        }
        ret = xQueueReceive(pcm_speex_deal_queue, src_addr + (index * SPEEX_NEED_PCM_LEN), pdMS_TO_TICKS(20));
        if (pdPASS == ret)
        {
		#if VOICE_PLAY_BY_UART
            if (!free_m4a_buffer)
            {
                continue;
            }
		#endif
            #if AUDIO_COMPRESS_SPEEX_ENABLE
            if (index&&ci_speex_hander)
            #else
            if(index)
            #endif
            {
    #if AUDIO_COMPRESS_SPEEX_ENABLE
                memset(dst_addr, 0, sizeof(dst_addr));
                // 进入speex压缩                                                                                     //上传speex压缩后的音频数据到云端
                encode_t = cias_speex_compressed_data(ci_speex_hander, src_addr, dst_addr, SPEEX_NEED_PCM_LEN);     //压缩音频数据
                voice_data_packet_and_send(PCM_MIDDLE, dst_addr, encode_t);                                         //上传数据（打包协议）
    #else                                                                                                           //不压缩直接传输音频数据
                voice_data_packet_and_send(PCM_MIDDLE, src_addr, SPEEX_NEED_PCM_LEN * 2); //上传数据（打包协议）
    #endif
                // network_send(src_addr, SPEEX_NEED_PCM_LEN * 2); //上传前端处理后的数据到串口,一般用于采音调试用
                memset(src_addr, 0, sizeof(src_addr));
                index = 0;
            }
            else
            {
                index++;
            }
                
        }
    }
}

int voice_upload_task_init(void)
{
    pcm_speex_deal_queue = xQueueCreate(PCM_MSG_QUEUE_NUM, SPEEX_NEED_PCM_LEN);
    xTaskCreate(speex_deal_task, "speex_deal_task", 512, NULL, 5, NULL);   //4->5，解决离在线播放speex压缩优先级低导致播放异问题
}

#endif