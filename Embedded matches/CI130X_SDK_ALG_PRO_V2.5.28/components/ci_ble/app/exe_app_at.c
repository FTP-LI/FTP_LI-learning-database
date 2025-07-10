#include <stdint.h>
#include "stdio.h"
#include "ci_ble_spi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "ci_nvdata_manage.h"
#include "ci130x_gpio.h"
#include "ci130x_uart.h"
#include "exe_hal.h"
#include "exe_api.h"
#include "exe_app.h"
#include "sdk_default_config.h"

extern void reset_ble_task(void);
extern TaskHandle_t ble_task_handle;
typedef enum{
    AT_DATA_START = 0,
    AT_DATA_MAC = 1,
    AT_DATA_NAME = 2,
    AT_DATA_PWR = 3,
    AT_DATA_XTAL = 4,
    AT_DATA_MODE = 5,
    AT_DATA_CH = 6,
    AT_DATA_SN = 7,
    AT_DATA_ATE = 8
}ble_at_state_t;

typedef enum{
    MSG_IDE  = 0,
    MSG_TYPE = 1,
    MSG_ATE = 2,
    MSG_AT_DATA = 3,
    MSG_GET_DATA = 4,
    MSG_END = 5,
    MSG_END_ERRO = 6
}ble_msg_state_t;

QueueHandle_t product_msg_recv_queue = NULL;


uint8_t ble_mac[18] = {0};
uint8_t ble_sn[10] = "1234567890";
uint8_t ble_name[RF_RX_TX_MAX_LEN] = {0};
uint8_t ble_mode = 0;
uint8_t ble_ch = 2;

extern uint8_t ble_pwr;
extern uint8_t ble_xtal;     
extern uint8_t exe_gbuf_bda[6];
extern uint8_t exe_gbuf_adv_ind[42];


/**
 * @brief 串口接收AT指令的任务，将收到的串口消息，发送到串口解析的消息队列
 * 
 */
void at_msg_task(void)
{
    uint8_t temp_ch;
    while (1)
    {
        temp_ch = UartPollingReceiveData(CONFIG_CI_LOG_UART_PORT);
        xQueueSendFromISR(product_msg_recv_queue, &temp_ch, pdFALSE);
        
    }
}

/**
 * @brief 接收串口消息队列，解析AT指令，配置蓝牙芯片出厂参数的任务
 * 
 */
void product_recv_task()
{
    uint8_t product_recv_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t product_send_data[RF_RX_TX_MAX_LEN] = {0};
    uint8_t temp_recv_data, product_recv_length = 0, product_send_length = 0;
    uint8_t msg_state = MSG_IDE;
    uint8_t at_data_state = AT_DATA_START;
    bool updata_ble_status = false;
    product_msg_recv_queue =  xQueueCreate(255, sizeof(int8_t));

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_BLE_MAC, 17, ble_mac, &product_recv_length))
    {
/*         for (int i = 0; i < 6; i++)  
        {  
            sprintf(&ble_mac[i*3], "%02x:", exe_gbuf_bda[5-i]);  
        } 
        ble_mac[17] = '\0'; */
        mprintf("cinv_item_read ble_mac :%s\r\n",ble_mac);
    }
    else
    {
        mprintf("cinv_item_read NVDATA_ID_BLE_MAC sucess %s\r\n",ble_mac);
        StrToMacHex(exe_gbuf_bda, ble_mac, 6);
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_BLE_NAME, RF_RX_TX_MAX_LEN, ble_name, &product_recv_length))
    {
        //mprintf("cinv_item_read NVDATA_ID_BLE_NAME fail\r\n");
        ble_name[0] = 0x07;
        ble_name[1] = 0x09;
        memcpy(&ble_name[2], DEFAULT_NAME, 6);
        memcpy(&exe_gbuf_adv_ind[19], ble_name, ble_name[0]+1);
        cinv_item_init(NVDATA_ID_BLE_NAME, RF_RX_TX_MAX_LEN, ble_name);
    }
    else
    {
        memcpy(&exe_gbuf_adv_ind[19], ble_name, ble_name[0]+1);
        //mprintf("cinv_item_read NVDATA_ID_BLE_NAME sucess %s %d \r\n",ble_name, ble_name[0]);
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_BLE_PWR, 1, &ble_pwr, &product_recv_length))
    {
        //mprintf("cinv_item_read NVDATA_ID_BLE_PWR fail\r\n");
        cinv_item_init(NVDATA_ID_BLE_PWR, 1, &ble_pwr);
    }
    else
    {
       // mprintf("cinv_item_read NVDATA_ID_BLE_PWR sucess %d\r\n",ble_pwr);
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_BLE_XTAL, 1, &ble_xtal, &product_recv_length))
    {
        //mprintf("cinv_item_read NVDATA_ID_BLE_XTAL fail\r\n");
        cinv_item_init(NVDATA_ID_BLE_XTAL, 1, &ble_xtal);
    }
    else
    {
        //mprintf("cinv_item_read NVDATA_ID_BLE_XTAL sucess %s\r\n",ble_xtal);
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_BLE_MODE, 1, &ble_mode, &product_recv_length))
    {
        //mprintf("cinv_item_read NVDATA_ID_BLE_MODE fail\r\n");
        cinv_item_init(NVDATA_ID_BLE_MODE, 1, &ble_mode);
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(NVDATA_ID_BLE_CH, 1, &ble_ch, &product_recv_length))
    {
        //mprintf("cinv_item_read NVDATA_ID_BLE_CH fail\r\n");
        cinv_item_init(NVDATA_ID_BLE_CH, 1, &ble_ch);
    }

/*     for (size_t i = 0; i < 42; i++)
    {
        mprintf("%x ", exe_gbuf_adv_ind[i]);
    } */
    
    if (product_msg_recv_queue == NULL)
    {
        //mprintf("network_msg_recv_queue create fail\r\n");
    }

    while (1)
    {
        if (xQueueReceive(product_msg_recv_queue, &temp_recv_data, portMAX_DELAY))
        {
           switch (msg_state)
            {
                case MSG_IDE:
                    if (temp_recv_data == '+')
                    {
                        msg_state = MSG_TYPE;
                        product_recv_length = 0;
                        memset(product_recv_data, 0, RF_RX_TX_MAX_LEN);
                    }
                    break;

                case MSG_TYPE:
                    product_recv_data[product_recv_length] = temp_recv_data;
                    product_recv_length ++;
                    if (temp_recv_data == '=')
                    {
                        if (memcmp(product_recv_data, "ATE_OK", 6) == 0)
                        {
                            msg_state = MSG_ATE;
                        }
                        else
                        {
                            msg_state = MSG_AT_DATA;                                                                  
                            if (memcmp(product_recv_data, "BLEMAC", 6) == 0)
                                at_data_state = AT_DATA_MAC;
                            else if (memcmp(product_recv_data, "BLENAME", 7) == 0)
                                at_data_state = AT_DATA_NAME;
                            else if (memcmp(product_recv_data, "BLEPWR", 6) == 0)
                                at_data_state = AT_DATA_PWR;
                            else if (memcmp(product_recv_data, "BLEXTAL", 7) == 0)
                                at_data_state = AT_DATA_XTAL;
                            else if (memcmp(product_recv_data, "BLEMODE", 7) == 0)
                                at_data_state = AT_DATA_MODE;
                            else if (memcmp(product_recv_data, "BLECH", 5) == 0)
                                at_data_state = AT_DATA_CH;
                        }

                        product_recv_length = 0;
                        memset(product_recv_data, 0, RF_RX_TX_MAX_LEN);
                    }
                    else if (temp_recv_data == '\r')
                    {
                        if (memcmp(product_recv_data, "RST_BLE", 7) == 0)
                        {
                            mprintf("RST_BLE\r\n");
                            reset_ble_task();
                        }
                        else
                        {
                            mprintf("RST\r\n");
                            dpmu_software_reset_system_config();
                        }
                        msg_state = MSG_END;
                    }
                    else if (temp_recv_data == '?')
                    {
                        msg_state = MSG_GET_DATA;
                        if (memcmp(product_recv_data, "BLEMAC", 6) == 0)
                            at_data_state = AT_DATA_MAC;
                        else if (memcmp(product_recv_data, "BLENAME", 7) == 0)
                            at_data_state = AT_DATA_NAME;
                        else if (memcmp(product_recv_data, "ATE_OK", 6) == 0)
                            at_data_state = AT_DATA_ATE;
                        else if (memcmp(product_recv_data, "BLEMODE", 7) == 0)
                            at_data_state = AT_DATA_MODE;
                        else if (memcmp(product_recv_data, "BLEPWR", 6) == 0)
                            at_data_state = AT_DATA_PWR;
                        else if (memcmp(product_recv_data, "BLEXTAL", 7) == 0)
                            at_data_state = AT_DATA_XTAL;
                        else if (memcmp(product_recv_data, "BLECH", 5) == 0)
                            at_data_state = AT_DATA_CH;
                        product_recv_length = 0;
                        memset(product_recv_data, 0, RF_RX_TX_MAX_LEN);
                    }
                    break;

                case MSG_ATE:
                    product_recv_data[product_recv_length] = temp_recv_data;
                    product_recv_length ++;
                    if (temp_recv_data == '\r')
                    {
                        if (product_recv_data[0] == '1')
                        {
                            mprintf(" ate ok \r\n");
                            updata_ate();
                        }
                        else
                        {
                            mprintf(" ate erro \r\n");
                            ate_err();
                            updata_ate();
                            reset_ble_task();
                        }
                        msg_state = MSG_END;
                    }
                    break;

                case MSG_AT_DATA:
                    product_recv_data[product_recv_length] = temp_recv_data;
                    product_recv_length ++;

                    if (temp_recv_data == '\r')
                    {
                        switch (at_data_state)
                        {
                            case AT_DATA_MAC:
                                updata_ble_status = updata_ble_mac(&product_recv_data[1], product_recv_length-2);
                                break;
                            case AT_DATA_NAME:
                                updata_ble_status = updata_ble_name(&product_recv_data[1], product_recv_length-2);
                                break;
                            case AT_DATA_PWR:
                                updata_ble_status = updata_ble_pwr(product_recv_data[0]);
                                break;
                            case AT_DATA_XTAL:
                                updata_ble_status = updata_ble_xtal(product_recv_data, product_recv_length);
                                break;
                            case AT_DATA_MODE:
                                updata_ble_status = updata_ble_mode(product_recv_data[0]);
                                break;
                            case AT_DATA_CH:
                                updata_ble_status = updata_ble_ch(product_recv_data, product_recv_length);
                                break;
                        }
                        if (updata_ble_status)
                        {
                            reset_ble_task();
                            msg_state = MSG_END;
                        }
                        else
                            msg_state = MSG_END_ERRO;
                        
                        at_data_state = AT_DATA_START;
                        
                    }           
                    break;

                case MSG_GET_DATA:
                    product_recv_data[product_recv_length] = temp_recv_data;
                    product_recv_length ++;

                    if (temp_recv_data == '\r')
                    {
                        switch (at_data_state)
                        {
                            case AT_DATA_MAC:
                                product_send_length = 17;
                                memcpy(product_send_data, ble_mac, 17);
                                break;

                            case AT_DATA_NAME:
                                product_send_length = exe_gbuf_adv_ind[19] - 1;
                                memcpy(product_send_data, &exe_gbuf_adv_ind[21], product_send_length);
                                break;

                            case AT_DATA_ATE:
                                product_send_data[0] = ble_pwr  + 0x30;
                                product_send_data[1] = ',';
                                if (ble_xtal > 9)
                                {
                                    product_send_length = 4;
                                    product_send_data[2] = ble_xtal/10 + 0x30;
                                    product_send_data[3] = ble_xtal%10 + 0x30;
                                }
                                else 
                                {
                                    product_send_length = 3;
                                    product_send_data[2] = ble_xtal + 0x30;
                                }
                                break;

                            case AT_DATA_PWR:
                                product_send_data[0] = ble_pwr  + 0x30;
                                product_send_length = 1;
                                break;

                            case AT_DATA_XTAL:
                                product_send_length = 2;
                                product_send_data[0] = ble_xtal/10 + 0x30;
                                product_send_data[1] = ble_xtal%10 + 0x30;
                                break;

                            case AT_DATA_MODE:
                                product_send_data[0] = ble_mode  + 0x30;
                                product_send_length = 1;
                                break;

                            case AT_DATA_CH:
                                if (ble_ch > 9)
                                {
                                    product_send_length = 2;
                                    product_send_data[0] = ble_ch/10 + 0x30;
                                    product_send_data[1] = ble_ch%10 + 0x30;
                                }
                                else 
                                {
                                    product_send_length = 1;
                                    product_send_data[0] = ble_ch + 0x30;
                                }
                                break;

                            case AT_DATA_SN:
                                memcpy(product_send_data, ble_sn, 10);
                                product_send_length = 10;
                                break;

                            default:
                                product_send_data[0] = 'e';
                                product_send_data[1] = 'r';
                                product_send_length = 2;
                                break;
                        }
                        replay_data(product_send_data, product_send_length);
                        at_data_state = AT_DATA_START;
                        msg_state = MSG_END;
                    }  
                    break;

                case MSG_END:
                    if (temp_recv_data == '\n')
                    {
                        mprintf("msg end\r\n");
                        msg_state = MSG_IDE;
                        product_recv_length = 0;
                        memset(product_recv_data, 0, RF_RX_TX_MAX_LEN);
                        replay_ok();
                    }
                    break;

                case MSG_END_ERRO:
                    if (temp_recv_data == '\n')
                    {
                        mprintf("msg end erro\r\n");
                        msg_state = MSG_IDE;
                        product_recv_length = 0;
                        memset(product_recv_data, 0, RF_RX_TX_MAX_LEN);
                        replay_erro();
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

int uart0_port_send_char(uint8_t ch)
{
    UartPollingSenddata(UART0, ch);
}
/**
 * @brief 返回AT指令读取蓝牙芯片参数的数据
 * 
 */
void replay_data(uint8_t *send_data, uint8_t send_len)
{
    for (size_t i = 0; i < send_len; i++)
    {
        uart0_port_send_char(send_data[i]);
    }
    
    uart0_port_send_char('\r');
    uart0_port_send_char('\n');
}
/**
 * @brief 返回解析AT指令成功
 * 
 */
void  replay_ok()
{
    uart0_port_send_char('O');
    uart0_port_send_char('K');
    uart0_port_send_char('\r');
    uart0_port_send_char('\n');
}

/**
 * @brief 返回解析AT指令失败
 * 
 */
void  replay_erro()
{
    uart0_port_send_char('E');
    uart0_port_send_char('R');
    uart0_port_send_char('R');
    uart0_port_send_char('O');
    uart0_port_send_char('\r');
    uart0_port_send_char('\n');
}


void StrToMacHex(unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
    char h1,h2;
    unsigned char s1,s2;
    int i;
    for (i=0; i<nLen; i++)
    {
        
        h1 = pbSrc[3*i];
        h2 = pbSrc[3*i+1];
        s1 = toupper(h1) - 0x30;
            if (s1 > 9)
                s1 -= 7;
        s2 = toupper(h2) - 0x30;
            if (s2 > 9)
                s2 -= 7;
        pbDest[5-i] = s1*16 + s2;
    }
}
 
/**
 * @brief 将AT指令配置的蓝牙芯片参数固化到芯片flash
 * 
 */
void updata_ate()
{
    cinv_item_write(NVDATA_ID_BLE_PWR, 1, &ble_pwr);
    cinv_item_write(NVDATA_ID_BLE_XTAL, 1, &ble_xtal);
    cinv_item_write(NVDATA_ID_BLE_MODE, 1, &ble_mode);
    cinv_item_write(NVDATA_ID_BLE_CH, 1, &ble_ch);
}

/**
 * @brief 取消AT指令配置的蓝牙芯片参数，将参数从新设置为默认值
 * 
 */
void ate_err()
{
    ble_pwr = DEFAULT_PWR;
    ble_xtal = DEFAULT_XTAL; 

}

/**
 * @brief AT指令配置的蓝牙芯片的MAC地址，立即生效并固化到flash
 * 
 */
bool updata_ble_mac(uint8_t *recv_data, uint8_t recv_len)
{
    if (recv_len == 18)
    {
        memcpy(ble_mac, recv_data, 17);
        cinv_item_write(NVDATA_ID_BLE_MAC, 17, ble_mac);
        StrToMacHex(exe_gbuf_bda, ble_mac, 6);
        return true;
    }
    else 
        return false;
}

/**
 * @brief AT指令配置的蓝牙芯片的广播名字，立即生效并固化到flash
 * 
 */
bool updata_ble_name(uint8_t *recv_data, uint8_t recv_len)
{
    if ((recv_len>=3)&&(recv_len<=22))
    {
        mprintf("%s len = %d\r\n",recv_data,recv_len);
        memset(ble_name, 0, RF_RX_TX_MAX_LEN);
        memset(&exe_gbuf_adv_ind[19], 0, RF_RX_TX_MAX_LEN);
        ble_name[0] = recv_len ;
        ble_name[1] = 0x09 ;
        memcpy(&ble_name[2], recv_data, ble_name[0]-1);
        memcpy(&exe_gbuf_adv_ind[19], ble_name, ble_name[0]+1);
        cinv_item_write(NVDATA_ID_BLE_NAME, ble_name[0]+1, ble_name);
        return true;
    }
    else 
        return false;

}

/**
 * @brief AT指令配置的蓝牙芯片的射频发送功率
 * 
 */
bool updata_ble_pwr(uint8_t recv_data)
{
    if((recv_data >= 0x30)&&(recv_data <= 0x37)) 
    {
        ble_pwr = recv_data - 0x30;
        return true;
    }
    else 
        return false;
}

/**
 * @brief AT指令配置的蓝牙芯片的晶振频偏
 * 
 */
bool updata_ble_xtal(uint8_t* recv_data, uint8_t recv_len)
{
    uint8_t temp_xtal;
    if(recv_len == 2)
        temp_xtal = recv_data[0] - 0x30;
    else
    {
        temp_xtal = (recv_data[0] - 0x30)*10;
        temp_xtal = temp_xtal + (recv_data[1] - 0x30);
    }

    if (temp_xtal <= 31)
    {
        ble_xtal = temp_xtal;
        return true;
    }
    else 
        return false;
}

/**
 * @brief AT指令配置的蓝牙芯片的载波模式
 * 
 */
bool updata_ble_mode(uint8_t recv_data)
{
    if ((recv_data>=0x30)&&(recv_data <= 0x35))
    {
        ble_mode = recv_data - 0x30;
        return true;
    }
    else 
    {
        return false;
    }
}

/**
 * @brief AT指令配置的蓝牙芯片的射频发送频道
 * 
 */
bool updata_ble_ch(uint8_t* recv_data, uint8_t recv_len)
{
    uint8_t temp_ch;
    if(recv_len == 2)
        temp_ch = recv_data[0] - 0x30;
    else
    {
        temp_ch = (recv_data[0] - 0x30)*10;
        temp_ch = temp_ch + (recv_data[1] - 0x30);
    }

    if (temp_ch <= 80)
    {
        ble_ch = temp_ch;
        return true;
    }
    else 
        return false;
}

void  string_to_uint32(char *temp_str, const char* str, uint8_t recv_len)
{
    uint32_t result = 0;

    for (size_t i = 0; i < recv_len; i++) {
        if(str[i]< '0')
        break;
        result = result * 10 + (str[i] - '0');
    }

    temp_str[0] = (result >> 24) & 0xFF;
    temp_str[1] = (result >> 16) & 0xFF;
    temp_str[2] = (result >> 8) & 0xFF;
    temp_str[3] = result & 0xFF;
}

