#include "device.h"
#ifdef DEVICE_SUPPORT_LIGHT
#include "light_device.h"
#include "common_api.h"

/*按键定义*/
static stLightKey LightKey =
{
    .PowerOnKey     = {LIGHT_VOICE_POWER_ON, MAIN_VOICE_BEEP},
    .PowerOffKey    = {LIGHT_VOICE_POWER_OFF, MAIN_VOICE_BEEP},
    .BrightKey      = {LIGHT_VOICE_BRIGHT, MAIN_VOICE_BEEP},
    .DarkKey        = {LIGHT_VOICE_DARK, MAIN_VOICE_BEEP},
    .SwitchKey      = {LIGHT_VOICE_SWITCH_LIGHT, MAIN_VOICE_BEEP},
};


/*gloable*/
static stDevicePrivateData PrivateData = {0};
static stKey* CurrentKey = NULL;

static AsrCmd_Key_TypeDef light_CmdKey[] =
{
    {LIGHT_ASR_OPEN_LIGHT,&LightKey.PowerOnKey},
    {LIGHT_ASR_CLOSE_LIGHT, &LightKey.PowerOffKey},
    {LIGHT_ASR_BRIGHT_LIGHT, &LightKey.BrightKey},
    {LIGHT_ASR_DARK_LIGHT,&LightKey.DarkKey},
    {LIGHT_ASR_SWITCH_LIGHT,&LightKey.SwitchKey},

};

static int light_init_key_fun_parm(void)
{
    return init_key_fun_parm((stKey *)(&LightKey),PrivateData.keyNum);
}

static int light_init_key(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if (NULL == pdevice)
    {
        goto out;
    }

    PrivateData.keyNum = sizeof(LightKey)/sizeof(stKey);
    PrivateData.cmdKeyNum = sizeof(light_CmdKey)/sizeof(AsrCmd_Key_TypeDef);

    if (RETURN_OK != light_init_key_fun_parm())
    {
        goto out;
    }


    /*按组处理IR数据*/
    ret = init_keydata((stKey *)(&LightKey), PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int light_device_init(stDevice* pdevice)
{
    int ret = RETURN_ERR;

    if (NULL == pdevice)
    {
        goto out;
    }

    ret = light_init_key(pdevice);
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

    ret = key_clean_IrCodeData((stKey *)(&LightKey), PrivateData.keyNum);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int light_cleanIrCode(void)
{
    clean_IrCode();

    return RETURN_OK;
}

static int light_device_getASR(stDevice* pDevice, uint32_t cmd_handle, int* device_id)
{
    uint32_t semantic_id = -1;

    if ((NULL == pDevice) || (NULL == device_id))
    {
        goto out;
    }

    semantic_id = cmd_info_get_semantic_id(cmd_handle);


    if (!((semantic_id >= LIGHT_ASR_MATCH_LIGHT) && (semantic_id <= LIGHT_ASR_MAX)))
    {
        goto out;
    }

    *device_id = get_light_device()->device_index;

    if ((0 == PrivateData.power) && (semantic_id > LIGHT_ASR_CLOSE_LIGHT))
    {
        prompt_player_enable(ENABLE);
        pause_voice_in();
        prompt_play_by_semantic_id(LIGHT_VOICE_OPEN_FAN_FIRST, -1, default_play_done_callback, pdTRUE);
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN != pDevice->ir_mode) \
        && ((!pDevice->CheckKey) || ((RETURN_OK != pDevice->CheckKey(pDevice)) && (semantic_id >= LIGHT_ASR_OPEN_LIGHT))))
    {
        prompt_player_enable(ENABLE);
        pause_voice_in();
        prompt_play_by_semantic_id(LIGHT_VOICE_MATCH_FAN_FIRST, -1, default_play_done_callback, pdTRUE);
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


static int light_device_playid(stDevice* pdevice, stAsrInfo* asr_msg)
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
                            key_clean_allkey_mode((stKey *)&LightKey, PrivateData.keyNum);//清除所有按键的学习标志位
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
                        key_clean_allkey_mode((stKey *)&LightKey, PrivateData.keyNum);//清除所有按键的学习标志位
                        CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                    }
                }
                break;
            default:
                break;
        }
    }

    //处理自己的asr_index
    else if (get_light_device()->device_index == pdevice->device_index)
    {
        /*需要判断出asr_msg关联的是哪个key*/
        switch(asr_msg->semantic_id)
        {
            case LIGHT_ASR_MATCH_LIGHT:
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

                    #if USE_SEPARATE_WAKEUP_EN
                    cmd_info_change_cur_model_group(2);
                    #endif

                    CurrentKey = (stKey *)(&LightKey);

                    //play
                    if(CurrentKey->playVoice)
                    {
                        CurrentKey->playVoice(CurrentKey, CurrentKey->iplay_befor_normal);
                        key_clean_allkey_mode((stKey *)&LightKey, PrivateData.keyNum);//清除所有按键的学习标志位
                        CurrentKey->keymode = KEY_MODE_LEARN;//设置当前KEY的学习标志位
                    }

                    ir_data_learn_ctl(IR_START_KEY_LEARN, NULL);
                }
                break;
            case LIGHT_ASR_OPEN_LIGHT:
            case LIGHT_ASR_CLOSE_LIGHT:
            case LIGHT_ASR_BRIGHT_LIGHT:
            case LIGHT_ASR_DARK_LIGHT:
            case LIGHT_ASR_SWITCH_LIGHT:

                {
                    pKey = GetCmdKey(asr_msg->semantic_id, light_CmdKey, PrivateData.cmdKeyNum);
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

                    if (asr_msg->semantic_id == LIGHT_ASR_OPEN_LIGHT)
                    {
                        PrivateData.power = 1;
                    }
                    else if (asr_msg->semantic_id == LIGHT_ASR_CLOSE_LIGHT)
                    {
                        PrivateData.power = 0;
                    }
                }
                break;
            case LIGHT_ASR_CLEAN_CODE:
                {
                    light_cleanIrCode();
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


static int light_device_learnIr(stDevice* pdevice, stKey* key, eDeviceMsgType type, void* data)
{
    int ret = RETURN_ERR;
    stKey* Tempkey = NULL;
    ir_data_t *ir_data = NULL;
    int asr_id = -1;

    DEVICE_LOCK();
    if ((NULL == pdevice) || (E_DEVICE_MODE_LEARN != pdevice->ir_mode) \
        || (get_light_device()->device_index != pdevice->device_index))
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

        Tempkey = key_get_learnkey((stKey *)&LightKey, PrivateData.keyNum);
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
            key_clean_allkey_mode((stKey *)&LightKey, PrivateData.keyNum);//清除所有按键的学习标志位

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
            key_clean_allkey_mode((stKey *)&LightKey, PrivateData.keyNum);//清除所有按键的学习标志位
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


static int light_device_checkKey(stDevice* pdevice)
{
    if ((NULL == pdevice) || (pdevice->device_index != get_light_device()->device_index))
    {
        goto out;
    }

     //目前只判断必学按键
    if (RETURN_OK != checkAllLearnKey((stKey *)&LightKey, PrivateData.keyNum))
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static stDevice light_device =
{
    .Init = light_device_init,//初始化相应设备
    .Deinit = NULL,//去初始化相应设备
    .GetAsrId = light_device_getASR,//查找设备的ASR索引
    .PlayId = light_device_playid,//播放相应处理
    .LearnIR = light_device_learnIr,//学习IR按键
    .CheckKey = light_device_checkKey,//检查按键学习状况
    .CleanIrData = light_cleanIrCode,
};

stDevice* get_light_device(void)
{
    return &light_device;
}

#endif //DEVICE_SUPPORT_LIGHT
