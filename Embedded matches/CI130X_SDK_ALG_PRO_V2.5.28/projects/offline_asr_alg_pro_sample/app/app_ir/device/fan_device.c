#include "device.h"

#ifdef DEVICE_SUPPORT_FAN

#include "common_api.h"

/*按键定义*/
static stFanKey FanKey =
{
    .PowerOnKey  = {FAN_VOICE_POWER_ON, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
    .PowerOffKey = {FAN_VOICE_POWER_OFF, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
    .ShakKey     = {FAN_VOICE_SHAK, MAIN_VOICE_BEEP, 	.Nec = {0x00, 0x00, 0x00, 0x00},},
    .StopShakKey = {FAN_VOICE_STOP_SHAK, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
    .SpeedKey    = {FAN_VOICE_SPEED, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
    .TimeKey     = {FAN_VOICE_TIME, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
    .SpeedKey1     = {FAN_VOICE_SPEED_1, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
    .SpeedKey2     = {FAN_VOICE_SPEED_2, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
    .SpeedKey3     = {FAN_VOICE_SPEED_3, MAIN_VOICE_BEEP, .Nec = {0x00, 0x00, 0x00, 0x00},},
};

/*gloable*/
static stDevicePrivateData PrivateData = {0};
static stKey* CurrentKey = NULL;

static AsrCmd_Key_TypeDef fan_CmdKey[] =
{
    {FAN_ASR_OPEN_FAN,&FanKey.PowerOnKey},
    {FAN_ASR_CLOSE_FAN, &FanKey.PowerOffKey},
    {FAN_ASR_START_SHAKE, &FanKey.ShakKey},
    {FAN_ASR_STOP_SHAKE, &FanKey.StopShakKey},
    {FAN_ASR_SWITCH_SPEED, &FanKey.SpeedKey},
    {FAN_ASR_FAN_TIMER, &FanKey.TimeKey},
    {FAN_ASR_SPEED_1, &FanKey.SpeedKey1},
    {FAN_ASR_SPEED_2, &FanKey.SpeedKey2},
    {FAN_ASR_SPEED_3, &FanKey.SpeedKey3},
};

static int fan_init_key_fun_parm(void)
{
    return init_key_fun_parm((stKey*)&FanKey,PrivateData.keyNum);
}

/*初始化按键IR数据*/
static int fan_init_key(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if (NULL == pdevice)
    {
        goto out;
    }

    PrivateData.keyNum = sizeof(FanKey)/sizeof(stKey);
    PrivateData.cmdKeyNum = sizeof(fan_CmdKey)/sizeof(AsrCmd_Key_TypeDef);

    if (RETURN_OK != fan_init_key_fun_parm())
    {
        goto out;
    }

    /*按组处理IR数据*/
    ret = init_keydata((stKey*)(&FanKey), PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int fan_device_init(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if (NULL == pdevice)
    {
        goto out;
    }

    ret = fan_init_key(pdevice);
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

    ret = key_clean_IrCodeData((stKey *)(&FanKey), PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int fan_cleanIrCode(void)
{
    clean_IrCode();

    return RETURN_OK;
}

static int fan_device_getASR(stDevice* pDevice, uint32_t cmd_handle, int* device_id)
{
    uint32_t semantic_id = -1;

    if ((NULL == pDevice) || (NULL == device_id))
    {
        goto out;
    }

    semantic_id = cmd_info_get_semantic_id(cmd_handle);

    if (!((semantic_id >= FAN_ASR_MATCH_FAN) && (semantic_id <= FAN_ASR_MAX)))
    {
        goto out;
    }

    *device_id = get_fan_device()->device_index;

    if ((0 == PrivateData.power) && (semantic_id > FAN_ASR_CLOSE_FAN))
    {
        prompt_player_enable(ENABLE);
        pause_voice_in();
        prompt_play_by_semantic_id(FAN_VOICE_OPEN_FAN_FIRST, -1, default_play_done_callback, pdTRUE);
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN != pDevice->ir_mode) \
        && ((!pDevice->CheckKey) || ((RETURN_OK != pDevice->CheckKey(pDevice)) && (semantic_id >= FAN_ASR_OPEN_FAN))))
    {
        prompt_player_enable(ENABLE);
        pause_voice_in();
        prompt_play_by_semantic_id(FAN_VOICE_MATCH_FAN_FIRST, -1, default_play_done_callback, pdTRUE);
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

static int fan_device_playid(stDevice* pdevice, stAsrInfo* asr_msg)
{
    int ret = RETURN_ERR;
    stKey* pKey = NULL;

    if ((NULL == pdevice) || (NULL == asr_msg))
    {
        goto out;
    }

    // 处理从get_common_device传过来的任务
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
            case MAIN_ASR_LEARN_PREV_KEY:
                {
                    if ((PrivateData.learnKeyIndex > 0) && (PrivateData.learnKeyIndex <= (PrivateData.keyNum - 1)))
                    {
                        CurrentKey--;
                        PrivateData.learnKeyIndex--;

                        //play
                        if(CurrentKey->playVoice)
                        {
                            CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
                            key_clean_allkey_mode((stKey *)&FanKey, PrivateData.keyNum);//清除所有按键的学习标志位
                            CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                        }
                    }
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
                        key_clean_allkey_mode((stKey *)&FanKey, PrivateData.keyNum);//清除所有按键的学习标志位
                        CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                    }
                }
                break;
            default:
                break;
        }
    }
    //处理自己的asr_index
    else if (get_fan_device()->device_index == pdevice->device_index)
    {
        /*需要判断出asr_msg关联的是哪个key*/
        switch(asr_msg->semantic_id)
        {
            case FAN_ASR_MATCH_FAN:
                {
                 
                    PrivateData.learnKeyIndex = 0;
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
                    cmd_info_change_cur_model_group(2);
                    #endif

                    CurrentKey = (stKey *)&FanKey;
                    //play
                    if(CurrentKey->playVoice)
                    {
                        prompt_player_enable(ENABLE);
                        CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
                        key_clean_allkey_mode((stKey *)&FanKey, PrivateData.keyNum);//清除所有按键的学习标志位
                        CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                    }

                    ir_data_learn_ctl(IR_START_KEY_LEARN, NULL);
                }
                break;
            case FAN_ASR_OPEN_FAN:
            case FAN_ASR_CLOSE_FAN:
            case FAN_ASR_START_SHAKE:
            case FAN_ASR_STOP_SHAKE:
            case FAN_ASR_SWITCH_SPEED:
            case FAN_ASR_FAN_TIMER:
            case FAN_ASR_SPEED_1 :
            case FAN_ASR_SPEED_2 :
            case FAN_ASR_SPEED_3 :
                {
                    pKey = GetCmdKey(asr_msg->semantic_id, fan_CmdKey, PrivateData.cmdKeyNum);
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

                    if (asr_msg->semantic_id == FAN_ASR_OPEN_FAN)
                    {
                        PrivateData.power = 1;
                    }
                    else if (asr_msg->semantic_id == FAN_ASR_CLOSE_FAN)
                    {
                        PrivateData.power = 0;
                    }
                }
                break;
            case FAN_ASR_CLEAN_CODE:
                {
                    fan_cleanIrCode();
                    ret = AsrCmdPlayIdByMsg(asr_msg, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }
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

static int fan_device_learnIr(stDevice* pdevice, stKey* key, eDeviceMsgType type, void* data)
{
    int ret = RETURN_ERR;
    stKey* Tempkey = NULL;
    ir_data_t *ir_data = NULL;
    int asr_id = -1;

    DEVICE_LOCK();
    if ((NULL == pdevice) || (E_DEVICE_MODE_LEARN != pdevice->ir_mode) \
        || (get_fan_device()->device_index != pdevice->device_index))
    {
        goto out;
    }

    if (TYPE_DEVICEMSG_LEARN_KEY == type)
    {
        int *pReturn = (int *)data;
        if (5 == *pReturn)
        {
            mprintf("return timeout\n");
            goto out;
        }

        Tempkey = key_get_learnkey((stKey *)&FanKey, PrivateData.keyNum);
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
            key_clean_allkey_mode((stKey *)&FanKey, PrivateData.keyNum);//清除所有按键的学习标志位

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


            //play
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

            key_clean_allkey_mode((stKey *)&FanKey, PrivateData.keyNum);//清除所有按键的学习标志位
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

static int fan_device_checkKey(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if ((NULL == pdevice) || (pdevice->device_index != get_fan_device()->device_index))
    {
        goto out;
    }

    //目前只判断必学按键
    ret = checkAllLearnKey((stKey *)&FanKey, PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static stDevice fan_device =
{
    .Init = fan_device_init,//初始化相应设备
    .Deinit = NULL,//去初始化相应设备
    .GetAsrId = fan_device_getASR,//查找设备的ASR索引
    .PlayId = fan_device_playid,//播放相应处理
    .LearnIR = fan_device_learnIr,//学习IR按键
    .CheckKey = fan_device_checkKey,//检查按键学习状况
    .CleanIrData = fan_cleanIrCode,
};

stDevice* get_fan_device(void)
{
    return &fan_device;
}

#endif
