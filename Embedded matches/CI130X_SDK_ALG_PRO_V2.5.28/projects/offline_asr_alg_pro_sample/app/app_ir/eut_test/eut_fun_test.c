#include "eut_fun_test.h"
#include "system_msg_deal.h"
#include "ir_remote_driver.h"
#include "FreeRTOS.h"
#include "audio_play_api.h"
#include "timers.h"
#include "prompt_player.h"
#include "led_light_control.h"
#include "ci130x_it.h"
#include "ci130x_uart.h"
#if(IR_TEST==1)

uint8_t g_test_state=BOARD_STATE_IDLE;

#define IR_RECV_VALU    0xA5

#define IR_RECV_NEC_LEAD  (9000+2250)
#define IR_RECV_NEC_DIFF_LEAD  (2000)


#define IR_RECV_NEC_H  (560+1680)
#define IR_RECV_NEC_L  (560+560)

#define IR_RECV_NEC_H_DIFF  (672)
#define IR_RECV_NEC_L_DIFF  (336)

static uint16_t ir_level_code[] ={1680,1680};

#define IR_LEVEL_SIZE  (sizeof(ir_level_code)/sizeof(ir_level_code[0]))

static uint16_t *ir_level_recv_code =NULL;//


#define TEST_SEND_REPT_TIME   100//500ms周期发送一次
#define TEST_PLAY_ID  (2000)

static TimerHandle_t  timer_test_send_repeat= NULL;


static void timer_test_send_repeat_callback(TimerHandle_t xTimer)
{	
	if(g_test_state==BOARD_STATE_IR_SEND_START)
	{
		send_ir_code_start(IR_LEVEL_SIZE);
		
	}
}

void userapp_test_state_recv_cmd(uint8_t cmd)
{
	if(cmd>=BOARD_STATE_IDLE && cmd<=BOARD_STATE_TEST_END_HANDSHK)
	{
		xTimerStop(timer_test_send_repeat,0);
		switch(cmd)
		{			
			case BOARD_STATE_IDLE:
			{
				eut_test_init();//收到测试指令初始化
				vTaskDelay(pdMS_TO_TICKS(10)); 
				test_board_send_protocol(EUT_STATE_IDLE);					
				break;				
			}			
			case BOARD_STATE_IR_RECV_START://开启红外接收头
			{	
				
				ir_level_recv_code=(uint16_t *)pvPortMalloc(sizeof(uint16_t)*1024);
				vTaskDelay(pdMS_TO_TICKS(10)); 
				set_ir_level_code_addr_for_test((uint32_t)ir_level_recv_code,1024);
				ir_receive_start(5000);		
				g_test_state=BOARD_STATE_IR_RECV_START;
				break;				
			}			
			case BOARD_STATE_IR_RECV_FAIL_HANDSHK://
			{
				if(g_test_state==BOARD_STATE_IR_RECV_START)
				{
					vPortFree(ir_level_recv_code);
					test_board_send_protocol(EUT_STATE_IR_RECV_FAIL_HANDSHK);
				}				
				break;				
			}			
			case BOARD_STATE_IR_SEND_START://
			{
				vPortFree(ir_level_recv_code);
				g_test_state=BOARD_STATE_IR_SEND_START;
				set_ir_level_code_addr_for_test((uint32_t)ir_level_code,IR_LEVEL_SIZE);
				stop_play(NULL,NULL);
				test_board_send_protocol(EUT_STATE_IR_SEND_START);//				
				xTimerStart(timer_test_send_repeat,0);

				break;			
			}				
			case BOARD_STATE_IR_SEND_FAIL_HANDSHK://
			{
				if(g_test_state==BOARD_STATE_IR_SEND_START)
				{
					test_board_send_protocol(EUT_STATE_IR_SEND_FAIL_HANDSHK);
				}				
				break;			
			}	
			case BOARD_STATE_MIC_START://
			{				
				g_test_state=BOARD_STATE_MIC_START;
				vTaskDelay(pdMS_TO_TICKS(10)); 
				test_board_send_protocol(EUT_STATE_MIC_START);//							
				break;			
			}				
			case BOARD_STATE_MIC_FAIL_HANDSHK://
			{
				if(g_test_state==BOARD_STATE_MIC_START)
				{
					test_board_send_protocol(EUT_STATE_MIC_FAIL_HANDSHK);		
				}						
				break;				
			}	
			case BOARD_STATE_SPK_START://
			{
				g_test_state=BOARD_STATE_SPK_START;
							
				test_board_send_protocol(RECV_STATE_SPK_START);		
 
				vTaskDelay(pdMS_TO_TICKS(100)); 
				
				pause_voice_in();
		
				prompt_play_by_cmd_id(TEST_PLAY_ID, -1, default_play_done_callback,true);
				
				break;				
			}					
			case BOARD_STATE_SPK_FAIL_HANDSHK://
			{
					night_light_init();
				night_light_set_brightness(1,50);	
				if(g_test_state==BOARD_STATE_SPK_START)
				{
					test_board_send_protocol(RECV_STATE_SPK_FAIL_HANDSHK);	
				}			
				break;				
			}	

			case BOARD_STATE_TEST_END://
			{
				g_test_state=BOARD_STATE_TEST_END;
				night_light_init();
				night_light_set_brightness(1,50);		
				test_board_send_protocol(RECV_STATE_TEST_END_HANDSHK);		
				break;				
			}	
			case BOARD_STATE_TEST_END_HANDSHK://
			{
                night_light_init();
				night_light_set_brightness(1,50);			
				if(g_test_state==BOARD_STATE_TEST_END)
				{
					test_board_send_protocol(RECV_STATE_TEST_END_HANDSHK);
					//关闭串口,存储状态
					set_flash_eut_state(1);
					UART_EN(HAL_UART0_BASE, DISABLE);
				}					
				break;		
				
			}	
			
		}	
		
	}
}

static int test_ir_recv_chk(void)
{
	if(g_test_state==BOARD_STATE_IR_RECV_START)
	{
		uint32_t ir_level_count=0;
		uint16_t *ir_level_code=NULL;
		uint16_t ir_level_add=0;//
		uint8_t  ir_recv_valu=0;//
		uint8_t  ir_recv_valid=0;//
		uint8_t  ir_recv_valid_cnt=0;
		
		ir_level_count=get_receive_level_count();
		
		if(get_ir_driver_buf()!=NULL)
		{
			if(ir_level_count<20)
			{
				ir_receive_start(5000);
				return 0;
			}
			
			ir_level_code=get_ir_driver_buf();
			
			for(uint8_t i=0;i<ir_level_count;i++)
			{			
				ir_level_add=(*(ir_level_code+i)+*(ir_level_code+i+1))*IR_DATA_DIV_COEFFICIENT;

				if((ir_level_add<=(IR_RECV_NEC_LEAD+IR_RECV_NEC_DIFF_LEAD)) && (ir_level_add>=(IR_RECV_NEC_LEAD-IR_RECV_NEC_DIFF_LEAD)))
				{
					ir_recv_valu=0;
					ir_recv_valid_cnt=0;
					ir_recv_valid=1;
				}
				else
				{
					if(ir_recv_valid_cnt==16 && ir_recv_valid==1)
					{
						ir_recv_valu=0;
						ir_recv_valid_cnt=0;
						ir_recv_valid=0;
					}
					else if(ir_recv_valid==1)
					{						
						if((ir_level_add<=(IR_RECV_NEC_H+IR_RECV_NEC_H_DIFF)) && (ir_level_add>=(IR_RECV_NEC_H-IR_RECV_NEC_H_DIFF)))
						{
							ir_recv_valu|=0x01; 
						}
						else if((ir_level_add<=(IR_RECV_NEC_L+IR_RECV_NEC_L_DIFF)) && (ir_level_add>=(IR_RECV_NEC_L-IR_RECV_NEC_L_DIFF)))
						{
							ir_recv_valu&=0xFE;
						}	
						if(ir_recv_valu==IR_RECV_VALU)
						{
									
							test_board_send_protocol(EUT_STATE_IR_RECV_OK);
							
							g_test_state=BOARD_STATE_IR_SEND_START;
							
							return 1;
						}	
						ir_recv_valid_cnt++;
						ir_recv_valu<<=1;				
					}
				}	
				
				i++;
			}
			
		}	
		
		ir_receive_start(5000);
	}
	
	return 0;

}

void vtask_test_board(void *p_arg)
{
	while(1)
	{	
		test_ir_recv_chk();
		
		vTaskDelay(pdMS_TO_TICKS(100)); 			
	}
	
}

int eut_test_init(void)
{

	BaseType_t ret;
	//ir_hw_init();
	
	timer_test_send_repeat = xTimerCreate("timer_test_send_repeat",pdMS_TO_TICKS(TEST_SEND_REPT_TIME),pdTRUE,(void *)0,timer_test_send_repeat_callback);

	if(timer_test_send_repeat == NULL)
	{
		mprintf("timer_test_send_repeat creat err \n");
	}

	
    ret = xTaskCreate(vtask_test_board,       /* 任务函数  */
        "vtask_test_board",                    /* 任务名    */
        200,                                /* 任务栈大小，单位word，也就是4字节 */
        NULL,                               /* 任务参数  */
        4,                                  /* 任务优先级*/
        NULL );                             /* 任务句柄  */
    if(ret != pdPASS)
    {
        mprintf("vtask_test_board creat err\n");
        return RETURN_ERR;
    }

	return RETURN_OK;
}

#endif //(IR_TEST == 1)



