#include "device.h"
#include "common_api.h"

static int device_task_asr(device_msg* rev_msg)
{
    int ret = RETURN_ERR;
    device_asr_msg* AsrMsg = &(rev_msg->msg_data.userasr_data);
    stDevice* pDevice = (stDevice*)AsrMsg->pDevice;
    stKey* pKey = (stKey*)AsrMsg->pKey;

    if ((NULL == rev_msg) || (NULL == AsrMsg->pDevice) || (NULL == AsrMsg->pKey))
    {
        goto out;
    }

    /*大致思想是这样的，还要加上一些其他的处理，如学习模的添加*/
    if ((E_DEVICE_MODE_LEARN == pDevice->ir_mode) && (pDevice->LearnIR))
    {
        ret = pDevice->LearnIR(pDevice, pKey, rev_msg->type, (void*)(&(AsrMsg->asr_id)));
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

static int device_task_learn_key(device_msg* rev_msg)
{
    int ret = RETURN_ERR;
    device_interrput_msg* TimerMsg = &(rev_msg->msg_data.timer_data);
    stDevice* pDevice = NULL;

    if (NULL == rev_msg)
    {
        goto out;
    }

    /*大致思想是这样的，还要加上一些其他的处理，如学习模的添加*/
    pDevice = DeviceGetLearnModeDevice();
    if (NULL == pDevice)
    {
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN == pDevice->ir_mode) && (pDevice->LearnIR))
    {
        ret = pDevice->LearnIR(pDevice, NULL, rev_msg->type, (void*)TimerMsg->pir_data);
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

static int device_task_softtime(device_msg* rev_msg)
{
    //做定时功能
    check_and_send_continue_key();
    air_CheckTimer();
    return RETURN_OK;
}


static int device_task_send_key(device_msg* rev_msg)
{
    int ret = RETURN_ERR;
    device_asr_msg* AsrMsg = &(rev_msg->msg_data.userasr_data);
    stDevice* pDevice = (stDevice*)AsrMsg->pDevice;
    stKey* pKey = (stKey*)AsrMsg->pKey;

    if ((NULL == rev_msg) || (NULL == AsrMsg->pDevice) || (NULL == AsrMsg->pKey))
    {
        goto out;
    }

    /*大致思想是这样的，还要加上一些其他的处理，如学习模的添加*/
    if ((E_DEVICE_MODE_LEARN != pDevice->ir_mode) && (pDevice->ContinueSendIR))
    {
        ret = pDevice->ContinueSendIR(pDevice, pKey, rev_msg->type, (void*)(&(AsrMsg->asr_id)));
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

static int device_task_air_key(device_msg* rev_msg)
{
    int ret = RETURN_ERR;
    stDevice* pDevice = NULL;

    if (NULL == rev_msg)
    {
        goto out;
    }

    /*检查是否有学习模式的设备*/
    pDevice = DeviceGetLearnModeDevice();
    if (NULL == pDevice)
    {
        goto out;
    }

    if ((E_DEVICE_MODE_LEARN == pDevice->ir_mode) && (pDevice->LearnIR))
    {
        ret = pDevice->LearnIR(pDevice, NULL, rev_msg->type, (void *)&(rev_msg->msg_data.ir_data.ir_code));
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

static int device_task_device_uart(device_msg* rev_msg)
{
    int ret = RETURN_ERR;
    stDevice* pDevice = NULL;

    if (NULL == rev_msg)
    {
        goto out;
    }

    pDevice = getFirstDevice();
    if (NULL == pDevice)
    {
        goto out;
    }

    while(pDevice)
    {
        if (pDevice->UartCallBack)
        {
            pDevice->UartCallBack(pDevice, (unsigned char*)&(rev_msg->msg_data.uart_data));
        }

        pDevice = pDevice->NextDevice;
    }


    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;

}

static int device_task_device_uart_learn_data(device_msg* rev_msg)
{
    int ret = RETURN_ERR;
    stDevice* pDevice = NULL;
    stUartLearnData UartLearnData = {0};

    if (NULL == rev_msg)
    {
        goto out;
    }

    UartLearnData = rev_msg->msg_data.uart_learn_data;

    mprintf("key_data addr:0x%x , file_data:%d\n", (unsigned int)(UartLearnData.key_data), UartLearnData.file_data);

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;

}

void DeviceTaskProcess(void* pvParameters)
{
    int ret = RETURN_ERR;
    device_msg rev_msg;
    BaseType_t err = pdFAIL;

    ret = DeviceInit();
    if (RETURN_OK != ret)
    {
        goto out;
    }

    ir_hw_init();

    while(1)
    {
        memset(&rev_msg, 0x00, sizeof(device_msg));
        err = xQueueReceive(device_queue, &rev_msg, portMAX_DELAY);
        if(pdPASS != err)
        {
            continue;
        }

        switch (rev_msg.type)
        {
            case TYPE_DEVICEMSG_LEARN_IR:
            {
                device_task_asr(&rev_msg);
            }
            break;
            case TYPE_DEVICEMSG_LEARN_KEY:
            {
                device_task_learn_key(&rev_msg);
            }
            break;
            case TYPE_DEVICEMSG_SOFTTIM:
            {
                device_task_softtime(&rev_msg);
            }
            break;
            case TYPE_DEVICEMSG_SEND_KEY:
            {
                device_task_send_key(&rev_msg);
            }
            break;
            case TYPE_DEVICEMSG_AIR_MATCH:
            {
                device_task_air_key(&rev_msg);
            }
            break;
            case TYPE_DEVICEMSG_UART:
            {
                device_task_device_uart(&rev_msg);
            }
            break;
            case TYPE_DEVICEMSG_AIRC_SEND_CALLBACK:
            {
                mprintf("%s airc send callback cmd = %d\n",__func__,rev_msg.msg_data.ir_callback_data.callback_msg);
                air_device_send_callback(rev_msg.type, rev_msg.msg_data.ir_callback_data.callback_msg);
            }
            break;
            case TYPE_DEVICEMSG_LEARN_SEND_CALLBACK:
            {
                mprintf("%s learn data send callback key id = %d\n",__func__,rev_msg.msg_data.ir_callback_data.callback_msg);
            }
            break;
            case TYPE_DEVICEMSG_AIRC_CHECK_CALLBACK:
            {
                mprintf("%s airc cmd check vaild = %d\n",__func__,rev_msg.msg_data.ir_callback_data.callback_msg);
                air_device_send_callback(rev_msg.type, rev_msg.msg_data.ir_callback_data.callback_msg);
            }
            break;
            case TYPE_DEVICEMSG_LEARN_KEY_UART:
            {
                device_task_device_uart_learn_data(&rev_msg);
            }
            break;
            default:
                break;
        }
    }

out:
    vTaskDelete(NULL);/*delete self*/
}
