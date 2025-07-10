#include "doa_app_handle.h"

#if USE_AI_DOA
int g_current_doa_angle = 0;
int g_doa_out_result_flag = 0;   //doa输出角度信息标志
#if CLOUD_UART_PROTOCOL_EN
#include "chipintelli_cloud_protocol.h"
extern DOACloudCmdProtocolTypedef gDOACloudCmdProtocol[DOA_STATE_MAX];
void ci_doa_get_cb(int audio_state, int doa_angle)
{

        g_current_doa_angle = m_doa_angle;
		mprintf("--------doa angle: %d°\n", doa_angle);
        for(int i = 0; i < DOA_STATE_MAX; i++)
        {
            if(gDOACloudCmdProtocol[i].doa_angle_id == g_current_doa_angle/10)
            {
                #if CLOUD_CFG_PLAY_EN
                if(gDOACloudCmdProtocol[i].cloudCmdProtocol.play_type == 0)  //主动播报
                    prompt_play_by_cmd_id(gDOACloudCmdProtocol[i].cloudCmdProtocol.cmd_id, -1, default_play_done_callback, true);
                #endif
                #if CLOUD_CFG_UART_SEND_EN
                    cloud_uart_send_buf(gDOACloudCmdProtocol[i].cloudCmdProtocol.protocol_buf, gDOACloudCmdProtocol[i].cloudCmdProtocol.protocol_len);
                #endif
            }
            
        }
	}	
}
#else
//该函数是doa算法回调函数，不可执行耗时操作
void ci_doa_get_cb(int audio_state, int doa_angle)
{
    g_doa_out_result_flag = 1;
    g_current_doa_angle = doa_angle;
	mprintf("--------doa angle: %d°\n", doa_angle);
}
#endif
int ci_get_doa_angle(void)
{
	return g_current_doa_angle;
}
//doa角度信息输出处理函数
void doa_out_result_hand_task(void *p)
{
    while(1)
    {
        if(g_doa_out_result_flag)
        {
            g_doa_out_result_flag = 0;
            //执行具体的操作-用户添加
        }
        vTaskDelay(50);
    }
}

#endif