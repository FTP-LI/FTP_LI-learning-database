#include "device.h"
#include "middle_device.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "system_msg_deal.h"
#include "ci_log.h"

//
void userapp_deal_asr_msg_ex(sys_msg_asr_data_t *asr_msg)
{
    int ret = RETURN_ERR;
    int device_id = 0;
    uint32_t semantic_id = 0;
    cmd_handle_t cmd_handle = -1;
    stAsrInfo device_msg = {0};

    if(MSG_ASR_STATUS_GOOD_RESULT == asr_msg->asr_status)
    {
        cmd_handle = asr_msg->asr_cmd_handle;

        ret = DeviceFindAsr(cmd_handle, &device_id);
        if (RETURN_OK != ret)
        {
            goto out;
        }

        semantic_id = cmd_info_get_semantic_id(cmd_handle);

        device_msg.cmd_handle = cmd_handle;
        device_msg.semantic_id = semantic_id;
        device_msg.bPlay = 1;
        device_msg.device_id = device_id;

        DeviceDealASRMsg(&device_msg);

    }

out:
    return;
}


void userapp_deal_com_msg_semantic_id(int semantic_id)
{
    int ret = RETURN_ERR;
    int device_id = 0;
    cmd_handle_t cmd_handle = -1;
    stAsrInfo device_msg = {0};

    cmd_handle = cmd_info_find_command_by_semantic_id(semantic_id);

    ret = DeviceFindAsr(cmd_handle, &device_id);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    device_msg.cmd_handle = cmd_handle;
    device_msg.semantic_id = semantic_id;
    device_msg.bPlay = 1;
    device_msg.device_id = device_id;

    DeviceDealASRMsg(&device_msg);

out:
    return;
}


static void userapp_deal_com_msg_cmdid(int cmd_id)
{
    int ret = RETURN_ERR;
    int device_id = 0;
    cmd_handle_t cmd_handle = -1;
    stAsrInfo device_msg = {0};

    cmd_handle = cmd_info_find_command_by_id(cmd_id);

    ret = DeviceFindAsr(cmd_handle, &device_id);
    if (RETURN_OK != ret)
    {
        goto out;
    }

    device_msg.cmd_handle = cmd_handle;
    device_msg.semantic_id = cmd_info_get_semantic_id(cmd_handle);
    device_msg.bPlay = 1;
    device_msg.device_id = device_id;

    DeviceDealASRMsg(&device_msg);

out:
    return;
}

//处理串口消息
void userapp_deal_com_msg_ex(sys_msg_com_data_t *com_msg)
{
#if USE_IR_ENABEL
    uint32_t u32id = 0;
    uint8_t u8id_type = 0;
    u32id = *((uint32_t *)(&com_msg->msg_data[2]));
    u8id_type = *((uint8_t *)(&com_msg->msg_data[1]));

    if (VMUP_MSG_DATA_PLAY_BY_SEMANTIC_ID == u8id_type)
    {
        userapp_deal_com_msg_semantic_id(u32id);
    }
    else if (VMUP_MSG_DATA_PLAY_BY_CMD_ID == u8id_type)
    {
        userapp_deal_com_msg_cmdid(u32id);
    }
#endif
}


