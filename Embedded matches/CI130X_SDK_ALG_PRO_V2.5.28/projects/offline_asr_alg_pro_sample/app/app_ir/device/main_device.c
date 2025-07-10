#include "device.h"
#include "common_api.h"
#include "system_msg_deal.h"

static int BrightNess[] = {25, 50, 75, 100};
static stMainStatus stBabySta = {
    .iWakeUpIndex = 5,
};

typedef struct
{
    unsigned char bLightInit;
}stTempStatus;

static stTempStatus temp_status = {0};
static int save_deviceInfo(void)
{
    if(CINV_OPER_SUCCESS != cinv_item_write(NVDATA_ID_MAININFO,sizeof(stMainStatus), &stBabySta))
    {
        mprintf("write error\n");
        while(1);
    }

    return RETURN_OK;
}

static int main_device_Init(stDevice* pdevice)
{
    uint16_t real_len = 0;

    if (NULL == pdevice)
    {
        goto out;
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_MAININFO, sizeof(stMainStatus), &stBabySta, &real_len))
    {
        stBabySta.night_light_level = 2;
        //first write
        if(CINV_ITEM_UNINIT == cinv_item_init(NVDATA_ID_MAININFO, sizeof(stMainStatus), &stBabySta))
        {
            mprintf("first write\n");
        }
    }
    get_flash_light_state();
    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static int main_device_getASR(stDevice* pDevice, uint32_t cmd_handle, int* device_id)
{
    stDevice * pTempDevice = NULL;
    uint32_t semantic_id = -1;

    semantic_id = cmd_info_get_semantic_id(cmd_handle);
    if ((NULL == pDevice) || (NULL == device_id))
    {
        goto out;
    }

    if (!((semantic_id >= MAIN_ASR_START) && (semantic_id <= MAIN_ASR_MAX)))
    {
        goto out;
    }

    *device_id = get_main_device()->device_index;

    return RETURN_OK;

out:
    return RETURN_ERR;
}


int DeviceDealASRMsg(stAsrInfo* pAsrInfo)
{
    int ret = RETURN_ERR;
    stDevice *device_Now = NULL;

    if (NULL == pAsrInfo)
    {
        goto out;
    }

    /*调用命令词所在device的接口处理*/
    device_Now = findDevicebyID(pAsrInfo->device_id);

    if (NULL == device_Now)
    {
        goto out;
    }

    if (device_Now->PlayId)
    {
        ret = device_Now->PlayId(device_Now, pAsrInfo);
        if (RETURN_OK != ret)
        {
            goto out;
        }
    }
    else
    {
        mprintf("no common device and private device func\n");
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

int getLevelCnt(void)
{
    return sizeof(BrightNess)/sizeof(int);
}

int getBrightLevelValue(int level)
{
    if (level >= getLevelCnt())
    {
        return 0;
    }

    return BrightNess[level];
}

/* 根据cmd_info里面是否有<打开小夜灯>来决定是否初始化功能*/
int common_device_InitNightLevel(void)
{
    night_light_init();
    temp_status.bLightInit = 1;

    return RETURN_OK;
}

int night_light_onoff=0;
/* 存储夜灯状态*/
void set_flash_light_state(int gt_wtn)
{
    night_light_onoff = gt_wtn;
    cinv_item_write(NVDATA_ID_NIGHT_STATE,sizeof(night_light_onoff), &night_light_onoff);
    
}

void get_flash_light_state(void)
{
   uint16_t real_len = 0;
   if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_NIGHT_STATE, sizeof(night_light_onoff), &night_light_onoff, &real_len))
    {

        //first write
        if(CINV_ITEM_UNINIT == cinv_item_init(NVDATA_ID_NIGHT_STATE, sizeof(night_light_onoff), &night_light_onoff))
        {
            mprintf("first write\n");
        }
    }
    //mprintf("wake_number %d\n",night_light_onoff);
    if(night_light_onoff == 1) //打开小夜灯
    {
        night_light_set_brightness(1 , getBrightLevelValue(stBabySta.night_light_level));
                stBabySta.LightPower = 1;
    }
    else //关闭小夜灯 
    {
          night_light_set_brightness(0 , getBrightLevelValue(stBabySta.night_light_level));
                stBabySta.LightPower = 0;
    }

}

static int device_SetNightLevel(int level)
{
    int status = 0;

    if (1 != stBabySta.LightPower)
    {
        status = 2;
        goto out;
    }

    if ((level < 0) || (level >= getLevelCnt()))
    {
        status = 1;
        goto out;
    }

    night_light_set_brightness(1 , getBrightLevelValue(level));
    stBabySta.night_light_level = level;
    save_deviceInfo();

out:
    return status;
}

static int main_device_SetNightLevelUp(void)
{
    return device_SetNightLevel(stBabySta.night_light_level + 1);
}

static int main_device_SetNightLevelDown(void)
{
    return device_SetNightLevel(stBabySta.night_light_level - 1);
}

eDeviceModeCtl main_GetDeviceMode(void)
{
    //实现获取当前是电视还是机顶盒
    return stBabySta.eTvStbSwitch;
}

static int main_device_baby_ctl(uint32_t asr_index, int* PlayIndex)
{
    if (NULL == PlayIndex)
    {
        goto out;
    }

    *PlayIndex = 0;

    switch(asr_index)
    {
        case MAIN_ASR_OPEN_LIGHT:
            {
                night_light_set_brightness(1 , getBrightLevelValue(stBabySta.night_light_level));
 set_flash_light_state(1);
                stBabySta.LightPower = 1;
            }
            break;
        case MAIN_ASR_CLOSE_LIGHT:
            {
                night_light_set_brightness(0 , getBrightLevelValue(stBabySta.night_light_level));
set_flash_light_state(0);
                stBabySta.LightPower = 0;
            }
            break;
        case MAIN_ASR_LIGHT_UP:
            {
                *PlayIndex = main_device_SetNightLevelUp();
            }
            break;
        case MAIN_ASR_LIGHT_DOWN:
            {
                *PlayIndex = main_device_SetNightLevelDown();
            }
            break;

        case MAIN_ASR_LOWST_LIGHT:
            {
                *PlayIndex = device_SetNightLevel(0);
            }
            break;
        case MAIN_ASR_HIGHT_LIGHT:
            {
                *PlayIndex = device_SetNightLevel(getLevelCnt() - 1);
            }
            break;
        case MAIN_ASR_BIGER_VOL:
            {
                uint8_t volume;
                uint16_t real_len;
                if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len))
                {
                    volume = VOLUME_DEFAULT;
                    cinv_item_init(NVDATA_ID_VOLUME, sizeof(volume), &volume);
                }

                if (volume >= VOLUME_MAX)
                {
                    *PlayIndex = 1;
                }
                else
                {
                    volume++;
                    vol_set(volume);
                    *PlayIndex = 0;
                }
            }
            break;
        case MAIN_ASR_SMALLER_VOL:
            {
                uint8_t volume;
                uint16_t real_len;
                if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_VOLUME, sizeof(volume), &volume, &real_len))
                {
                    volume = VOLUME_DEFAULT;
                    cinv_item_init(NVDATA_ID_VOLUME, sizeof(volume), &volume);
                }

                if (volume <= VOLUME_MIN)
                {
                    *PlayIndex = 1;
                }
                else
                {
                    volume--;
                    vol_set(volume);
                    *PlayIndex = 0;
                }

            }
            break;
        case MAIN_ASR_MAX_VOL:
            {
                vol_set(VOLUME_MAX);
                *PlayIndex = 0;
            }
            break;
        case MAIN_ASR_MIN_VOL:
            {
                vol_set(VOLUME_MIN);
                *PlayIndex = 0;
            }
            break;
        default:
            break;

    }

    return RETURN_OK;

out:
    return RETURN_ERR;
}

int main_device_overLearnIR(int iNeedPlay, int iPlayId)
{
    int ret = RETURN_ERR;
    stDevice* CurrentLearnDevice = NULL;
    stAsrInfo AsrInfo = {0};

    AsrInfo.bPlay = iNeedPlay;
    AsrInfo.semantic_id = MAIN_ASR_EXIT_MATCH;
    AsrInfo.cmd_handle = cmd_info_find_command_by_id(AsrInfo.semantic_id);
    AsrInfo.device_id = get_main_device()->device_index;

    CurrentLearnDevice = DeviceGetLearnModeDevice();
    if ((NULL == CurrentLearnDevice) || (NULL == CurrentLearnDevice->PlayId))
    {
        goto out;
    }

    ret = CurrentLearnDevice->PlayId(get_main_device(), &AsrInfo);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    ret = DeviceClearnAllLearnMode();
    if (RETURN_OK != ret)
    {
        goto out;
    }

    pause_voice_in();
    learn_done_play_callback(AsrInfo.cmd_handle);

    return RETURN_OK;

out:
    return RETURN_ERR;

}


static int main_device_playid(stDevice* pdevice, stAsrInfo* pAsrInfo)
{
    int PlayIndex = -1;
    int ret = RETURN_ERR;
    stDevice* CurrentLearnDevice = NULL;

    if ((NULL == pdevice) || (get_main_device()->device_index != pdevice->device_index) || (NULL == pAsrInfo))
    {
        goto out;
    }

    if ((pdevice->device_index != get_main_device()->device_index) \
        || (pdevice->device_index != pAsrInfo->device_id))
    {
        goto out;
    }

    CurrentLearnDevice = DeviceGetLearnModeDevice();

    switch(pAsrInfo->semantic_id)
    {
        case MAIN_ASR_EXIT_MATCH:
            {

                if ((NULL != CurrentLearnDevice) && (CurrentLearnDevice->PlayId))
                {
                    ret = CurrentLearnDevice->PlayId(pdevice, pAsrInfo);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }

                    pause_voice_in();
                    prompt_play_by_cmd_handle(pAsrInfo->cmd_handle, -1, learn_done_play_callback, pdTRUE);

                }
                else
                {
                    goto out;
                }

                ret = DeviceClearnAllLearnMode();
                if (RETURN_OK != ret)
                {
                    goto out;
                }

            }
            break;
        case MAIN_ASR_LEARN_NEXT_KEY:
        case MAIN_ASR_LEARN_PREV_KEY:
            {
                if ((NULL != CurrentLearnDevice) && (CurrentLearnDevice->PlayId))
                {
                    ret = CurrentLearnDevice->PlayId(pdevice, pAsrInfo);
                }
            }
            break;
        case MAIN_ASR_OPEN_LIGHT:
        case MAIN_ASR_CLOSE_LIGHT:
            {
                if (0 == temp_status.bLightInit)
                {
                    goto out;
                }

                if (RETURN_OK != main_device_baby_ctl(pAsrInfo->semantic_id, &PlayIndex))
                {
                    goto out;
                }

                ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;
        case MAIN_ASR_BIGER_VOL:
            {
                if (RETURN_OK != main_device_baby_ctl(pAsrInfo->semantic_id, &PlayIndex))
                {
                    goto out;
                }

                if (0 == PlayIndex)
                {
                    ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }
                }
                else
                {
                    common_PlayVoiceAsrIndex(MAIN_VOICE_MAX_VOICE, -1);
                }
            }
            break;
        case MAIN_ASR_SMALLER_VOL:
            {
                if (RETURN_OK != main_device_baby_ctl(pAsrInfo->semantic_id, &PlayIndex))
                {
                    goto out;
                }

                if (0 == PlayIndex)
                {
                    ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                    if (RETURN_OK != ret)
                    {
                        goto out;
                    }
                }
                else
                {
                    common_PlayVoiceAsrIndex(MAIN_VOICE_MIN_VOICE, -1);
                }
            }
            break;
        case MAIN_ASR_MAX_VOL:
        case MAIN_ASR_MIN_VOL:
            {
                if (RETURN_OK != main_device_baby_ctl(pAsrInfo->semantic_id, &PlayIndex))
                {
                    goto out;
                }

                ret = AsrCmdPlayIdByMsg(pAsrInfo, PlayIndex);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;
        case MAIN_ASR_LIGHT_UP:
        case MAIN_ASR_LIGHT_DOWN:
        case MAIN_ASR_LOWST_LIGHT:
        case MAIN_ASR_HIGHT_LIGHT:
            {
                if (0 == temp_status.bLightInit)
                {
                    goto out;
                }

                if (RETURN_OK != main_device_baby_ctl(pAsrInfo->semantic_id, &PlayIndex))
                {
                    goto out;
                }

                ret = AsrCmdPlayIdByMsg(pAsrInfo, PlayIndex);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;
        case MAIN_ASR_VOICE_GUIDE:
            {
                ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;

        case MAIN_ASR_BEEP_MODE:
            {
                ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                if (RETURN_OK != ret)
                {
                    goto out;
                }

               prompt_player_enable(DISABLE);
            }
            break;

        case MAIN_ASR_VOICE_MODE:
            {
                prompt_player_enable(ENABLE);
                ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;

        case MAIN_ASR_REASE_ALL_CODE:
            {
                stDevice *device_Now = NULL;
                device_Now = getFirstDevice();
                while(device_Now)
                {
                    if (device_Now->CleanIrData)
                    {
                        device_Now->CleanIrData();
                    }

                    device_Now = device_Now->NextDevice;
                }

                ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;
        case MAIN_ASR_CANCEL_CLEAN_CODE:
            {
                ret = AsrCmdPlayIdByMsg(pAsrInfo, -1);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;
        default:
            break;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;

}

/*此数据是用于公共的，需要实现里面的接口*/
static stDevice main_Device = //唯一的单体案例，同时也是小宝设备，管理其他设备
{
    .Init = main_device_Init,
    .GetAsrId = main_device_getASR,
    .PlayId = main_device_playid,//播放相应处理
};

stDevice* get_main_device(void)
{
    return &main_Device;
}

int main_CheckGetLearnStatus(stKey* key, ir_data_t *ir_data)
{
    /*NULL 表示接收的是失败的匹配，需要播报匹配失败*/
    static int ifailCnt = 0;

    if (NULL == ir_data)
    {
        if (ifailCnt >= 2)
        {
            //清除所有设备学习模式
            if (RETURN_OK != DeviceClearnAllLearnMode())
            {
                ifailCnt = 0;
                goto out;
            }

            #if 0
            prompt_play_info2_t p_play_info[] = {
                {AIR_VOICE_MATCH_FAIL, -1},
                {MAIN_ASR_EXIT_MATCH, -1},
            };

            pause_voice_in();
            prompt_play_by_multi_semantic_id((prompt_play_info2_t *)&p_play_info, sizeof(p_play_info)/sizeof(prompt_play_info2_t),  learn_done_play_callback);
            #else
            pause_voice_in();
            prompt_play_by_semantic_id(AIR_VOICE_MATCH_FAIL, -1, default_play_done_callback, true);
            pause_voice_in();
            prompt_play_by_semantic_id(MAIN_ASR_EXIT_MATCH, -1, learn_done_play_callback, false);
            #endif

            ifailCnt = 0;
            goto out;
        }

        ifailCnt++;
        //匹配失败，重新播放，重新匹配

        if((NULL != key) &&(key->playVoice))
        {
            #if 0
            prompt_play_info2_t p_play_info[] = {
                {AIR_VOICE_MATCH_FAIL, -1},
                {key->iplay_befor_normal, -1},
            };

            pause_voice_in();
            prompt_play_by_multi_semantic_id((prompt_play_info2_t *)&p_play_info, sizeof(p_play_info)/sizeof(prompt_play_info2_t),  default_play_done_callback);
            #else
            pause_voice_in();
            prompt_play_by_semantic_id(AIR_VOICE_MATCH_FAIL, -1, default_play_done_callback, true);
            pause_voice_in();
            prompt_play_by_semantic_id(key->iplay_befor_normal, -1, default_play_done_callback, false);
            #endif

        }

        ir_data_learn_ctl(IR_START_KEY_LEARN, NULL);

        goto out;
    }
    else
    {
        ifailCnt = 0;
    }

    return RETURN_OK;

out:
    return RETURN_ERR;
}
