/*
 * @FileName:: 
 * @Author: 
 * @Date: 2022-04-21 13:58:23
 * @LastEditTime: 2023-05-15 14:46:08
 * @Description: 
 */
#include "audio_play_api.h"
#include "cias_voice_plyer_handle.h"
#include "audio_play_api.h"
#include "cias_audio_data_handle.h"
#include "status_share.h"

media_play_resume_control media_play;
audio_play_os_stream_t tts_player = NULL;
audio_play_os_sem_t tts_player_end = NULL;

audio_play_os_stream_t mp3_player = NULL;
audio_play_os_sem_t mp3_player_end = NULL;

audio_play_os_stream_t m4a_player = NULL;
audio_play_os_sem_t m4a_player_end = NULL;

audio_play_os_stream_t wav_player = NULL;
audio_play_os_sem_t wav_player_end = NULL;

net_player_status_t tts_player_status = NET_PLAYER_IDLE;
net_player_status_t mp3_player_status = NET_PLAYER_IDLE;
net_player_status_t m4a_player_status = NET_PLAYER_IDLE;
net_player_status_t wav_player_status = NET_PLAYER_IDLE;

net_player_type_t net_play_type = PLAY_NULL; // 1 mp3  ;2 tts


static bool repeat_dialogue_flag = false;
static bool voice_invalid = false;
#if 1



/**
 * @brief Set the repeat dialogue flag object
 * 设置多轮对话标记
 *
 * @param val
 */
void set_repeat_dialogue_flag(bool val)
{
    repeat_dialogue_flag = val;
}

#if 1
/**
 * @brief Get the repeat dialogue flag object
 * 获取多轮对话标记
 *
 * @return true
 * @return false
 */
bool get_repeat_dialogue_flag(void)
{
    return repeat_dialogue_flag;
}

/**
 * @brief 设置语音无效标记
 *
 * @param val
 */
void set_voice_invalid(bool val)
{
    voice_invalid = val;
}

/**
 * @brief 获取语音无效标记
 *
 * @return true 无效
 * @return false 有效
 */
bool get_voice_invalid(void)
{
    return voice_invalid;
}


/**
 * @brief Set the net player status object
 *
 * @param net_player
 * @param val
 */
void set_net_player_status(net_player_type_t net_player, net_player_status_t val)
{

    if (net_player == PLAY_MP3)
    {
        mp3_player_status = val;
    }
    else if (net_player == PLAY_TTS)
    {
        tts_player_status = val;
    }
    else if (net_player == PLAY_M4A)
    {
        m4a_player_status = val;
    }
    else if (net_player == PLAY_WAV)
    {
        wav_player_status = val;
    }
    else
    {
        ci_logdebug(LOG_MEDIA, "err state\n");
    }
}

/**
 * @brief Get the net player status object
 *
 * @param net_player
 * @return net_player_status_t
 */
net_player_status_t get_net_player_status(net_player_type_t net_player)
{
    if (net_player == PLAY_MP3)
    {
        return mp3_player_status;
    }
    else if (net_player == PLAY_TTS)
    {
        return tts_player_status;
    }
    else if (net_player == PLAY_M4A)
    {
        return m4a_player_status;
    }
    else if (net_player == PLAY_WAV)
    {
        return wav_player_status;
    }
    else
    {
        return NET_PLAYER_IDLE;
    }
}


uint8_t next_resume_play_data[8] = {0};
static unsigned int music_data_offset = 0;
static unsigned int last_music_data_offset = 0;


unsigned int get_music_data_offset(void)
{
    return music_data_offset;
}

void set_music_data_offset(unsigned int offset)
{
    music_data_offset = offset;
}

uint8_t *get_next_resume_play_data(void)
{
    return next_resume_play_data;
}

/**
 * @brief Set the next resume play data object
 *
 * @param next_data NULL 清空buf
 */
void set_next_resume_play_data(uint8_t *next_data)
{
    if (next_data != NULL)
    {
        memcpy(next_resume_play_data, next_data, 8);
    }
    else
    {
        memset(next_resume_play_data, 0, 8);
    }
}

void stop_net_player(void)
{
    outside_send_end_sem(tts_player_end);
    outside_send_end_sem(mp3_player_end);
    outside_send_end_sem(m4a_player_end);
    outside_send_end_sem(wav_player_end);
}

void clear_net_player_stopsem(void)
{
    outside_clear_stream(NULL, m4a_player_end);
}


/**
 * @brief Set the net play type object
 *
 * @param val 1 mp3 2 tts
 */
void set_net_play_type(net_player_type_t val)
{
    if (val != PLAY_NULL)
    {
        send_exit_wakeup_msg(1);
    }
    net_play_type = val;
}

net_player_type_t get_net_play_type()
{
    return net_play_type;
}


static bool rev_voice_end_flag = false;
void mp3_player_end_callbk(int32_t play_cb_state)
{
    if (rev_voice_end_flag == false) //先收到0x0818 后收到0x0817
    {
        set_net_player_status(PLAY_MP3, NET_PLAYER_IDLE);
        set_net_play_type(PLAY_NULL);
        set_music_data_offset(get_play_offset());
        uint8_t next_data[8] = {0};
        if (RETURN_OK == get_near_offset_data(next_data))
        {
            set_next_resume_play_data(next_data);
            ci_logdebug(LOG_MEDIA, "next_data = 0x%x 0x%x 0x%x 0x%x ", next_data[0], next_data[1], next_data[2], next_data[3]);
            ci_logdebug(LOG_MEDIA, "0x%x 0x%x 0x%x 0x%x\n", next_data[4], next_data[5], next_data[6], next_data[7]);
        }
        if (AUDIO_PLAY_CB_STATE_INTERNAL_ERR == play_cb_state)
        {
            if ((get_music_data_offset() != 0) || (last_music_data_offset != 0))
            {
                cias_send_cmd(NET_PLAY_NEXT, DEF_FILL);
            }
            last_music_data_offset = get_music_data_offset();
            ci_logdebug(LOG_MEDIA, "INTERNAL_ERR\n");
        }
        else if (AUDIO_PLAY_CB_STATE_DONE == play_cb_state)
        {
            ci_logdebug(LOG_MEDIA, "STATE_DONE\n");
        }
        last_music_data_offset = get_music_data_offset();
        cias_send_cmd(NET_PLAY_RECONECT_URL, get_music_data_offset());
        if (AUDIO_PLAY_CB_STATE_PAUSE == play_cb_state)
        {
            ci_logdebug(LOG_MEDIA, "STATE_PAUSE\n");
            set_net_player_status(PLAY_MP3, NET_PLAYER_PAUSE);
        }
        // if(!get_is_wake_up())
        {
            cias_send_cmd(NET_PLAY_END, DEF_FILL);
        }

        //ci_logdebug(LOG_MEDIA, "MP3 play_end, change to bf *********************\n");
#if CIAS_AIOT_ENABLE
       // ciss_set(CI_SS_PLAY_TYPE,CI_SS_PLAY_TYPE_CMD_PROMPT);
#endif
        start_recv_flag = false;
         if((media_play.mp3_decode_fail)&&(!media_play.recv_media_stream_end))
        {
            media_play.mp3_decode_fail = false;
            ci_loginfo(LOG_USER, "\r\n NET_PLAY_RESTART=============\r\n");
            cias_send_cmd(NET_PLAY_RESTART, DEF_FILL);
        } 
    }
}

void m4a_player_end_callbk(int32_t play_cb_state)
{
#if USE_AAC_DECODER
#if CIAS_AIOT_ENABLE
    //ciss_set(CI_SS_PLAY_TYPE,CI_SS_PLAY_TYPE_CMD_PROMPT);
#endif
    play_cb_state = -1;
    set_net_player_status(PLAY_M4A, NET_PLAYER_IDLE);
    set_net_play_type(PLAY_NULL);
    set_music_data_offset(get_play_offset());
    uint8_t next_data[8] = {0};
    if (RETURN_OK == get_near_offset_data(next_data))
    {
        set_next_resume_play_data(next_data);
    }
    if (AUDIO_PLAY_CB_STATE_PARSE_M4A_MEM_ERR == play_cb_state)
    {
        cias_send_cmd(NET_PLAY_NEXT, DEF_FILL);
        last_music_data_offset = get_music_data_offset();
        ci_logdebug(LOG_MEDIA, "M4A_MEM_ERR\n");
        return;
    }
    else if (AUDIO_PLAY_CB_STATE_INTERNAL_ERR == play_cb_state)
    {
        if ((get_music_data_offset() != 0) || (last_music_data_offset != 0))
        {
            cias_send_cmd(NET_PLAY_NEXT, DEF_FILL);
        }
        last_music_data_offset = get_music_data_offset();
        //ci_logdebug(LOG_MEDIA, "M4A play_end\n");
        //ci_logdebug(LOG_MEDIA, "M4A play_end :....%d,,,,,%d\n", media_play.acc_decode_fail,media_play.recv_media_stream_end);
        if((media_play.acc_decode_fail)&&(!media_play.recv_media_stream_end))
        {
            media_play.acc_decode_fail = false;
            ci_loginfo(LOG_USER, "\r\n NET_PLAY_RESTART=============\r\n");
            cias_send_cmd(NET_PLAY_RESTART, DEF_FILL);
            change_acc_decode_size();
        }
        else if (media_play.recv_media_stream_end)
        {
            ci_loginfo(LOG_USER, "\r\n NET_PLAY_NEXT=============\r\n");
            cias_send_cmd(PLAY_NEXT, DEF_FILL);
        }
        return;
    }
    else if ((AUDIO_PLAY_CB_STATE_DONE == play_cb_state) || (AUDIO_PLAY_CB_STATE_PARSE_FILE_LEASTDATA_ERR == play_cb_state))
    {
        ci_logdebug(LOG_MEDIA, "STATE_DONE %d\n", play_cb_state);
    }
    last_music_data_offset = get_music_data_offset();
    cias_send_cmd(NET_PLAY_RECONECT_URL, get_music_data_offset());
    if (AUDIO_PLAY_CB_STATE_PAUSE == play_cb_state)
    {
        ci_logdebug(LOG_MEDIA, "STATE_PAUSE\n");
        set_net_player_status(PLAY_M4A, NET_PLAYER_PAUSE);
    }
    // if(!get_is_wake_up())
    {
        cias_send_cmd(NET_PLAY_END, DEF_FILL);
    }
#endif
}

void wav_player_end_callbk(int32_t play_cb_state)
{
    set_net_player_status(PLAY_WAV, NET_PLAYER_IDLE);
    set_net_play_type(PLAY_NULL);
    set_music_data_offset(get_play_offset());
    uint8_t next_data[8] = {0};
    if (RETURN_OK == get_near_offset_data(next_data))
    {
        set_next_resume_play_data(next_data);
        ci_logdebug(LOG_MEDIA, "next_data = 0x%x 0x%x 0x%x 0x%x ", next_data[0], next_data[1], next_data[2], next_data[3]);
        ci_logdebug(LOG_MEDIA, "0x%x 0x%x 0x%x 0x%x\n", next_data[4], next_data[5], next_data[6], next_data[7]);
    }
    if (AUDIO_PLAY_CB_STATE_INTERNAL_ERR == play_cb_state)
    {
        if ((get_music_data_offset() != 0) || (last_music_data_offset != 0))
        {
            cias_send_cmd(NET_PLAY_NEXT, DEF_FILL);
        }
        last_music_data_offset = get_music_data_offset();
        ci_logdebug(LOG_MEDIA, "INTERNAL_ERR\n");
        ci_logdebug(LOG_MEDIA, "WAV play_end\n");
        return;
    }
    else if (AUDIO_PLAY_CB_STATE_DONE == play_cb_state)
    {
        ci_logdebug(LOG_MEDIA, "STATE_DONE\n");
    }
    last_music_data_offset = get_music_data_offset();
    cias_send_cmd(NET_PLAY_RECONECT_URL, get_music_data_offset());
    if (AUDIO_PLAY_CB_STATE_PAUSE == play_cb_state)
    {
        ci_logdebug(LOG_MEDIA, "STATE_PAUSE\n");
        set_net_player_status(PLAY_WAV, NET_PLAYER_PAUSE);
    }
    // if(!get_is_wake_up())
    {
        cias_send_cmd(NET_PLAY_END, DEF_FILL);
    }
    ci_logdebug(LOG_MEDIA, "WAV play_end\n");
}

void tts_player_end_callbk(int32_t play_cb_state)
{
    set_net_play_type(PLAY_NULL);
    /*是否进入多轮对话*/
    if (get_repeat_dialogue_flag())
    {
        set_repeat_dialogue_flag(false);
        // enter_next_dialogue(EXIT_WAKEUP_TIME);
    }
    else if (get_net_player_status(PLAY_MP3) == NET_PLAYER_PAUSE)
    {
        // ci_logdebug(LOG_MEDIA,"resume MP3 start!\n");
        cias_send_cmd(NET_PLAY_RECONECT_URL, get_music_data_offset());
    }
    else if (get_net_player_status(PLAY_M4A) == NET_PLAYER_PAUSE)
    {
        // ci_logdebug(LOG_MEDIA,"resume M4a start!\n");
        cias_send_cmd(NET_PLAY_RECONECT_URL, get_music_data_offset());
    }
    else if (get_net_player_status(PLAY_WAV) == NET_PLAYER_PAUSE)
    {
        // ci_logdebug(LOG_MEDIA,"resume WAV start!\n");
        cias_send_cmd(NET_PLAY_RECONECT_URL, get_music_data_offset());
    }
    cias_send_cmd(NET_PLAY_END, DEF_FILL);
    set_net_player_status(PLAY_TTS, NET_PLAYER_IDLE);
    //ci_logdebug(LOG_MEDIA, "TTS play_end, change to bf *********************\n");
#if CIAS_AIOT_ENABLE
   // ciss_set(CI_SS_PLAY_TYPE,CI_SS_PLAY_TYPE_CMD_PROMPT);
#endif
    start_recv_flag = false;
    cias_send_cmd(PLAY_TTS_END, DEF_FILL);
    if((media_play.mp3_decode_fail)&&(!media_play.recv_media_stream_end))
    {
        media_play.mp3_decode_fail = false;
        ci_loginfo(LOG_USER, "\r\n NET_PLAY_RESTART=============\r\n");
        cias_send_cmd(NET_PLAY_RESTART, DEF_FILL);
    }
}

int32_t pause_net_music(bool from_isr_flag)
{
    BaseType_t pxHigherPriorityTaskWoken;
    int32_t ret = RETURN_ERR;
    if (check_current_playing())
    {
        if (get_net_player_status(PLAY_M4A) == NET_PLAYER_START)
        {
            if (from_isr_flag)
            {
                ret = pause_play(NULL, &pxHigherPriorityTaskWoken);
            }
            else
            {
                ret = pause_play(NULL, NULL);
            }
        }
    }
    return ret;
}


bool is_net_player(void)
{
#if AUDIO_PLAYER_ENABLE
    if (check_current_playing())
    {
        if (PLAY_NULL != get_net_play_type())
        {
            return true;
        }
    }
    return false;
#else
    return check_current_playing();
#endif
}
#endif
#endif

#if VOICE_PLAY_BY_UART
/****************
* 播报器参数初始化
* 
* * * **/
void audio_player_param_init()    
{
    outside_init_stream(&mp3_player, &mp3_player_end, IOT_AUDIO_PLAY_BUF_SIZE);
    if (!mp3_player)
        mprintf("mp3_player alloc error %x\n", mp3_player);
    else
        ci_loginfo(LOG_MEDIA, "mp3_player alloc success\n");
    set_curr_outside_handle(mp3_player, mp3_player_end);
}
#endif