#include "FreeRTOS.h" 
#include "task.h"
#include "timers.h"
#include "ci130x_dpmu.h"
#include "system_msg_deal.h"
#include "user_config.h"
#include "sdk_default_config.h"
#include "rf_msg_deal.h"
#include "crc.h"
#include "exe_api.h"
#include "cias_tbm_msg_deal.h"


tbm_dev_t tbm_dev;
extern rf_cb_funcs_t rf_cb_funcs; //射频发送和接收的回调函数


void tbm_init(void)
{
    tbm_dev.power = FUN_TURN_OFF;
    tbm_dev.child = FUN_TURN_OFF;
    tbm_dev.left_water = 0;
    tbm_dev.left_heatperservation = 0;
    tbm_dev.left_refrigeration = 0;
    tbm_dev.right_autoBoilingWater = 0;
    tbm_dev.right_water = 0;
    tbm_dev.right_heat = 0;
    tbm_dev.right_currentTmp = 40;

}

uint8_t tbm_report(uint16_t cmd_id)
{
    bool send_flag = true;
    uint8_t send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t len;

    send_data[0] = 0xA5;
    send_data[1] = 0x5A;
    send_data[2] = 0x01;
    send_data[3] = 0x01;
    send_data[4] = TEABAR_DEV;
    send_data[5] = 0x01;
    send_data[6] = 0x21;
    send_data[8] = 0x00;
    send_data[9] = 0x01;
    switch (cmd_id)
    {
        case TURN_OFF:
        {
            send_data[7] = TBM_POWER;
            send_data[10] = FUN_TURN_OFF;//数据
            tbm_dev.power = FUN_TURN_OFF;
            break;
        }
        case TURN_ON:
        {
            send_data[7] = TBM_POWER;
            send_data[10] = FUN_TURN_ON;//数据
            tbm_dev.power = FUN_TURN_ON;
            break;
        }
        case CLOSE_CHILD_LOCK://童锁
        {
            send_data[7] = TBM_CHILD;
            send_data[10] = FUN_TURN_OFF;
            tbm_dev.child = FUN_TURN_OFF;
            break;
        }
        case OPEN_CHILD_LOCK://童锁
        {
            send_data[7] = TBM_CHILD;
            send_data[10] = FUN_TURN_ON;
            tbm_dev.child = FUN_TURN_ON;
            break;
        }
        case LEFT_STOP_FETCH_WATER ... OPEN_COOLING://左壶
        {
            send_data[7] = TBM_LIEFT;
            switch (cmd_id)
            {
                case LEFT_STOP_FETCH_WATER:
                send_data[10] = 0x01;
                tbm_dev.left_water = send_data[10];
                break;
                case STOP_INSULATION:
                send_data[10] = 0x02;                
                tbm_dev.left_heatperservation = send_data[10];
                break;
                case STOP_COOLING:
                send_data[10] = 0x03;
                tbm_dev.left_refrigeration = send_data[10];
                break;
                case LEFT_FETCH_WATER:
                send_data[10] = 0x11;
                tbm_dev.left_water = send_data[10];
                break;
                case OPEN_INSULATION:
                send_data[10] = 0x12;
                tbm_dev.left_heatperservation = send_data[10];
                break;
                case OPEN_COOLING:
                send_data[10] = 0x13;
                tbm_dev.left_refrigeration = send_data[10];
                break;
            
                default:
                break;
            }           
            
            break;
        }
        case BOILING_WATER_OFF ... KETTLE_HEATING://右壶
        {
            send_data[7] = TBM_RIGHT;
            switch (cmd_id)
            {
                case BOILING_WATER_OFF:
                send_data[10] = 0x01;
                tbm_dev.right_autoBoilingWater = send_data[10];
                break;
                case RIGHT_STOP_FETCH_WATER:
                send_data[10] = 0x02;
                tbm_dev.right_water = send_data[10];
                break;
                case STOP_FETCH_WATER:
                send_data[10] = 0x03;
                tbm_dev.right_heat = send_data[10];
                break;
                case AUTO_WATER_BOILING:
                send_data[10] = 0x11;
                tbm_dev.right_autoBoilingWater = send_data[10];
                break;
                case RIGHT_FETCH_WATER :
                send_data[10] = 0x12;
                tbm_dev.right_water = send_data[10];
                break;
                case KETTLE_HEATING:
                send_data[10] = 0x13;
                tbm_dev.right_heat = send_data[10];
                break;
            
                default:
                break;
            }
            break;
        }
        case TEMPERATURE_40 ... TEMPERATURE_100://温度
        {
            send_data[7] = TBM_TEMPERATURE;
            send_data[10] = (cmd_id-TEMPERATURE_40)*5+40;
            tbm_dev.right_currentTmp = send_data[10];
            break;
        }
        default:
        {
            send_flag = false;
            break;
        }
    }
    if (send_flag)
    {
        uint16_t crc_cal = crc16_ccitt(0, send_data, 11);
        mprintf("crc = 0x%x\r\n",crc_cal);
        //send_data[send_data[7]+8] = crc_cal >> 8;
        send_data[11] = crc_cal>>8;
        send_data[12] = crc_cal&0xFF;

        cias_crypto_data(send_data, 13); 
        rf_cb_funcs.rf_send(send_data, 13);
    }
    return 0;
}



void tbm_callback(ble_msg_V1_t msg)
{
    int8_t ret = -1;
    switch (msg.function_id)
    {
        case TBM_POWER:
        {
            mprintf("茶吧机开关:%x\r\n",msg.data[0]); 

            tbm_dev.power = msg.data[0];
            
            if (msg.data[0] == FUN_TURN_OFF)
            {
                prompt_play_by_cmd_id(TURN_OFF, -1, default_play_done_callback,true);
                uart_send_asr(TURN_OFF); 
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                prompt_play_by_cmd_id(TURN_ON, -1, default_play_done_callback,true);
                uart_send_asr(TURN_ON);  
            }

            break;
        }
        case TBM_CHILD:
        {
            mprintf("茶吧机童锁:%x\r\n",msg.data[0]); 
            tbm_dev.child = msg.data[0];
            
            if (msg.data[0] == FUN_TURN_OFF)
            {
                prompt_play_by_cmd_id(CLOSE_CHILD_LOCK, -1, default_play_done_callback,true);
                uart_send_asr(CLOSE_CHILD_LOCK); 
            }
            else if (msg.data[0] == FUN_TURN_ON)
            {
                prompt_play_by_cmd_id(OPEN_CHILD_LOCK, -1, default_play_done_callback,true);
                uart_send_asr(OPEN_CHILD_LOCK);  
            }
            break;
        }
        case TBM_LIEFT:
        {
            mprintf("茶吧机左壶:%x\r\n",msg.data[0]); 
            switch (msg.data[0])
            {
                case 0x01:
                {
                    tbm_dev.left_water = msg.data[0];
                    prompt_play_by_cmd_id(LEFT_STOP_FETCH_WATER, -1, default_play_done_callback,true);  
                    uart_send_asr(LEFT_STOP_FETCH_WATER); 
                    break;
                }
                case 0x02:
                {
                    tbm_dev.left_heatperservation = msg.data[0];
                    prompt_play_by_cmd_id(STOP_INSULATION, -1, default_play_done_callback,true);  
                    uart_send_asr(STOP_INSULATION); 
                    break;
                }
                case 0x03:
                {
                    tbm_dev.left_refrigeration = msg.data[0];
                    prompt_play_by_cmd_id(STOP_COOLING, -1, default_play_done_callback,true);  
                    uart_send_asr(STOP_COOLING); 
                    break;
                }
                case 0x11:
                {
                    tbm_dev.left_water = msg.data[0];
                    prompt_play_by_cmd_id(LEFT_FETCH_WATER, -1, default_play_done_callback,true);  
                    uart_send_asr(LEFT_FETCH_WATER); 
                    break;
                }
                case 0x12:
                {
                    tbm_dev.left_refrigeration = 0x03;
                    tbm_dev.left_heatperservation = msg.data[0];
                    prompt_play_by_cmd_id(OPEN_INSULATION, -1, default_play_done_callback,true);  
                    uart_send_asr(OPEN_INSULATION); 
                    break;
                }
                case 0x13:
                {
                    tbm_dev.left_refrigeration = msg.data[0];
                    tbm_dev.left_heatperservation = 0x02;
                    prompt_play_by_cmd_id(OPEN_COOLING, -1, default_play_done_callback,true);  
                    uart_send_asr(OPEN_COOLING); 
                    break;
                } 
            default:
                break;
            }

            break;
        }
        case TBM_RIGHT:
        {
            mprintf("茶吧机右壶:%x\r\n",msg.data[0]); 
            switch (msg.data[0])
            {
                case 0x01:
                {
                    tbm_dev.right_autoBoilingWater = msg.data[0];
                    prompt_play_by_cmd_id(BOILING_WATER_OFF, -1, default_play_done_callback,true);  
                    uart_send_asr(BOILING_WATER_OFF); 
                    break;
                }
                case 0x02:
                {
                    tbm_dev.right_water = msg.data[0];
                    prompt_play_by_cmd_id(RIGHT_STOP_FETCH_WATER, -1, default_play_done_callback,true);  
                    uart_send_asr(RIGHT_STOP_FETCH_WATER); 
                    break;
                }
                case 0x03:
                {
                    tbm_dev.right_heat = msg.data[0];
                    prompt_play_by_cmd_id(BOILING_WATER_OFF, -1, default_play_done_callback,true);  
                    uart_send_asr(BOILING_WATER_OFF); 
                    break;
                }
                case 0x11:
                {
                    tbm_dev.right_heat = 0x03;
                    tbm_dev.right_autoBoilingWater = msg.data[0];
                    prompt_play_by_cmd_id(AUTO_WATER_BOILING, -1, default_play_done_callback,true);  
                    uart_send_asr(AUTO_WATER_BOILING); 
                    break;
                }
                case 0x12:
                {
                    tbm_dev.right_water = msg.data[0];
                    prompt_play_by_cmd_id(RIGHT_FETCH_WATER , -1, default_play_done_callback,true);  
                    uart_send_asr(RIGHT_FETCH_WATER ); 
                    break;
                }
                case 0x13:
                {
                    tbm_dev.right_heat = msg.data[0];
                    tbm_dev.right_autoBoilingWater = 0x01;
                    prompt_play_by_cmd_id(KETTLE_HEATING, -1, default_play_done_callback,true);  
                    uart_send_asr(KETTLE_HEATING); 
                    break;
                } 
            default:
                break;
            }

            break;
        }
        case TBM_TEMPERATURE:
        {
            mprintf("茶吧机调温:%d ℃\r\n",msg.data[0]); 
            tbm_dev.right_currentTmp = msg.data[0];

            uint16_t play_id = (tbm_dev.right_currentTmp - 40)/5+TEMPERATURE_40;

            prompt_play_by_cmd_id(play_id, -1, default_play_done_callback,true);  
            uart_send_asr(play_id); 

            break;
        }
        default:
        {
            mprintf("other cmd\r\n");
            break;
        }

    }
}



void tbm_query(ble_msg_V1_t msg)
{
    return;
}