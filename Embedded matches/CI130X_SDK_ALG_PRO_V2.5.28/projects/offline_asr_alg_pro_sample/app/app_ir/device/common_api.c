#include "common_api.h"
#include "asr_api.h"
#include "command_info.h"
// #include "ci112x_audio_capture.h"
#include "asr_api.h"
#include "system_msg_deal.h"
#include "user_msg_deal.h"


static int device_common_send_fromASR(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey, void* data)
{
    device_msg rev_msg;

    if ((NULL == pDevice) || (NULL == pKey))
    {
        goto out;
    }

    rev_msg.type = msgtyp;
    rev_msg.msg_data.userasr_data.pDevice = pDevice;
    rev_msg.msg_data.userasr_data.pKey = pKey;
    rev_msg.msg_data.userasr_data.asr_id = (*(int*)data);
    xQueueSend(device_queue,&rev_msg,0);

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}


static int device_common_send_fromISR(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey, void* data)
{
    device_msg rev_msg;

    rev_msg.type = msgtyp;
    rev_msg.msg_data.timer_data.pDevice = pDevice;
    rev_msg.msg_data.timer_data.pKey = pKey;
    rev_msg.msg_data.timer_data.pir_data = (ir_data_t*)data;

    xQueueSend(device_queue,&rev_msg,0);

    return RETURN_OK;
}

static int device_common_send_fromIr(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey, void* data)
{
    device_msg rev_msg;

    rev_msg.type = msgtyp;

    if (NULL == data)
    {
        rev_msg.msg_data.ir_data.ir_code = 0;
    }
    else
    {
        rev_msg.msg_data.ir_data.ir_code = (*(int*)data);
    }

    xQueueSend(device_queue,&rev_msg,0);

    return RETURN_OK;
}

static int device_common_send_fromUart(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey, void* data)
{
    device_msg rev_msg;

    rev_msg.type = msgtyp;

    memcpy(&(rev_msg.msg_data.uart_data), (unsigned char*)data, sizeof(device_uart_msg));

    xQueueSend(device_queue, &rev_msg, 0);

    return RETURN_OK;
}

static int device_common_send_learnfromUart(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey, void* data)
{
    device_msg rev_msg;

    rev_msg.type = msgtyp;

    memcpy(&(rev_msg.msg_data.uart_learn_data), (unsigned char*)data, sizeof(stUartLearnData));

    xQueueSend(device_queue, &rev_msg, 0);

    return RETURN_OK;
}

static int device_common_send_fromIrSendCallback(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey, void* data)
{
    device_msg rev_msg;

    rev_msg.type = msgtyp;

    rev_msg.msg_data.ir_callback_data.callback_msg = (*(int*)data);

    xQueueSend(device_queue, &rev_msg, 0);

    return RETURN_OK;
}


int device_common_sendmsg(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey ,void* data)
{
    int ret = RETURN_ERR;

    switch(msgtyp)
    {
        case TYPE_DEVICEMSG_LEARN_IR:
            {
                ret = device_common_send_fromASR(msgtyp, pDevice, pKey, data);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;
        case TYPE_DEVICEMSG_LEARN_KEY:
        case TYPE_DEVICEMSG_SOFTTIM:
            {
                ret = device_common_send_fromISR(msgtyp, pDevice, pKey, data);
                if (RETURN_OK != ret)
                {
                    goto out;
                }
            }
            break;
        case TYPE_DEVICEMSG_SEND_KEY:
            {

            }
            break;
        case TYPE_DEVICEMSG_AIR_MATCH:
            {
                ret = device_common_send_fromIr(msgtyp, pDevice, pKey, data);
                if (RETURN_OK != ret)
                {
                    goto out;
                }

            }
            break;
        case TYPE_DEVICEMSG_UART:
            {
                ret = device_common_send_fromUart(msgtyp, pDevice, pKey, data);
                if (RETURN_OK != ret)
                {
                    goto out;
                }

            }
            break;
        case TYPE_DEVICEMSG_LEARN_KEY_UART:
            {
                ret = device_common_send_learnfromUart(msgtyp, pDevice, pKey, data);
                if (RETURN_OK != ret)
                {
                    goto out;
                }

            }
            break;
        case TYPE_DEVICEMSG_AIRC_SEND_CALLBACK:
        case TYPE_DEVICEMSG_LEARN_SEND_CALLBACK:
            {
                ret = device_common_send_fromIrSendCallback(msgtyp, pDevice, pKey, data);
                if (RETURN_OK != ret)
                {
                    goto out;
                }

            }
            break;
        case TYPE_DEVICEMSG_AIRC_CHECK_CALLBACK:
            {
                ret = device_common_send_fromIrSendCallback(msgtyp, pDevice, pKey, data);
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

stKey* GetCmdKey(uint32_t CmdIndex, AsrCmd_Key_TypeDef* CmdKey, int len)
{
    AsrCmd_Key_TypeDef* pCmd = NULL;
    int index = 0;

    if (NULL == CmdKey)
    {
        goto out;
    }

    for (index = 0; index < len;index++)
    {
        pCmd = &CmdKey[index];

        if(CmdIndex == pCmd->index)
        {
            return pCmd->pKey;
        }
    }

out:
    return NULL;
}

int AsrCmdPlayIdByCmdHandle(unsigned int cmd_handle, int play_id)
{
    #if 0
    uint8_t option_number = get_voice_option_number(cmd_handle);

    if (option_number < play_id)
    {
        goto out;
    }
    #endif

    user_led_light_flash();

    prompt_play_by_cmd_handle(cmd_handle, play_id, default_play_done_callback, pdTRUE);

    return RETURN_OK;

out:
    return RETURN_ERR;
}

//通过命令词MSG播报,里面含有是否需要播报的设置
int AsrCmdPlayIdByMsg(stAsrInfo* pAsrInfo, int play_id)
{
    if (NULL == pAsrInfo)
    {
        goto out;
    }

    if (0 == pAsrInfo->bPlay)
    {
        return RETURN_OK;
    }

    pause_voice_in();

    return AsrCmdPlayIdByCmdHandle(pAsrInfo->cmd_handle , play_id);

out:
    return RETURN_ERR;
}


//通过命令词ID播报,里面不含有是否需要播报的设置
int AsrCmdPlayIdByCmdID(uint32_t semantic_id, int play_id)
{
    cmd_handle_t cmd_handle = cmd_info_find_command_by_semantic_id(semantic_id);
    if (INVALID_HANDLE == cmd_handle)
    {
        goto out;
    }

    pause_voice_in();

    return AsrCmdPlayIdByCmdHandle(cmd_handle , play_id);

out:
    return RETURN_ERR;
}

//通过命令词ID播报,里面含有是否需要播报的设置
int AsrCmdPlayIdByCmdIDEx(stAsrInfo* pAsrInfo, uint32_t semantic_id, int play_id)
{
    cmd_handle_t cmd_handle = INVALID_HANDLE;
    if (NULL == pAsrInfo)
    {
        goto out;
    }

    if (0 == pAsrInfo->bPlay)
    {
        return RETURN_OK;
    }

    cmd_handle = cmd_info_find_command_by_semantic_id(semantic_id);
    if (INVALID_HANDLE == cmd_handle)
    {
        goto out;
    }

    pause_voice_in();

    return AsrCmdPlayIdByCmdHandle(cmd_handle , play_id);

out:
    return RETURN_ERR;
}

int CheckKeyJumpNext(stKeyJump* keyjump, int len, stKey* key, int *plen)
{
    int i = 0;
    stKey* TempKey = NULL;
    stKeyJump* TempKeyjump = NULL;

    if ((NULL == keyjump) || (NULL == key) || (NULL == plen) || (0 == len))
    {
        goto out;
    }

    for(i = 0; i < len; i++)
    {
        TempKeyjump = keyjump + i;
        if (key->key_id == TempKeyjump->key->key_id)
        {
            TempKey = TempKeyjump->key;
            *plen = TempKeyjump->jumpCnt;

            //清除下面的组按键的bHaveKey标志
            for (int j = 0; j < TempKeyjump->jumpCnt; j++)
            {
                TempKey++;
            }
            break;
        }
    }

    if (i >= len)
    {
        goto out;
    }

    return RETURN_OK;

out:
    return RETURN_ERR;
}

int common_PlayVoiceAsrIndex(uint32_t semantic_id, int play_index)
{
    pause_voice_in();
    prompt_play_by_semantic_id(semantic_id, play_index, default_play_done_callback , pdTRUE);

    return RETURN_OK;
}

static stContinueSendKey* pContinueSendKeyHeader = NULL;
static SemaphoreHandle_t tvDeviceMutex = NULL;

int ContinueSendKey_list_init(void)
{
    //初始化tvDeviceMutex
    tvDeviceMutex = xSemaphoreCreateMutex();
    if (NULL == tvDeviceMutex)
    {
        goto out;
    }
    return RETURN_OK;
out:
    return RETURN_ERR;
}

int check_key_in_ContinueSendKey_list(stKey* pKey)
{
    stContinueSendKey* pTempCDKey = pContinueSendKeyHeader;

    xSemaphoreTake(tvDeviceMutex, portMAX_DELAY);

    while(NULL != pTempCDKey)
    {
        if (pTempCDKey->pKey == pKey)
        {
            xSemaphoreGive(tvDeviceMutex);
            return RETURN_OK;
        }
        pTempCDKey = pTempCDKey->Next;
    }

    xSemaphoreGive(tvDeviceMutex);

    return RETURN_ERR;
}

void add_to_ContinueSendKey_list(stContinueSendKey* pContinueSendKey)
{
    stContinueSendKey* pTempCDKey = NULL;

    if (NULL == pContinueSendKey)
    {
        goto out;
    }

    pTempCDKey = pvPortMalloc(sizeof(stContinueSendKey));

    if (NULL == pTempCDKey)
    {
        goto out;
    }

    memcpy(pTempCDKey, pContinueSendKey, sizeof(stContinueSendKey));

    xSemaphoreTake(tvDeviceMutex, portMAX_DELAY);

    pTempCDKey->Next = pContinueSendKeyHeader;

    pContinueSendKeyHeader = pTempCDKey;

    xSemaphoreGive(tvDeviceMutex);

out:
    return ;
}

stContinueSendKey* delete_form_ContinueSendKey_list(stContinueSendKey* pContinueSendKey)
{
    stContinueSendKey* pTempCDKey = pContinueSendKeyHeader;
    stContinueSendKey* pPrevTempCDKey = NULL;
    stContinueSendKey* pNextTempCDKey = NULL;

    if (NULL == pContinueSendKey)
    {
        goto out;
    }

    xSemaphoreTake(tvDeviceMutex, portMAX_DELAY);

    if (pTempCDKey == pContinueSendKey)
    {
        if (pTempCDKey->ContinueKeySendDown)
        {
            pTempCDKey->ContinueKeySendDown(pTempCDKey->pKey);
        }

        pContinueSendKeyHeader = pTempCDKey->Next;
        vPortFree(pTempCDKey);
        pTempCDKey = NULL;
        pNextTempCDKey = pContinueSendKeyHeader;
    }
    else
    {
        //赋初始值
        pPrevTempCDKey = pTempCDKey;
        pTempCDKey = pTempCDKey->Next;

        while(NULL != pTempCDKey)
        {
            if (pTempCDKey == pContinueSendKey)
            {
                if (pTempCDKey->ContinueKeySendDown)
                {
                    pTempCDKey->ContinueKeySendDown(pTempCDKey->pKey);
                }

                pPrevTempCDKey->Next = pTempCDKey->Next;
                pNextTempCDKey = pTempCDKey->Next;
                vPortFree(pTempCDKey);
                pTempCDKey = NULL;
                break;
            }
            else
            {
                pPrevTempCDKey = pTempCDKey;
                pTempCDKey = pTempCDKey->Next;
            }
        }
    }

    xSemaphoreGive(tvDeviceMutex);

out:
    return pNextTempCDKey;
}

int delete_all_ContinueSendKey_list(void)
{
    stContinueSendKey* pTempCDKey = pContinueSendKeyHeader;

    while(NULL != pTempCDKey)
    {
        delete_form_ContinueSendKey_list(pTempCDKey);
        pTempCDKey = pTempCDKey->Next;
    }

    return RETURN_OK;
}


int stop_key_in_ContinueSendKey_list(stKey* pKey)
{
    stContinueSendKey* pTempCDKey = pContinueSendKeyHeader;

    while(NULL != pTempCDKey)
    {
        if (pTempCDKey->pKey == pKey)
        {
            pTempCDKey->send_cnt.bFlag = 0;
            pTempCDKey->send_timer.bFlag = 0;
            return RETURN_OK;
        }
        pTempCDKey = pTempCDKey->Next;
    }

    return RETURN_ERR;
}


int check_send_continue_key(stContinueSendKey *pContinueSendKey)
{
    int ret = RETURN_EXE_NULL;
    signed char bSendFlag = 0;

    if ((1 == pContinueSendKey->send_cnt.bFlag) || (1 == pContinueSendKey->send_timer.bFlag))
    {
        pContinueSendKey->offset_TimeMS += DEVICE_TIME;
        if (pContinueSendKey->offset_TimeMS >= pContinueSendKey->send_key_TimeMS)
        {
            pContinueSendKey->pKey->sendIR(pContinueSendKey->pKey);
            bSendFlag = 1;
            ret = RETURN_EXE_OK;
        }

        if (1 == bSendFlag)
        {
            if (1 == pContinueSendKey->send_cnt.bFlag)
            {
                pContinueSendKey->send_cnt.cnt++;

                if (pContinueSendKey->send_cnt.cnt >= pContinueSendKey->send_cnt.totle_cnt)
                {
                    pContinueSendKey->send_cnt.bFlag = 0;
                    pContinueSendKey->send_cnt.cnt = 0;
                    pContinueSendKey->send_cnt.totle_cnt = 0;
                    pContinueSendKey->send_key_TimeMS = 0;
                    ret = RETURN_EXE_OVER;
                }
            }

            if (1 == pContinueSendKey->send_timer.bFlag)
            {
                pContinueSendKey->send_timer.timer += pContinueSendKey->offset_TimeMS;

                if (pContinueSendKey->send_timer.timer >= pContinueSendKey->send_timer.totle_timer)
                {
                    pContinueSendKey->send_timer.bFlag = 0;
                    pContinueSendKey->send_timer.timer = 0;
                    pContinueSendKey->send_timer.totle_timer = 0;
                    pContinueSendKey->send_key_TimeMS = 0;
                    ret = RETURN_EXE_OVER;
                }
            }

            pContinueSendKey->offset_TimeMS = 0;
        }
    }
    else
    {
        ret = RETURN_EXE_OVER;
    }
    return ret;
}

int check_and_send_continue_key(void)
{
    int ret = RETURN_EXE_NULL;
    stContinueSendKey* pNextTempCDKey = NULL;
    stContinueSendKey* pTempCDKey = pContinueSendKeyHeader;

    while(NULL != pTempCDKey)
    {
        ret = check_send_continue_key(pTempCDKey);
        if (RETURN_EXE_OVER == ret)
        {
            pNextTempCDKey = delete_form_ContinueSendKey_list(pTempCDKey);
            pTempCDKey = pNextTempCDKey;
        }
        else
        {
            pTempCDKey = pTempCDKey->Next;
        }

    }
    return RETURN_OK;
}

void learn_done_play_callback(cmd_handle_t cmd_handle)
{
#if USE_SEPARATE_WAKEUP_EN
    cmd_info_change_cur_model_group(0);
#endif
    default_play_done_callback(cmd_handle);
}
