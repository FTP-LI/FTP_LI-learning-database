#include "device.h"

#ifdef DEVICE_SUPPORT_TV
#include "tv_device.h"
#include "common_api.h"

#define AUTO_SCAN_CHANNELS_TIMEOUT  (5*60*1000)
#define MATCH_KEY_WAITE_VOICE_TIME  (1000)
/*按键定义*/
static stTvKey TvKey =
{
    .StbPowerOnKey      = {TV_VOICE_STB_POWER_ON, MAIN_VOICE_BEEP},
    .StbPowerOffKey     = {TV_VOICE_STB_POWER_OFF, MAIN_VOICE_BEEP},
    .NextProgKey        = {TV_VOICE_NEXT_PROG, MAIN_VOICE_BEEP},
    .PrevProgKey        = {TV_VOICE_PREV_PROG, MAIN_VOICE_BEEP},
    .TvPowerOnKey       = {TV_VOICE_TV_POWER_ON, MAIN_VOICE_BEEP},
    .TvPowerOffKey      = {TV_VOICE_TV_POWER_OFF, MAIN_VOICE_BEEP},
    .TvSoureKey         = {TV_VOICE_SORUCE, MAIN_VOICE_BEEP},
    .MuteKey            = {TV_VOICE_MUTE, MAIN_VOICE_BEEP},
    .TvVolUpKey         = {TV_VOICE_TV_VOL_UP, MAIN_VOICE_BEEP},
    .TvVolDownKey       = {TV_VOICE_TV_VOL_DOWN, MAIN_VOICE_BEEP},

    .StbMenuKey         = {TV_VOICE_STB_MENU, MAIN_VOICE_BEEP},
    .StbMenuConfirmeKey = {TV_VOICE_STB_MENU_CONFIRME, MAIN_VOICE_BEEP},
    .StbMenuBackKey     = {TV_VOICE_STB_MENU_BACK, MAIN_VOICE_BEEP},

    .upKey         = {TV_VOICE_UP_C, MAIN_VOICE_BEEP},
    .downKey         = {TV_VOICE_DOWN_C, MAIN_VOICE_BEEP},
    .leftKey         = {TV_VOICE_LEFT_C, MAIN_VOICE_BEEP},
    .rightKey         = {TV_VOICE_RIGHT_C, MAIN_VOICE_BEEP},
};

/*gloable*/
static stDevicePrivateData PrivateData = {0};
static stKey* CurrentKey = NULL;

static AsrCmd_Key_TypeDef tv_CmdKey[] =
{
    {TV_ASR_OPEN_TV,&TvKey.TvPowerOnKey},
    {TV_ASR_CLOSE_TV, &TvKey.TvPowerOffKey},
    {TV_ASR_PREV_PROG, &TvKey.PrevProgKey},
    {TV_ASR_NEXT_PROG,&TvKey.NextProgKey},
    {TV_ASR_UP_TV_VOL, &TvKey.TvVolUpKey},
    {TV_ASR_DOWN_TV_VOL, &TvKey.TvVolDownKey},
    {TV_ASR_SWITCH_SOURCE, &TvKey.TvSoureKey},
    {TV_ASR_MUTE_VOL,&TvKey.MuteKey},

    {TV_ASR_OPEN_STB,&TvKey.StbPowerOnKey},
    {TV_ASR_CLOSE_STB, &TvKey.StbPowerOffKey},
    {TV_ASR_STB_MENU, &TvKey.StbMenuKey},
    {TV_ASR_STB_MENU_CONFIRME, &TvKey.StbMenuConfirmeKey},
    {TV_ASR_STB_BACK, &TvKey.StbMenuBackKey},

    {TV_ASR_UP_C, &TvKey.upKey},
    {TV_ASR_DOWN_C, &TvKey.downKey},
    {TV_ASR_LEFT_C, &TvKey.leftKey},
    {TV_ASR_RIGHT_C, &TvKey.rightKey},
};

extern void change_wakeup_period(uint32_t exit_wakup_ms);

static int tv_init_key_fun_parm(void)
{
    return init_key_fun_parm((stKey*)(&TvKey),PrivateData.keyNum);
}

/*初始化按键IR数据*/

static int tv_init_key(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if (NULL == pdevice)
    {
        goto out;
    }

    PrivateData.keyNum = sizeof(TvKey)/sizeof(stKey);
    PrivateData.cmdKeyNum = sizeof(tv_CmdKey)/sizeof(AsrCmd_Key_TypeDef);

    if (RETURN_OK != tv_init_key_fun_parm())
    {
        goto out;
    }

    ret = init_keydata((stKey *)&TvKey, PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int tv_device_init(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if (NULL == pdevice)
    {
        goto out;
    }

    ret = tv_init_key(pdevice);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int clean_IrCode(void)
{
    int ret = RETURN_ERR;

    ret = key_clean_IrCodeData((stKey *)&TvKey, PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int tv_cleanIrCode(void)
{
    clean_IrCode();

    return RETURN_OK;
}

static int tv_ContinueKeySendDown(stKey* pKey)
{
    if (pKey->key_id == TvKey.NextProgKey.key_id)
    {
        change_wakeup_period(EXIT_WAKEUP_TIME);
    }

    return 0;
}


static int tv_device_getASR(stDevice* pDevice, uint32_t cmd_handle, int* device_id)
{
    uint32_t semantic_id = -1;

    if ((NULL == pDevice) || (NULL == device_id))
    {
        goto out;
    }

    semantic_id = cmd_info_get_semantic_id(cmd_handle);

    if (!((semantic_id >= TV_ASR_MATCH_TV) && (semantic_id <= TV_ASR_MAX)))
    {
        goto out;
    }

    *device_id = get_tv_device()->device_index;

    if ((NULL == pDevice) || ((0 == PrivateData.power) && (semantic_id > TV_ASR_CLOSE_STB)))
    {
        prompt_player_enable(ENABLE);
        pause_voice_in();
        prompt_play_by_semantic_id(TV_VOICE_OPEN_FAN_FIRST, -1, default_play_done_callback, pdTRUE);
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN != pDevice->ir_mode) \
        && ((!pDevice->CheckKey) || ((RETURN_OK != pDevice->CheckKey(pDevice)) && (semantic_id >= TV_ASR_OPEN_TV))))
    {
        prompt_player_enable(ENABLE);
        pause_voice_in();
        prompt_play_by_semantic_id(TV_VOICE_MATCH_FAN_FIRST, -1, default_play_done_callback, pdTRUE);
        goto out;
    }

    if ((DEVICE_MODE_TV != main_GetDeviceMode()) && (semantic_id == TV_ASR_MAX))//本身没有几个命令词  所以最后用TV_ASR_MAX代替，本行不起任何作用
    {
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN == pDevice->ir_mode) \
        && (semantic_id != MAIN_ASR_EXIT_MATCH) \
        && (semantic_id != MAIN_ASR_LEARN_PREV_KEY) \
        && (semantic_id != MAIN_ASR_LEARN_NEXT_KEY) \
        )
    {
        goto out;
    }

    return RETURN_OK;

out:
    return RETURN_ERR;
}

static int tv_device_playid(stDevice* pdevice, stAsrInfo* asr_msg)
{
    int ret = RETURN_ERR;
    stKey* pKey = NULL;
    int iGrupKeyCnt = 0;

    if ((NULL == pdevice) || (NULL == asr_msg))
    {
        goto out;
    }

    // 处理从get_common_device传过来的asr_index
    if (get_main_device()->device_index == pdevice->device_index)
    {
        switch(asr_msg->semantic_id)
        {
            case MAIN_ASR_EXIT_MATCH:
                {
                    ret = DeviceClearnAllLearnMode();//清除所有设备学习模式
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    ret = ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    CurrentKey = NULL;
                    PrivateData.learnKeyIndex = 0;
                }
                break;
            case MAIN_ASR_LEARN_NEXT_KEY:
                {
                    CurrentKey->deleteIR(CurrentKey);
                    if ((PrivateData.learnKeyIndex >= 0) && (PrivateData.learnKeyIndex < (PrivateData.keyNum - 1)))
                    {
                        CurrentKey++;
                        PrivateData.learnKeyIndex++;
                    }
                    else if (PrivateData.learnKeyIndex >= (PrivateData.keyNum - 1))
                    {
                        DeviceClearnAllLearnMode();//清除所有设备学习模式

                        CurrentKey = NULL;
                        PrivateData.learnKeyIndex = 0;

                        ret = ir_data_learn_ctl(IR_STOP_KEY_LEARN, NULL);
                        if (RETURN_OK != ret)
                        {
                            goto out;
                        }

                        pause_voice_in();
                        prompt_play_by_semantic_id(AIR_VOICE_MATCH_SUCCESS, -1, learn_done_play_callback , pdTRUE);
                    }

                    //play
                    if(CurrentKey->playVoice)
                    {
                        CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
                        key_clean_allkey_mode((stKey *)&TvKey, PrivateData.keyNum);//清除所有按键的学习标志位
                        CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                    }
                }
                break;
            default:
                break;
        }

    }
    //处理自己的asr_index
    else if(get_tv_device()->device_index == pdevice->device_index)
    {
        /*需要判断出asr_msg关联的是哪个key*/
        switch(asr_msg->semantic_id)
        {
            case TV_ASR_MATCH_TV:
                {
                    PrivateData.learnKeyIndex = 0;
                    ret = DeviceClearnAllLearnMode();//清除所有设备学习模式
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    ret = DeviceSetDeviceLearnMode(pdevice);//设置学习模式
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    CurrentKey = (stKey *)&TvKey;

                    #if USE_SEPARATE_WAKEUP_EN
                    //切换成空调品牌模型
                    cmd_info_change_cur_model_group(2);
                    #endif

                    //play
                    if(CurrentKey->playVoice)
                    {
                        CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
                        key_clean_allkey_mode((stKey *)&TvKey, PrivateData.keyNum);//清除所有按键的学习标志位
                        CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                    }

                    ir_data_learn_ctl(IR_START_KEY_LEARN, NULL);
                }
                break;
            case TV_ASR_OPEN_TV:
            case TV_ASR_CLOSE_TV:
            case TV_ASR_SWITCH_SOURCE:
            case TV_ASR_OPEN_STB:
            case TV_ASR_CLOSE_STB:
            case TV_ASR_STB_MENU:
            case TV_ASR_STB_MENU_CONFIRME:
            case TV_ASR_STB_BACK:
            case TV_ASR_MUTE_VOL:
            case TV_ASR_UP_C:
            case TV_ASR_DOWN_C:
            case TV_ASR_LEFT_C:
            case TV_ASR_RIGHT_C:
            case TV_ASR_PREV_PROG:
            case TV_ASR_NEXT_PROG:
            case TV_ASR_UP_TV_VOL://增大一格音量
                {
                    pKey = GetCmdKey(asr_msg->semantic_id, tv_CmdKey, PrivateData.cmdKeyNum);
                    if (NULL == pKey)
                    {
                        goto out;
                    }

                    ret = pKey->sendIR(pKey);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    ret = AsrCmdPlayIdByMsg(asr_msg, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    if (asr_msg->semantic_id == TV_ASR_OPEN_TV)
                    {
                        PrivateData.power = 1;
                    }
                    else if (asr_msg->semantic_id == TV_ASR_CLOSE_TV)
                    {
                        PrivateData.power = 0;

                        delete_all_ContinueSendKey_list();
                    }
                }
                break;
            case TV_ASR_DOWN_TV_VOL://减少1格音量
                {
                    if (0 == PrivateData.power)
                    {
                        goto out;
                    }

                    pKey = GetCmdKey(asr_msg->semantic_id, tv_CmdKey, PrivateData.cmdKeyNum);
                    if (NULL == pKey)
                    {
                        goto out;
                    }
                    if (FLAG_NO_STORE == pKey->checkIRLearn(pKey))
                    {
                        common_PlayVoiceAsrIndex(MAIN_VOICE_NO_IR_DATA, -1);
                        goto out;
                    }

                    ret = AsrCmdPlayIdByMsg(asr_msg, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    if (RETURN_OK != check_key_in_ContinueSendKey_list(pKey))
                    {
                        stContinueSendKey ContinueSendKey;

                        //循环发送赋值
                        memset(&ContinueSendKey, 0x00, sizeof(stContinueSendKey));
                        ContinueSendKey.offset_TimeMS = 0;
                        ContinueSendKey.pKey = pKey;
                        ContinueSendKey.send_key_TimeMS = 1000;
                        ContinueSendKey.send_cnt.totle_cnt = 1;
                        ContinueSendKey.send_cnt.cnt = 0;
                        ContinueSendKey.send_cnt.bFlag = 1;

                        add_to_ContinueSendKey_list(&ContinueSendKey);
                    }
                }
                break;
            case TV_ASR_CLEAN_CODE:
                {
                    tv_cleanIrCode();
                    ret = AsrCmdPlayIdByMsg(asr_msg, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }
                }
                break;
            case TV_ASR_AUTO_SWITCH_CHANNEL:
                {
                    if (0 == PrivateData.power)
                    {
                        goto out;
                    }

                    pKey = GetCmdKey(TV_ASR_NEXT_PROG, tv_CmdKey, PrivateData.cmdKeyNum);

                    if (NULL == pKey)
                    {
                        goto out;
                    }

                    if (FLAG_NO_STORE == pKey->checkIRLearn(pKey))
                    {
                        common_PlayVoiceAsrIndex(MAIN_VOICE_NO_IR_DATA, -1);
                        goto out;
                    }

                    ret = AsrCmdPlayIdByMsg(asr_msg, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    if (RETURN_OK != check_key_in_ContinueSendKey_list(pKey))
                    {

                        stContinueSendKey ContinueSendKey;

                        //循环发送赋值
                        memset(&ContinueSendKey, 0x00, sizeof(stContinueSendKey));
                        ContinueSendKey.offset_TimeMS = 0;
                        ContinueSendKey.pKey = pKey;
                        ContinueSendKey.send_key_TimeMS = 5000;
                        ContinueSendKey.send_timer.timer = 0;
                        ContinueSendKey.send_timer.totle_timer = AUTO_SCAN_CHANNELS_TIMEOUT;
                        ContinueSendKey.send_timer.bFlag = 1;
                        ContinueSendKey.ContinueKeySendDown = tv_ContinueKeySendDown;

                        add_to_ContinueSendKey_list(&ContinueSendKey);

                        change_wakeup_period(AUTO_SCAN_CHANNELS_TIMEOUT);
                    }
                }
                break;
            case TV_ASR_STOP_AUTO_SWITCH_CHANNEL:
                {
                    if (0 == PrivateData.power)
                    {
                        goto out;
                    }

                    pKey = GetCmdKey(TV_ASR_NEXT_PROG, tv_CmdKey, PrivateData.cmdKeyNum);

                    if (NULL == pKey)
                    {
                        goto out;
                    }

                    ret = AsrCmdPlayIdByMsg(asr_msg, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    stop_key_in_ContinueSendKey_list(pKey);
                }
                break;
            default:
                break;
        }
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;

}

static int tv_device_learnIr(stDevice* pdevice, stKey* key, eDeviceMsgType type, void* data)
{
    int ret = RETURN_ERR;
    stKey* Tempkey = NULL;
    ir_data_t *ir_data = NULL;
    int asr_id = -1;
    DEVICE_LOCK();

    if ((NULL == pdevice) || (E_DEVICE_MODE_LEARN != pdevice->ir_mode) \
        || (get_tv_device()->device_index != pdevice->device_index))
    {
        goto out;
    }

    if (TYPE_DEVICEMSG_LEARN_KEY == type)
    {
        int *pReturn = (int *)data;
        if (5 == *pReturn)
        {
            mprintf("timeout\n");
            goto out;
        }

        Tempkey = key_get_learnkey((stKey *)&TvKey, PrivateData.keyNum);
        if (NULL == Tempkey)
        {
            goto out;
        }

        CurrentKey = Tempkey;
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
            //需要处理最后一个key，
            key_clean_allkey_mode((stKey *)&TvKey, PrivateData.keyNum);//清除所有按键的学习标志位

            #if 0
            prompt_play_info2_t p_play_info[] = {
                {Tempkey->iplay_get_irdata, -1},
                {AIR_VOICE_MATCH_SUCCESS, -1},
            };

            pause_voice_in();
            prompt_play_by_multi_semantic_id((prompt_play_info2_t *)&p_play_info, sizeof(p_play_info)/sizeof(prompt_play_info2_t),  learn_done_play_callback);
            #else
            pause_voice_in();
            prompt_play_by_semantic_id(Tempkey->iplay_get_irdata, -1, default_play_done_callback, true);
            pause_voice_in();
            prompt_play_by_semantic_id(AIR_VOICE_MATCH_SUCCESS, -1, learn_done_play_callback, false);
            #endif

            ret = DeviceClearnAllLearnMode();//清除所有设备学习模式
            if (RETURN_OK != ret)
            {
                goto out;
            }

            CurrentKey = NULL;

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

            //play
            key_clean_allkey_mode((stKey *)&TvKey, PrivateData.keyNum);//清除所有按键的学习标志位
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

static int tv_device_checkKey(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if ((NULL == pdevice) || (pdevice->device_index != get_tv_device()->device_index))
    {
        goto out;
    }

    //目前只判断必学按键
    ret = checkAllLearnKey((stKey *)&TvKey, PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}


static stDevice tv_device =
{
    .Init = tv_device_init,//初始化相应设备
    .GetAsrId = tv_device_getASR,//查找设备的ASR索引
    .PlayId = tv_device_playid,//播放相应处理
    .LearnIR = tv_device_learnIr,//学习IR按键
    .CheckKey = tv_device_checkKey,//检查按键学习状况
    .CleanIrData = tv_cleanIrCode,
};

stDevice* get_tv_device(void)
{
    return &tv_device;
}

#endif //DEVICE_SUPPORT_TV
