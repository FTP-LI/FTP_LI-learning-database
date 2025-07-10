#include "ir_test_protocol.h"
#include "ci_nvdata_manage.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "sdk_default_config.h"
#include "voice_module_uart_protocol.h"
#include "system_msg_deal.h"
#include "command_info.h"
#include "ci130x_spiflash.h"
#include "ci130x_dpmu.h"
#include "prompt_player.h"
#include "audio_play_api.h"
#include "asr_api.h"
#include "firmware_updater.h"
#include <string.h>
#include <stdlib.h>
#include "romlib_runtime.h"
#include "flash_manage_outside_port.h"
#include "baudrate_calibrate.h"

int eut_state = 0;

#if (IR_TEST == 1)
void userapp_test_cmd(sys_msg_com_data_t *msg)
{
	userapp_test_state_recv_cmd(msg->msg_cmd);
}
#endif

void set_flash_eut_state(int gt_wtn)
{
    eut_state = gt_wtn;
    cinv_item_write(NVDATA_ID_WAKE_NUMBER,sizeof(eut_state), &eut_state);
}
void get_flash_eut_state(void)
{
   uint16_t real_len = 0;
   if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_WAKE_NUMBER, sizeof(eut_state), &eut_state, &real_len))
    {
	     //first write
        if(CINV_ITEM_UNINIT == cinv_item_init(NVDATA_ID_WAKE_NUMBER, sizeof(eut_state), &eut_state))
        {
            mprintf("first write\n");
        }
    }
    mprintf("wake_number %d\n",eut_state);

}

