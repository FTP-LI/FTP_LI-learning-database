#include "device.h"
#include "common_api.h"
#include "ir_data.h"
#include "system_msg_deal.h"
#include "user_msg_deal.h"

#if (1 == PLAY_BEEP_CMD_EN)
#include "buzzer_voice.h"
#endif

static stAirAsrCmdTable AirAsrCmdTable[] =
{
    {AIR_ASR_OPEN_AIR, 5,},
    {AIR_ASR_CLOSE_AIR, 6,},
    {AIR_ASR_HIGHT_FAN, 7,},
    {AIR_ASR_MIND_FAN, 8,},
    {AIR_ASR_LOW_FAN, 9,},
    {AIR_ASR_AUTO_FAN, 10,},
    {AIR_ASR_CLOSE_SCAN, 11,},
    {AIR_ASR_OPEN_SCAN, 12,},
    {AIR_ASR_TEMP_19, 13,},
    {AIR_ASR_TEMP_20, 14,},
    {AIR_ASR_TEMP_21, 15,},
    {AIR_ASR_TEMP_22, 16,},
    {AIR_ASR_TEMP_23, 17,},
    {AIR_ASR_TEMP_24, 18,},
    {AIR_ASR_TEMP_25, 19,},
    {AIR_ASR_TEMP_26, 20,},
    {AIR_ASR_TEMP_27, 21,},
    {AIR_ASR_TEMP_28, 22,},
    {AIR_ASR_TEMP_29, 23,},
    {AIR_ASR_TEMP_30, 24,},
    {AIR_ASR_COOL_MODE, 25,},
    {AIR_ASR_HOT_MODE, 26,},
    {AIR_ASR_FAN_MODE, 27,},
    {AIR_ASR_WET_MODE, 28,},
    {AIR_ASR_AUTO_MODE, 29,},
    {AIR_ASR_TEMP_16, 30,},
    {AIR_ASR_TEMP_17, 31,},
    {AIR_ASR_TEMP_18, 32,},
    {AIR_ASR_UP_DOWN_SCAN, 68,},
    {AIR_ASR_LEFT_DOWN_SCAN, 69,},
    {AIR_ASR_UP_DOWN_SCAN_STOP, 70,},
    {AIR_ASR_LEFT_DOWN_SCAN_STOP, 71,},
    {AIR_ASR_TEMP_UP, 72,},
    {AIR_ASR_TEMP_DOWN, 73,},
    {AIR_ASR_FAN_UP, 74,},
    {AIR_ASR_FAN_DOWN, 75,},
    {AIR_ASR_SLEEP_CLOSE, 200,},
    {AIR_ASR_SLEEP_OPEN, 201,},
    {AIR_ASR_ECO_CLOSE, 208,},
    {AIR_ASR_ECO_OPEN, 103,},
    {AIR_ASR_OSD_CLOSE, 212,},
    {AIR_ASR_OSD_OPEN, 213,},
    {AIR_ASR_OPEN_AUXILIARY_HEAT, 102,},
    {AIR_ASR_CLOSE_AUXILIARY_HEAT, 206,},
    {AIR_ASR_OPEN_FRESH_AIR, 221,},
    {AIR_ASR_CLOSE_FRESH_AIR, 220,},
    {AIR_ASR_OPEN_WIND_TO_MAN, 205,},
    {AIR_ASR_CLOSE_WIND_TO_MAN, 204,},
    {AIR_ASR_SLEEP2_OPEN, 202,},
    {AIR_ASR_SLEEP3_OPEN, 203,},
    {AIR_ASR_OPEN_HEALTH, 211,},
    {AIR_ASR_CLOSE_HEALTH, 210,},
    {AIR_ASR_OPEN_MUTE_FAN, 215,},
    {AIR_ASR_CLOSE_MUTE_FAN, 214,},
    {AIR_ASR_OPEN_STRONG_FAN, 217,},
    {AIR_ASR_CLOSE_STRONG_FAN, 216,},
    {AIR_ASR_OPEN_SENSE, 219,},
    {AIR_ASR_CLOSE_SENSE, 218,},
};

int get_air_cmd_by_semantic_id(int semantic_id)
{
    int len = sizeof(AirAsrCmdTable)/sizeof(stAirAsrCmdTable);

    for (int i = 0; i < len; i++)
    {
        if (semantic_id == AirAsrCmdTable[i].semantic_id)
        {
            return AirAsrCmdTable[i].air_cmd;
        }
    }

    return -1;
}

int get_semantic_id_by_air_cmd(int air_cmd)
{
    int len = sizeof(AirAsrCmdTable)/sizeof(stAirAsrCmdTable);

    for (int i = 0; i < len; i++)
    {
        if (air_cmd == AirAsrCmdTable[i].air_cmd)
        {
            return AirAsrCmdTable[i].semantic_id;
        }
    }

    return -1;
}

static stAirKey AirKey =
{
    .PowerOnKey     = {AIR_VOICE_POWER_ON, MAIN_VOICE_BEEP},
    .PowerOffKey    = {AIR_VOICE_POWER_OFF, MAIN_VOICE_BEEP},
    .AutoModeKey    = {AIR_VOICE_AUTO_MODE, MAIN_VOICE_BEEP},
    .MoveWetKey     = {AIR_VOICE_MOVE_WET, MAIN_VOICE_BEEP},
    .CoolKey        = {AIR_VOICE_COOL_MODE, MAIN_VOICE_BEEP},
    .HotKey         = {AIR_VOICE_HOTE_MODE, MAIN_VOICE_BEEP},
    .SavePowerKey   = {AIR_VOICE_SAVE_POWER, MAIN_VOICE_BEEP},
    .TempUpKey      = {AIR_VOICE_TEMP_UP, MAIN_VOICE_BEEP},
    .TempDownKey    = {AIR_VOICE_TEMP_DOWN, MAIN_VOICE_BEEP},
    .HoriScanKey    = {AIR_VOICE_HORIZONTAL_FAN, MAIN_VOICE_BEEP},
    .VertScanKey    = {AIR_VOICE_VERTICAL_FAN, MAIN_VOICE_BEEP},
    .WindSpeedKey   = {AIR_VOICE_SWITCH_FAN_SPEED, MAIN_VOICE_BEEP},
};

static stKey* CurrentKey = NULL;
static stDevicePrivateData PrivateData = {0};
static stAirInfo stAirSta = {0};
static int TimerTick = 0;
extern void pause_voice_in(void);

static AsrCmd_Key_TypeDef air_CmdKey[] =
{
    {AIR_ASR_OPEN_AIR,  &AirKey.PowerOnKey},
    {AIR_ASR_CLOSE_AIR, &AirKey.PowerOffKey},
    {AIR_ASR_TEMP_UP,   &AirKey.TempUpKey},
    {AIR_ASR_TEMP_DOWN, &AirKey.TempDownKey},
    {AIR_ASR_AUTO_MODE, &AirKey.AutoModeKey},
    {AIR_ASR_WET_MODE,  &AirKey.MoveWetKey},
    {AIR_ASR_COOL_MODE, &AirKey.CoolKey},
    {AIR_ASR_HOT_MODE,  &AirKey.HotKey},
    {AIR_ASR_UP_DOWN_SCAN,      &AirKey.HoriScanKey},
    {AIR_ASR_UP_DOWN_SCAN_STOP, &AirKey.HoriScanKey},
    {AIR_ASR_LEFT_DOWN_SCAN,    &AirKey.VertScanKey},
    {AIR_ASR_LEFT_DOWN_SCAN_STOP,   &AirKey.VertScanKey},
    {AIR_ASR_HIGHT_FAN, &AirKey.WindSpeedKey},
    {AIR_ASR_MIND_FAN,  &AirKey.WindSpeedKey},
    {AIR_ASR_LOW_FAN,   &AirKey.WindSpeedKey},
    {AIR_ASR_ECO_CLOSE, &AirKey.SavePowerKey},
    {AIR_ASR_ECO_OPEN,  &AirKey.SavePowerKey},
};

static int air_init_key_fun_parm(void)
{
    return init_key_fun_parm((stKey *)&AirKey,PrivateData.keyNum);
}

/*初始化按键IR数据*/
static int air_init_key(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if (NULL == pdevice)
    {
        goto out;
    }

    PrivateData.keyNum = sizeof(AirKey)/sizeof(stKey);
    PrivateData.cmdKeyNum = sizeof(air_CmdKey)/sizeof(AsrCmd_Key_TypeDef);
    if (RETURN_OK != air_init_key_fun_parm())
    {
        goto out;
    }

    /*按组处理IR数据*/
    ret = init_keydata((stKey *)(&AirKey), PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

void search_over_play_done_callback(cmd_handle_t cmd_handle)
{
    resume_voice_in();
    DeviceClearnAllLearnMode();//清除所有设备学习模式
    cmd_info_change_cur_model_group(0);
}



static int air_search_send_callback(eAirSearchCbType CbType, int air_code_id)
{
    switch(CbType)
    {
        case SEARCH_CB_TYPE_ID:
        {
            update_awake_time();
            stAirSta.ir_code = air_code_id;
            //播放beep音，表示在发码
            common_PlayVoiceAsrIndex(MAIN_VOICE_BEEP, -1);
        }
        break;
        case SEARCH_CB_TYPE_AUTO_OVER_LOOP:
        {
            pause_voice_in();
            prompt_play_by_semantic_id(AIR_VOICE_SEARCH_COMPLETE, -1, search_over_play_done_callback, pdTRUE);
        }
        break;
        case SEARCH_CB_TYPE_ASR_STOP_LOOP:
        {

        }
        break;
    }
    return RETURN_OK;
}


static uint32_t tic_id = 0;
void search_play_done_callback(cmd_handle_t cmd_handle)
{

    ir_search_ctl ctl = {0};



    resume_voice_in();

    ctl.send_cnt = 5;
    ctl.timeout_ms = 5000;
    ctl.ir_search_send_callback = air_search_send_callback;

    stAirSta.air_mode = 2;


    if (AIR_ASR_SERCH_AIR_INDEX == tic_id)
    {
        ir_data_search_ctl(IR_SERCH_AIR_INDEX, &ctl);
       
    }
    else if (AIR_ASR_SERCH_AIR_BRAND == tic_id)
    {
  
        ir_data_search_ctl(IR_SERCH_AIR_BRAND, &ctl);
    }
     tic_id = 0;
}


static int air_device_init(stDevice* pdevice)
{
    int ret = RETURN_ERR;
    uint16_t real_len = 0;

    if (NULL == pdevice)
    {
        goto out;
    }

    // no this item .creat
    if(CINV_ITEM_UNINIT == cinv_item_init(NVDATA_ID_AIRINFO, sizeof(stAirInfo), &stAirSta))
    {
        stAirSta.ir_code = get_airc_brand_id(BRAND_GREE_MEDIA);//初始化为格力-美的-奥克斯-海尔-长虹空调
        cinv_item_write(NVDATA_ID_AIRINFO,sizeof(stAirInfo), &stAirSta);
        mprintf("first write\n");
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_AIRINFO, sizeof(stAirInfo), &stAirSta, &real_len))
    {
        goto out;
    }

    ret = air_init_key(pdevice);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    set_g_air_code_index(stAirSta.ir_code);

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int clean_IrCode(void)
{
    int ret = RETURN_ERR;

    ret = key_clean_IrCodeData((stKey *)(&AirKey), PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int save_deviceInfo(void)
{
    if(CINV_OPER_SUCCESS != cinv_item_write(NVDATA_ID_AIRINFO,sizeof(stAirInfo), &stAirSta))
    {
        mprintf("write error\n");
        while(1);
    }

    return RETURN_OK;
}

static int air_cleanIrCode(void)
{
    clean_IrCode();

    stAirSta.ir_code = 0;
    save_deviceInfo();

    return RETURN_OK;
}

static int air_device_checkMustLearnKey(void)
{
    if (stAirSta.ir_code < 1000)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int air_set_ir_code(int ir_code, unsigned int cmd_handle)
{
    //处理匹配遥控器的数据
    stAirSta.ir_code = ir_code;
    PrivateData.iMatchFailCnt = 0;
    PrivateData.power = 0;
    save_deviceInfo();
    if (PrivateData.bPlay)
    {
        pause_voice_in();
        prompt_play_by_cmd_handle(cmd_handle, -1, learn_done_play_callback, pdTRUE);
    }

    DeviceClearnAllLearnMode();
    #if (1 == PLAY_BEEP_CMD_EN)
    cmd_beep();
    #endif

    return RETURN_OK;
}

static int air_device_getASR(stDevice* pDevice, uint32_t cmd_handle, int* device_id)
{
    uint32_t semantic_id = -1;

    if ((NULL == pDevice) || (NULL == device_id))
    {
        goto out;
    }

    semantic_id = cmd_info_get_semantic_id(cmd_handle);

    if (!((semantic_id >= AIR_ASR_FAST_MATCH_AIR) && (semantic_id <= AIR_ASR_MAX)))
    {
        goto out;
    }

    *device_id = get_air_device()->device_index;

    if ((0 == PrivateData.power) && (((semantic_id > AIR_ASR_CLOSE_AIR) && (semantic_id < AIR_ASR_SET_LG)) \
        || ((semantic_id >= AIR_ASR_OPEN_AUXILIARY_HEAT) && (semantic_id <= AIR_ASR_CLOSE_SENSE))))
    {
        prompt_player_enable(ENABLE);
        pause_voice_in();
        prompt_play_by_semantic_id(AIR_VOICE_OPEN_AIR_FIRST, -1, default_play_done_callback, pdTRUE);
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN != pDevice->ir_mode) \
        && ((!pDevice->CheckKey) || ((RETURN_OK != pDevice->CheckKey(pDevice)) && ((semantic_id >= AIR_ASR_OPEN_AIR) && (semantic_id < AIR_ASR_SET_LG)))))
    {
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN == pDevice->ir_mode) \
        && (semantic_id != MAIN_ASR_EXIT_MATCH) \
        && (semantic_id != MAIN_ASR_LEARN_PREV_KEY) \
        && (semantic_id != MAIN_ASR_LEARN_NEXT_KEY) \
        && (!((semantic_id >= AIR_ASR_SET_LG) && (semantic_id < AIR_ASR_SET_END))) \
        && (!((semantic_id >= AIR_ASR_SERCH_AIR_INDEX) && (semantic_id <= AIR_ASR_SERCH_AIR_BRAND))) \
        )
    {
        goto out;
    }

    return RETURN_OK;

out:
    return RETURN_ERR;
}

#ifndef DEVICE_GET_IR_WITH_PLAY
/**
 * @brief 匹配空调播报完成，需要开始执行匹配命令
 *
 * @param cmd_handle 命令信息句柄
 */
void match_air_play_done_callback(cmd_handle_t cmd_handle)
{
    resume_voice_in();

    int TimeOutS = 100 ;//15 S
    ir_data_learn_ctl(IR_START_AIR_KEY_MATCH, &TimeOutS);

}

#endif

static int air_device_playid(stDevice* pdevice, stAsrInfo* asr_msg)
{
    int ret = RETURN_ERR;
    int PlayIndex = 0;
    int iCmdOkFlag = 0;
    stKey* pKey = NULL;

    if ((NULL == pdevice) || (NULL == asr_msg))
    {
        goto out;
    }

    PrivateData.semantic_id = asr_msg->semantic_id;
    PrivateData.cmd_handle = asr_msg->cmd_handle;
    PrivateData.bPlay = asr_msg->bPlay;

    if (AIR_ASR_LEARN_AIR == asr_msg->semantic_id)
    {
        ret = DeviceClearnAllLearnMode();//清除所有设备学习模式
        if (RETURN_OK != ret)
        {
            goto out;
        }

        ret = DeviceSetDeviceLearnMode(pdevice);//当前设置学习模式
        if (RETURN_OK != ret)
        {
            goto out;
        }

        #if USE_SEPARATE_WAKEUP_EN
        //切换成空调品牌模型
        cmd_info_change_cur_model_group(2);
        #endif

        CurrentKey = (stKey *)&AirKey;
        stAirSta.ir_code = 0;
        stAirSta.air_mode = 1;
        save_deviceInfo();

        //play
        if(CurrentKey->playVoice)
        {
            CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
            key_clean_allkey_mode((stKey *)&AirKey, PrivateData.keyNum);//清除所有按键的学习标志位
            CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
        }

        ir_data_learn_ctl(IR_START_KEY_LEARN, NULL);
    }

    // 处理从get_common_device传过来的asr_index
    if (get_main_device()->device_index == pdevice->device_index)
    {
        switch(asr_msg->semantic_id)
        {
            case MAIN_ASR_EXIT_MATCH:
                {
                    #if (1 == PLAY_BEEP_CMD_EN)
                    if (E_DEVICE_MODE_LEARN == get_air_device()->ir_mode)
                    {
                        exit_match_beep();
                    }
                    #endif

                    ret = DeviceClearnAllLearnMode();//清除所有设备学习模式
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    if (stAirSta.air_mode == 1)
                    {
                        ret = ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);
                    }
                    else if (stAirSta.air_mode == 0)
                    {
                        ret = ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    else if (stAirSta.air_mode == 2)
                    {
                        ret = ir_data_search_ctl(IR_STOP_SERCH_AIR_INDEX, NULL);
                        stAirSta.air_mode = 0;
                        save_deviceInfo();
                    }

                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    PrivateData.learnKeyIndex = 0;
                    PrivateData.iMatchFailCnt = 0;
                }
                break;
            case MAIN_ASR_LEARN_PREV_KEY:
                {
                    if ((stAirSta.air_mode != 1) || (NULL == CurrentKey))
                    {
                        goto out;
                    }

                    if ((PrivateData.learnKeyIndex > 0) && (PrivateData.learnKeyIndex <= (PrivateData.keyNum - 1)))
                    {
                        CurrentKey--;
                        PrivateData.learnKeyIndex--;
                        //play
                        if(CurrentKey->playVoice)
                        {
                            CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
                            key_clean_allkey_mode((stKey *)&AirKey, PrivateData.keyNum);//清除所有按键的学习标志位
                            CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                        }
                    }
                }
                break;
            case MAIN_ASR_LEARN_NEXT_KEY:
                {
                    if ((stAirSta.air_mode != 1) || (NULL == CurrentKey))
                    {
                        goto out;
                    }

                    CurrentKey->deleteIR(CurrentKey);
                    if ((PrivateData.learnKeyIndex >= 0) && (PrivateData.learnKeyIndex < (PrivateData.keyNum - 1)))
                    {
                        CurrentKey++;
                        PrivateData.learnKeyIndex++;

                        //play
                        if(CurrentKey->playVoice)
                        {
                            CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
                        }

                        key_clean_allkey_mode((stKey *)&AirKey, PrivateData.keyNum);//清除所有按键的学习标志位
                        CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                    }
                    else if (PrivateData.learnKeyIndex >= (PrivateData.keyNum - 1))
                    {
                        if (RETURN_OK != DeviceClearnAllLearnMode())//清除所有设备学习模式
                        {
                            goto out;
                        }

                        key_clean_allkey_mode((stKey *)&AirKey, PrivateData.keyNum);//清除所有按键的学习标志位

                        CurrentKey = NULL;
                        PrivateData.learnKeyIndex = 0;
                        PrivateData.iMatchFailCnt = 0;
                        ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);

                        pause_voice_in();
                        prompt_play_by_semantic_id(AIR_VOICE_MATCH_SUCCESS, -1, learn_done_play_callback , pdTRUE);
                        main_device_overLearnIR(pdTRUE , 1);
                    }
                }
                break;
            default:
                break;
        }

    }
    //处理自己的asr_index
    else if(get_air_device()->device_index == pdevice->device_index)
    {
        if (AIR_ASR_FAST_MATCH_AIR == asr_msg->semantic_id)
        {
            stAirSta.air_mode = 0;
        }

        if (stAirSta.air_mode == 0)
        {
            switch(asr_msg->semantic_id)
            {
                case AIR_ASR_FAST_MATCH_AIR:
                    {
                     	PrivateData.learnKeyIndex = 0;
                        if( RETURN_OK != DeviceClearnAllLearnMode())
                        {
                            goto out;
                        }

                        if( RETURN_OK != DeviceSetDeviceLearnMode(pdevice))
                        {
                            goto out;
                        }

                        PrivateData.iMatchFailCnt = 0;


                        #ifdef DEVICE_GET_IR_WITH_PLAY

                        int TimeOutS = 100 ;//15 S
                        ret = ir_data_learn_ctl(IR_START_AIR_KEY_MATCH, &TimeOutS);
                        if (RETURN_OK != ret)
                        {
                            goto out;
                        }

                        prompt_player_enable(ENABLE);
                        pause_voice_in();
                        prompt_play_by_semantic_id(asr_msg->semantic_id, -1, default_play_done_callback, pdTRUE);
                        #else
                        pause_voice_in();
                        prompt_play_by_semantic_id(asr_msg->semantic_id, -1, match_air_play_done_callback, pdTRUE);
                        #endif

                        #if USE_SEPARATE_WAKEUP_EN
                        //切换成空调品牌模型
                        cmd_info_change_cur_model_group(2);
                        #endif
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif

                    }
                    break;
                case AIR_ASR_OPEN_AIR:
                case AIR_ASR_CLOSE_AIR:
                case AIR_ASR_HIGHT_FAN:
                case AIR_ASR_MIND_FAN:
                case AIR_ASR_LOW_FAN:
                case AIR_ASR_AUTO_FAN:
                case AIR_ASR_CLOSE_SCAN:
                case AIR_ASR_OPEN_SCAN:
                case AIR_ASR_TEMP_16:
                case AIR_ASR_TEMP_17:
                case AIR_ASR_TEMP_18:
                case AIR_ASR_TEMP_19:
                case AIR_ASR_TEMP_20:
                case AIR_ASR_TEMP_21:
                case AIR_ASR_TEMP_22:
                case AIR_ASR_TEMP_23:
                case AIR_ASR_TEMP_24:
                case AIR_ASR_TEMP_25:
                case AIR_ASR_TEMP_26:
                case AIR_ASR_TEMP_27:
                case AIR_ASR_TEMP_28:
                case AIR_ASR_TEMP_29:
                case AIR_ASR_TEMP_30:
                case AIR_ASR_COOL_MODE:
                case AIR_ASR_HOT_MODE:
                case AIR_ASR_FAN_MODE:
                case AIR_ASR_WET_MODE:
                case AIR_ASR_AUTO_MODE:
                case AIR_ASR_UP_DOWN_SCAN:
                case AIR_ASR_LEFT_DOWN_SCAN:
                case AIR_ASR_UP_DOWN_SCAN_STOP:
                case AIR_ASR_LEFT_DOWN_SCAN_STOP:
                    {
                        int ir_cmd = get_air_cmd_by_semantic_id(asr_msg->semantic_id);
                        if (ir_cmd < 0)
                        {
                            goto out;
                        }
                        if (AIR_ASR_CLOSE_AIR == asr_msg->semantic_id)
                        {
                            PrivateData.power = 0;
                            TimerTick = 0;
                        }
                        else if (AIR_ASR_OPEN_AIR == asr_msg->semantic_id)
                        {
                            PrivateData.power = 1;
                            normal_status_t * new_state = get_air_normal_status_val();
                            new_state->mode = 1;//1->制冷
                            save_air_normal_status_val(new_state);
                        }

                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, asr_msg->semantic_id - AIR_ASR_FAST_MATCH_AIR);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;

                case AIR_ASR_SLEEP_CLOSE:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 200);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_SLEEP_OPEN:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 201);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_ECO_CLOSE:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 208);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_ECO_OPEN:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 103);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_OSD_CLOSE:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 212);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_OSD_OPEN:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 213);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TEMP_UP:
                case AIR_ASR_TEMP_DOWN:
                case AIR_ASR_FAN_UP:
                case AIR_ASR_FAN_DOWN:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, asr_msg->semantic_id - AIR_ASR_FAST_MATCH_AIR);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_CLEAN_CODE:
                    {
                        air_cleanIrCode();
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_CANCEL:
                    {
                        TimerTick = 0;
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_1H:
                    {
                        TimerTick = (1 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_2H:
                    {
                        TimerTick = (2 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_3H:
                    {
                        TimerTick = (3 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_4H:
                    {
                        TimerTick = (4 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_5H:
                    {
                        TimerTick = (5 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_6H:
                    {
                        TimerTick = (6 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_7H:
                    {
                        TimerTick = (7 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_8H:
                    {
                        TimerTick = (8 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_9H:
                    {
                        TimerTick = (9 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_TIME_10H:
                    {
                        TimerTick = (10 * 60 * 60)/(DEVICE_TIME/1000);
                        #if (1 == PLAY_BEEP_CMD_EN)
                        cmd_beep();
                        #endif
                        if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
                        {
                            goto out;
                        }
                    }
                    break;

                case AIR_ASR_SET_LG:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_LG), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_TCL:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_TCL), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_AUX:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_AUX), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_CHIGO:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_CHIGO), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_PANASONIC:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_PANASONIC), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_GREE:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_GREE), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_HISENSE:
                case AIR_ASR_SET_KELON:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_HISENSE), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_HAIER:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_HAIER), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_MEDIA:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_MEDIA), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_CHANGHONG:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_CHANGHONG), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;

                case AIR_ASR_SET_HITACHI:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_HITACHI), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_DAIKIN:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_DAIKIN), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_MITSUBISHI:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_MITSUBISHI), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_MI:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_MI), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_WHIRLPOOL:
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_WHIRLPOOL), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;

                case AIR_ASR_SET_GALANZ:       //格兰仕
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_GALANZ), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_FUJITSU:      //富士通
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_FUJITSU), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_SANSHUI:      //山水
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_SANSHUI), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_YORK:         //约克
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_YORK), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_SKYWORTH:     //创维
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_SKYWORTH), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_SHINCO:       //新科
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_SHINCO), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_CHUNLAN:      //春兰
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_CHUNLAN), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_CHEBLO:       //樱花
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_CHEBLO), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_SAMSUNG:      //三星
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_SAMSUNG), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_AUCMA:        //澳柯玛
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_AUCMA), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_XINFEI:       //新飞
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_XINFEI), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_TOSHIBA:      //东芝
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_TOSHIBA), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_SAMPO:        //声宝
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_SAMPO), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_YUETU:        //月兔
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_YUETU), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_YAIR:         //扬子
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_YAIR), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_MOYE:         //夏新
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_AMOI), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_SET_SHAIP:         //夏普
                    {
                        air_set_ir_code(get_airc_brand_id(BRAND_SHARP), asr_msg->cmd_handle);
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                    }
                    break;
                case AIR_ASR_OPEN_AUXILIARY_HEAT:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 102);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_CLOSE_AUXILIARY_HEAT:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 206);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_OPEN_FRESH_AIR:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 221);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_CLOSE_FRESH_AIR:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 220);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_OPEN_WIND_TO_MAN:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 223);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;
                case AIR_ASR_CLOSE_WIND_TO_MAN:
                    {
                        ret = ir_data_Air_Send_Ctl(stAirSta.ir_code, 222);
                        if (RETURN_ERR == ret)
                        {
                            goto out;
                        }
                    }
                    break;

                case AIR_ASR_SERCH_AIR_INDEX :
                case AIR_ASR_SERCH_AIR_BRAND :
                    {
                        ir_data_learn_ctl(IR_STOP_AIR_KEY_MATCH, NULL);
                        pause_voice_in();
                        tic_id = asr_msg->semantic_id;
                        prompt_play_by_cmd_handle(asr_msg->cmd_handle, -1, search_play_done_callback, pdTRUE);
                    }
                    break;
                default:
                    break;
            }
        }
        else if (E_DEVICE_MODE_NORMAL == get_air_device()->ir_mode)
        {
            if (AIR_ASR_OPEN_AIR == asr_msg->semantic_id)
            {
                PrivateData.power = 1;
                iCmdOkFlag = 1;
            }
            else if (AIR_ASR_CLOSE_AIR == asr_msg->semantic_id)
            {
                PrivateData.power = 0;
                iCmdOkFlag = 1;
            }

            switch(asr_msg->semantic_id)
            {
                case AIR_ASR_OPEN_AIR:
                case AIR_ASR_CLOSE_AIR:
                case AIR_ASR_TEMP_UP:
                case AIR_ASR_TEMP_DOWN:
                case AIR_ASR_AUTO_MODE:
                case AIR_ASR_WET_MODE:
                case AIR_ASR_COOL_MODE:
                case AIR_ASR_HOT_MODE:
                case AIR_ASR_UP_DOWN_SCAN:
                case AIR_ASR_UP_DOWN_SCAN_STOP:
                case AIR_ASR_LEFT_DOWN_SCAN:
                case AIR_ASR_LEFT_DOWN_SCAN_STOP:
                case AIR_ASR_HIGHT_FAN:
                case AIR_ASR_MIND_FAN:
                case AIR_ASR_LOW_FAN:
                case AIR_ASR_ECO_CLOSE:
                case AIR_ASR_ECO_OPEN:
                    iCmdOkFlag = 1;
                    break;
                case AIR_ASR_CLEAN_CODE:
                    air_cleanIrCode();
                    AsrCmdPlayIdByMsg(asr_msg , -1);
                    break;
                default:
                    break;
            }

            if (iCmdOkFlag != 1)
            {
                goto out;
            }
            pKey = GetCmdKey(asr_msg->semantic_id, air_CmdKey, PrivateData.cmdKeyNum);
            if (NULL == pKey)
            {
                goto out;
            }

            ret = pKey->sendIR(pKey);
            if (RETURN_OK != ret)
            {
                goto out;
            }

            if (RETURN_OK != AsrCmdPlayIdByMsg(asr_msg , -1))
            {
                goto out;
            }
        }

    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;

}

static int air_device_learnIr(stDevice* pdevice, stKey* key, eDeviceMsgType type, void* data)
{
    int ret = RETURN_ERR;
    int ir_code = -1;
    int asr_id = -1;
    stKey* Tempkey = NULL;
    ir_data_t *ir_data = NULL;

    DEVICE_LOCK();

    if ((NULL == pdevice) || (E_DEVICE_MODE_LEARN != pdevice->ir_mode) \
        || (get_air_device()->device_index != pdevice->device_index))
    {
        goto out;
    }

    if ((TYPE_DEVICEMSG_AIR_MATCH == type) && (stAirSta.air_mode == 0))
    {
        /*学习失败播报*/
        if (!data)
        {
            goto out;
        }

        ir_code = *(int *)data;
        if (ir_code < 1000)
        {
            //time out ,目前不播报超时
            if (1 == ir_code)
            {
                PrivateData.iMatchFailCnt = 0;
                goto out;
            }

            if (PrivateData.iMatchFailCnt < 2)
            {
                int TimeOutS = 15 ;//15 S
                #ifdef PLAY_OTHER_CMD_EN
                common_PlayVoiceAsrIndex(AIR_VOICE_MATCH_FAIL1 + PrivateData.iMatchFailCnt, -1);
                #endif
                ir_data_learn_ctl(IR_START_AIR_KEY_MATCH, &TimeOutS);

                #if (1 == PLAY_BEEP_CMD_EN)
                match_fail_beep();
                #endif
            }
            else
            {
                ret = DeviceClearnAllLearnMode();//清除所有设备学习模式
                if (RETURN_OK != ret)
                {
                    goto out;
                }

                #if 0
                prompt_play_info2_t p_play_info[] = {
                    {AIR_VOICE_MATCH_FAIL3, -1},
                    {MAIN_ASR_EXIT_MATCH, -1},
                };

                pause_voice_in();
                prompt_play_by_multi_semantic_id((prompt_play_info2_t *)&p_play_info, sizeof(p_play_info)/sizeof(prompt_play_info2_t),  learn_done_play_callback);
                #else
                pause_voice_in();
                prompt_play_by_semantic_id(AIR_VOICE_MATCH_FAIL3, -1, default_play_done_callback, true);
                pause_voice_in();
                prompt_play_by_semantic_id(MAIN_ASR_EXIT_MATCH, -1, learn_done_play_callback, false);
                #endif

                #if (1 == PLAY_BEEP_CMD_EN)
                exit_match_beep();
                #endif

            }

            PrivateData.iMatchFailCnt++;

            goto out;
        }

        //处理匹配遥控器的数据

        stAirSta.ir_code = ir_code;
        PrivateData.iMatchFailCnt = 0;
        PrivateData.power = 0;
        save_deviceInfo();

        pause_voice_in();
        prompt_play_by_semantic_id(AIR_VOICE_MATCH_SUCCESS, -1, learn_done_play_callback , pdTRUE);

        #if (1 == PLAY_BEEP_CMD_EN)
        match_success_beep();
        #endif

        ret = DeviceClearnAllLearnMode();
        if (RETURN_OK != ret)
        {
            goto out;
        }

    }
    else if ((TYPE_DEVICEMSG_LEARN_KEY == type) && (stAirSta.air_mode == 1))
    {
        int *pReturn = (int *)data;
        if (5 == *pReturn)
        {
            mprintf("return timeout\n");
            goto out;
        }

        Tempkey = key_get_learnkey((stKey *)&AirKey, PrivateData.keyNum);
        if (NULL == Tempkey)
        {
            goto out;
        }

        CurrentKey = key;
        ir_data = (ir_data_t *)data;

        /*学习失败播报*/
        if (RETURN_OK != main_CheckGetLearnStatus(Tempkey, ir_data))
        {
            goto out;
        }

        ret = Tempkey->storeIR(Tempkey, ir_data);
        if (RETURN_OK != ret)
        {
            goto out;
        }

        //判断是否到最后一个按键
        if (PrivateData.learnKeyIndex >= (PrivateData.keyNum - 1))
        {
            //需要处理最后一个key，播报完成匹配
            key_clean_allkey_mode((stKey *)&AirKey, PrivateData.keyNum);//清除所有按键的学习标志位

            #if 0
            prompt_play_info2_t p_play_info[] = {
                {Tempkey->iplay_get_irdata, -1},
                {AIR_VOICE_MATCH_SUCCESS, -1},
            };

            pause_voice_in();
            prompt_play_by_multi_semantic_id((prompt_play_info2_t *)&p_play_info, sizeof(p_play_info)/sizeof(prompt_play_info2_t),  default_play_done_callback);
            #else
            pause_voice_in();
            prompt_play_by_semantic_id(Tempkey->iplay_get_irdata, -1, default_play_done_callback, true);
            pause_voice_in();
            prompt_play_by_semantic_id(AIR_VOICE_MATCH_SUCCESS, -1, learn_done_play_callback, false);
            #endif



            #if (1 == PLAY_BEEP_CMD_EN)
            cmd_beep();
            #endif

            ret = DeviceClearnAllLearnMode();//清除所有设备学习模式
            if (RETURN_OK != ret)
            {
                goto out;
            }

            PrivateData.iMatchFailCnt = 0;

            ret = ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);
            if (RETURN_OK != ret)
            {
                goto out;
            }

            PrivateData.learnKeyIndex = 0;
        }
        else
        {
            CurrentKey = Tempkey + 1;
            PrivateData.learnKeyIndex++;

            #if 0
            prompt_play_info2_t p_play_info[] = {
                {Tempkey->iplay_get_irdata, -1},
                {CurrentKey->iplay_befor_normal, -1},
            };

            pause_voice_in();
            prompt_play_by_multi_semantic_id((prompt_play_info2_t *)&p_play_info, sizeof(p_play_info)/sizeof(prompt_play_info2_t),  default_play_done_callback);
            #else
            pause_voice_in();
            prompt_play_by_semantic_id(Tempkey->iplay_get_irdata, -1, default_play_done_callback, true);
            pause_voice_in();
            prompt_play_by_semantic_id(CurrentKey->iplay_befor_normal, -1, default_play_done_callback, false);
            #endif

            key_clean_allkey_mode((stKey *)&AirKey, PrivateData.keyNum);//清除所有按键的学习标志位
            CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位

            ir_data_learn_ctl(IR_START_KEY_LEARN, NULL);

        }
    }
    else
    {
        goto out;
    }

    DEVICE_UNLOCK();
    return RETURN_OK;

out:
    Errprint();
    DEVICE_UNLOCK();
    return RETURN_ERR;

}

int air_device_send_callback(eDeviceMsgType type, int callback_msg)
{
    switch(type)
    {
        case TYPE_DEVICEMSG_AIRC_CHECK_CALLBACK:
            {
                //检测能执行响应的动作的回调
                if (callback_msg >= 0)
                {
                    int semantic_id = get_semantic_id_by_air_cmd(callback_msg);
                    if (-1 == semantic_id)
                    {
                        break;
                    }

                    if (PrivateData.bPlay)
                    {
                        if (PrivateData.semantic_id == semantic_id)
                        {
                            pause_voice_in();
                            prompt_play_by_cmd_handle(PrivateData.cmd_handle, -1, default_play_done_callback, pdTRUE);
                        }
                        else
                        {
                            pause_voice_in();
                            prompt_play_by_semantic_id(semantic_id, -1, default_play_done_callback, pdTRUE);
                        }
                    }
                    #if (1 == PLAY_BEEP_CMD_EN)
                    cmd_beep();
                    #endif
                    user_led_light_flash();
                }
            }
            break;
        case TYPE_DEVICEMSG_AIRC_SEND_CALLBACK:
            {
                //发送完成后的回调
            }
            break;
        default:
            break;


    }
    return 0;
}


static int air_device_checkKey(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if ((NULL == pdevice) || (pdevice->device_index != get_air_device()->device_index))
    {
        goto out;
    }

    //目前只判断必学按键
    if (0 == stAirSta.air_mode)
    {
        ret = air_device_checkMustLearnKey();
        if (RETURN_OK != ret)
        {
            goto out;
        }

    }
    else
    {
        ret = checkAllLearnKey((stKey *)&AirKey, PrivateData.keyNum);
        if (RETURN_OK != ret)
        {
            goto out;
        }
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

int air_CheckTimer(void)
{
    if ((0 != TimerTick) && (1 == PrivateData.power))
    {
        TimerTick--;
        mprintf("TimerTick: %d分:%d秒\n" ,TimerTick*(DEVICE_TIME/1000)/60, TimerTick*(DEVICE_TIME/1000)%60);
        if ((0 == TimerTick) && (1 == PrivateData.power))
        {
            ir_data_Air_Send_Ctl(stAirSta.ir_code, AIR_ASR_CLOSE_AIR - AIR_ASR_FAST_MATCH_AIR);
            PrivateData.power = 0;
        }
    }

    return RETURN_OK;
}

void air_ctl(int bOpen, cmd_handle_t cmd_handle)
{
    PrivateData.bPlay = 1;
    PrivateData.cmd_handle = cmd_handle;

    if (0 == bOpen)
    {
        PrivateData.power = 0;
        TimerTick = 0;
        ir_data_Air_Send_Ctl(stAirSta.ir_code, AIR_ASR_CLOSE_AIR - AIR_ASR_FAST_MATCH_AIR);
    }
    else
    {
        PrivateData.power = 1;
        normal_status_t * new_state = get_air_normal_status_val();
        new_state->mode = 1;//1->制冷
        save_air_normal_status_val(new_state);
        ir_data_Air_Send_Ctl(stAirSta.ir_code, AIR_ASR_OPEN_AIR - AIR_ASR_FAST_MATCH_AIR);
    }

}


static stDevice air_device =
{
    .Init = air_device_init,//初始化相应设备
    .GetAsrId = air_device_getASR,//查找设备的ASR索引
    .PlayId = air_device_playid,//播放相应处理
    .LearnIR = air_device_learnIr,//学习IR按键
    .CheckKey = air_device_checkKey,//检查按键学习状况
    .CleanIrData = air_cleanIrCode,
};

stDevice* get_air_device(void)
{
    return &air_device;
}


/**
 * @brief 设置空调设备匹配ID
 *
 * @param ir_code 红外ID
 */
void set_air_device_match_id(int ir_code)
{
    stAirSta.ir_code = ir_code;
}



