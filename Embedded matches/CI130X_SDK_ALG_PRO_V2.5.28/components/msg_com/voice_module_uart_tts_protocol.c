#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "voice_module_uart_tts_protocol.h"
#include "system_msg_deal.h"
#include "command_info.h"
#include "ci130x_spiflash.h"
#include "prompt_player.h"
#include "audio_play_api.h"
#include "audio_in_manage.h"
#include "ringbuffer.h"
#include "tts_audio.h"
#include "string.h"
#include "user_config.h"

#if USE_TTS

#define TIMEOUT_ONE_PACKET_INTERVAL (1000) /*ms, in this code, it should be bigger than portTICK_PERIOD_MS */
#define MAX_DATA_RECEIVE_PER_PACKET (80)   /*???*/

#define VMUP_PACKET_MIN_SIZE (8)
#define VMUP_PACKET_MAX_SIZE (VMUP_MSG_DATA_MAX_SIZE + VMUP_PACKET_MIN_SIZE)

const char tts_ring[] = "ring_";
const char tts_message[] = "message_";
const char tts_alert[] = "alert_";
const char tts_sound[] = "sound";
const char tts_defult_vst[] = "[g1][s5][t5][v5]";
/********************************************************************
                     port function
********************************************************************/
static SemaphoreHandle_t send_tts_packet_mutex;

#define log_debug(fmt, args...) \
    do                          \
    {                           \
    } while (0)
#define log_info _printf
#define log_error _printf
#define cinv_assert(x) \
    do                 \
    {                  \
    } while (0)


//发送音量语速语调
int vmup_port_send_packet_rev_play_parameter_msg(sys_tts_msg_com_data_t *msg)
{
    sys_tts_msg_t send_msg;
    BaseType_t xResult;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   // mprintf("vmup_port_send_packet parameter\r\n");
    send_msg.msg_type = SYS_MSG_TYPE_SET_PALY_PARAMETER;
    memcpy((uint8_t *)(&send_msg.msg_data), msg, sizeof(sys_tts_msg_com_data_t));

    xResult = send_tts_msg_to_sys_task(&send_msg, &xHigherPriorityTaskWoken);

    if ((xResult != pdFAIL) && (pdTRUE == xHigherPriorityTaskWoken))
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return xHigherPriorityTaskWoken;
}
//发送其他普通命令
int vmup_port_send_packet_rev_cmd_msg(sys_tts_msg_com_data_t *msg)
{
    sys_tts_msg_t send_msg;
    BaseType_t xResult;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   // mprintf("vmup_port_send_packet cmd\r\n");
    send_msg.msg_type = SYS_MSG_TYPE_COM;
    memcpy((uint8_t *)(&send_msg.msg_data), msg, sizeof(sys_tts_msg_com_data_t));

    xResult = send_tts_msg_to_sys_task(&send_msg, &xHigherPriorityTaskWoken);

    if ((xResult != pdFAIL) && (pdTRUE == xHigherPriorityTaskWoken))
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return xHigherPriorityTaskWoken;
}
//发送带文本的指令
int vmup_port_send_packet_rev_text_msg(sys_tts_msg_com_data_t *msg)
{
    sys_tts_msg_t send_msg;
    BaseType_t xResult;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   // mprintf("vmup_port_send_packet text\r\n");
    //init_timer0();
	//timer0_start_count();
    send_msg.msg_type = SYS_MSG_TYPE_TEXT;
    memcpy((uint8_t *)(&send_msg.msg_data), msg, sizeof(sys_tts_msg_com_data_t));

    xResult = send_tts_msg_to_sys_task(&send_msg, &xHigherPriorityTaskWoken);

    if ((xResult != pdFAIL) && (pdTRUE == xHigherPriorityTaskWoken))
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return xHigherPriorityTaskWoken;
}
static TickType_t last_time;
static bool vmup_port_timeout_one_packet(void)
{
    TickType_t now_time;
    TickType_t timeout;

    now_time = xTaskGetTickCountFromISR();

    timeout = (now_time - last_time); /*uint type, so overflow just used - */

    last_time = now_time;

    if (timeout > TIMEOUT_ONE_PACKET_INTERVAL / portTICK_PERIOD_MS) /*also as timeout = timeout*portTICK_PERIOD_MS;*/
    {
        return true;
    }
    else
    {
        return false;
    }
}

static int vmup_port_send_char(uint8_t ch)
{
    UartPollingSenddata((UART_TypeDef *)UART_TTS_NUMBER, ch);
    return RETURN_OK;
}

static void vump_tts_uart_irq_handler(void)
{
    /*发送数据*/
    if (((UART_TypeDef *)UART_TTS_NUMBER)->UARTMIS & (1UL << UART_TXInt))
    {
        UART_IntClear((UART_TypeDef *)UART_TTS_NUMBER, UART_TXInt);
    }
    /*接受数据*/
    if (((UART_TypeDef *)UART_TTS_NUMBER)->UARTMIS & (1UL << UART_RXInt))
    {

        vmup_tts_receive_packet(UART_RXDATA((UART_TypeDef *)UART_TTS_NUMBER));
        UART_IntClear((UART_TypeDef *)UART_TTS_NUMBER, UART_RXInt);
    }

    UART_IntClear((UART_TypeDef *)UART_TTS_NUMBER, UART_AllInt);
}
//#pragma GCC optimize("O0")
static int vmup_tts_port_hw_init(void)
{
    __eclic_irq_set_vector(UART_TTS_IRQ, (int32_t)vump_tts_uart_irq_handler);
    UARTInterruptConfig((UART_TypeDef *)UART_TTS_NUMBER, UART_TTS_BAUDRATE);  
    return RETURN_OK;
}

/********************************************************************
                     receive function
********************************************************************/
typedef enum
{
    TTS_REV_STATE_HEAD = 0x00,
    TTS_REV_STATE_LENGTH0 = 0x01,
    TTS_REV_STATE_LENGTH1 = 0x02,
    TTS_REV_STATE_CMD = 0x03,
    TTS_REV_STATE_TYPE = 0x04,
    TTS_REV_STATE_DATA = 0x05,
} vmup_TTS_receive_state_t;
//向tts上位机发送
sys_tts_msg_com_data_t tts_recever_packet;
static uint8_t rev_state = TTS_REV_STATE_HEAD;
static uint16_t length0 = 0, length1 = 0;
static uint16_t chk_sum0 = 0, chk_sum1 = 0;
static uint16_t data_rev_count = 0;
static uint16_t packet_number_even = 0;
static uint16_t packet_number_odd = 0;
static uint16_t packet_number = 0; //动态计算每次截取的数据个数
static uint16_t packet_number_ascll = 0;
void vmup_tts_send_receive_ack(uint8_t status)
{
    //mprintf("vmup_tts_send_receive_ack 0x%x\r\n",status);
    vmup_port_send_char(status);
}

extern bool audio_plyer_status;
void vmup_tts_receive_packet(uint8_t receive_char)
{
    if (true == vmup_port_timeout_one_packet())
    {
        rev_state = TTS_REV_STATE_HEAD;
    }
    //mprintf("%x \n", receive_char);
    uint16_t remain_data = 0;
    switch (rev_state)
    {
    case TTS_REV_STATE_HEAD:
        if (VMUP_TTS_MSG_HEAD == receive_char)
        {

            rev_state = TTS_REV_STATE_LENGTH0;
        }
        else
        {
            rev_state = TTS_REV_STATE_HEAD;
        }
        break;
    case TTS_REV_STATE_LENGTH0:
        length1 = receive_char;
        rev_state = TTS_REV_STATE_LENGTH1;
        break;
    case TTS_REV_STATE_LENGTH1:
        length0 = receive_char;
        length1 <<= 8;
        length1 += length0;
        if (length1 > 0)
        {
            tts_recever_packet.data_length = length1;
            rev_state = TTS_REV_STATE_CMD;
        }
        else
        {
            vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_RECEIVE_INCOMPLETE);
            rev_state = TTS_REV_STATE_HEAD;
        }
        break;
    case TTS_REV_STATE_CMD:
        tts_recever_packet.msg_cmd = receive_char;
        if (tts_recever_packet.msg_cmd == VMUP_TTS_MSG_CMD_START) //区分带文本命令和其他命令
        {
            rev_state = TTS_REV_STATE_TYPE;
        }
        else
        {
            if (tts_recever_packet.msg_cmd == VMUP_TTS_MSG_CMD_STOP || tts_recever_packet.msg_cmd == VMUP_TTS_MSG_CMD_PAUSE || tts_recever_packet.msg_cmd == VMUP_TTS_MSG_CMD_RESUME || tts_recever_packet.msg_cmd == VMUP_TTS_MSG_CMD_STATUS)
            {
                 //mprintf("CMD=%x\n", (tts_recever_packet.msg_cmd));
                vmup_port_send_packet_rev_cmd_msg(&tts_recever_packet);
                if (tts_recever_packet.msg_cmd != VMUP_TTS_MSG_CMD_STATUS)
                {
                    vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_RECEIVE_SUCCESS);
                }
                else
                {
                    if (audio_plyer_status)
                    {
                        vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_BUSY);
                    }
                    else
                    {
                        vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_IDLE);
                    }
                }
                memset(tts_recever_packet.msg_data, 0, VMUP_MSG_DATA_MAX_SIZE);
                data_rev_count = 0;
                rev_state = TTS_REV_STATE_HEAD;
            }
            #if USER_CODE_SWITCH_ENABLE
            else if(tts_recever_packet.msg_cmd == VMUP_TTS_CHANGE_ASR_STATUS)
            {
                mprintf("tts switch to asr status...\r\n");
                extern QueueHandle_t uart_msg_recv_queue;
                uint8_t send_buf[8] = {0xFD, 0x00, 0x01, 0x22, 0x00, 0x00, 0x00, 0x00};
                if (xQueueSendFromISR(uart_msg_recv_queue, send_buf, pdFALSE) == pdFALSE)
                {
                    mprintf("uart_msg_recv_queue send error....\r\n");
                }
            }
            #endif
            else
            {
                vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_RECEIVE_ERR);
                data_rev_count = 0;
                rev_state = TTS_REV_STATE_HEAD;
            }
        }
        break;
    case TTS_REV_STATE_TYPE:
        tts_recever_packet.msg_type = receive_char;
        rev_state = TTS_REV_STATE_DATA;
        data_rev_count = 0;
        break;
    case TTS_REV_STATE_DATA:
        tts_recever_packet.msg_data[data_rev_count] = receive_char;
        //mprintf("receive_char=%x,\n", receive_char);
        data_rev_count++;

        //解析是否是调节音量语速语调为默认值
        if ((length1 - 2) == VMUP_TTS_MSG_TYPE_DEFAULT_VST_LEGTH || (length1 - 2) == VMUP_TTS_MSG_TYPE_RING_LEGTH || (length1 - 2) == VMUP_TTS_MSG_TYPE_MESSAGE_LEGTH || (length1 - 2) == VMUP_TTS_MSG_TYPE_ALERT_LEGTH)

        {
            if ((strstr((const char *)(tts_recever_packet.msg_data), tts_ring) && tts_recever_packet.msg_data[5] != 0) ||
                (strstr((const char *)(tts_recever_packet.msg_data), tts_message) && tts_recever_packet.msg_data[8] != 0) ||
                (strstr((const char *)(tts_recever_packet.msg_data), tts_alert) && tts_recever_packet.msg_data[6] != 0) ||
                (strstr((const char *)(tts_recever_packet.msg_data), tts_defult_vst) && tts_recever_packet.msg_data[15] != 0))
            {
                BaseType_t flag;
                vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_RECEIVE_SUCCESS);
                vmup_port_send_packet_rev_cmd_msg(&tts_recever_packet);
                vmup_port_send_packet_rev_play_parameter_msg(&tts_recever_packet);
                memset(tts_recever_packet.msg_data, 0, VMUP_MSG_DATA_MAX_SIZE);
                data_rev_count = 0;
                rev_state = TTS_REV_STATE_HEAD;
                break;
            }
        }
        
        // xTimerResetFromISR(timer_receive_data_timeout, 0);
        //计算合适的截取位置
        if (data_rev_count == PACKET_MIN_SIZE)
        {
            for (int i = 0; i < PACKET_MIN_SIZE; i++)
            {
                if (tts_recever_packet.msg_data[i] <= 0x7E && tts_recever_packet.msg_data[i] > 0) //判断是否是单字节的ASCLL
                {
                    packet_number_ascll++;
                }
            }
            if (packet_number_ascll % 2 != 1)
            {
                packet_number = PACKET_MIN_SIZE;
                packet_number_odd++;
            }
            else
            {
                packet_number = PACKET_MIN_SIZE + 1;
                packet_number_even++;
            }
            //mprintf("packet_number_ascll=%d,%d,%d,%d\n", packet_number_ascll, packet_number_odd, packet_number_even,packet_number);
            packet_number_ascll = 0;
        }

        remain_data = (length1 - 2) - ((packet_number_even * (PACKET_MIN_SIZE + 1)) + (packet_number_odd * PACKET_MIN_SIZE));
        if (data_rev_count % packet_number == 0 && data_rev_count != 0)
        {

            vmup_port_send_packet_rev_play_parameter_msg(&tts_recever_packet);
            vmup_port_send_packet_rev_text_msg(&tts_recever_packet);
            memset(tts_recever_packet.msg_data, 0, VMUP_MSG_DATA_MAX_SIZE);
            data_rev_count = 0;
            if (remain_data == 0)
            {

                // xTimerStopFromISR(timer_receive_data_timeout, 0);
                vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_RECEIVE_SUCCESS);
                packet_number_even = 0;
                packet_number_odd = 0;
                packet_number = 0;
                data_rev_count = 0;
                vmup_port_send_packet_rev_play_parameter_msg(&tts_recever_packet);
                vmup_port_send_packet_rev_text_msg(&tts_recever_packet);
                vmup_port_send_packet_rev_cmd_msg(&tts_recever_packet);
                rev_state = TTS_REV_STATE_HEAD;
            }
        }
        else if (remain_data == data_rev_count)
        {
            // xTimerStopFromISR(timer_receive_data_timeout, 0);
            vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_RECEIVE_SUCCESS);
            tts_recever_packet.data_length = data_rev_count;
            vmup_port_send_packet_rev_play_parameter_msg(&tts_recever_packet);
            vmup_port_send_packet_rev_text_msg(&tts_recever_packet);
           vmup_port_send_packet_rev_cmd_msg(&tts_recever_packet);
            memset(tts_recever_packet.msg_data, 0, VMUP_MSG_DATA_MAX_SIZE);
            data_rev_count = 0;
            packet_number = 0;
            packet_number_even = 0;
            packet_number_odd = 0;
            rev_state = TTS_REV_STATE_HEAD;
        }
        break;
    default:
        rev_state = TTS_REV_STATE_HEAD;
        break;
    }
}

void vmup_tts_communicate_init(void)
{
    vmup_tts_port_hw_init();
}

/********************************************************************
                     user add function
********************************************************************/


int set_tts_init_done(uint32_t type)
{
    if(type == 1)
    {
        vmup_tts_send_receive_ack(VMUP_TTS_MSG_CMD_IDLE); //初始化完成
    }
}
#else
int set_tts_init_done(uint32_t type)   
{

}
#endif
