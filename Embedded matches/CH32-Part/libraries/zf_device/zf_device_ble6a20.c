/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library ����CH32V307VCT6 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ���CH32V307VCT6 ��Դ���һ����
*
* CH32V307VCT6 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_device_wireless_ble6a20
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MounRiver Studio V1.8.1
* ����ƽ̨          CH32V307VCT6
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����                                      ����                             ��ע
* 2024-06-25        ��W            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   RX                  �鿴 zf_device_wireless_ble6a20.h �� BLE6A20_RX_PIN  �궨��
*                   TX                  �鿴 zf_device_wireless_ble6a20.h �� BLE6A20_TX_PIN  �궨��
*                   RTS                 �鿴 zf_device_wireless_ble6a20.h �� BLE6A20_RTS_PIN �궨��
*                   VCC                 3.3V��Դ
*                   GND                 ��Դ��
*                   ������������
*                   ------------------------------------
*********************************************************************************************************************/

#include "zf_device_ble6a20.h"
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"
#include "zf_device_type.h"

static  fifo_struct                                     ble6a20_fifo;
static  uint8                                           ble6a20_buffer[BLE6A20_BUFFER_SIZE];

static          uint8                                   ble6a20_data          = 0;
#if (1 == BLE6A20_AUTO_BAUD_RATE)
static volatile ble6a20_auto_baudrate_state_enum  ble6a20_auto_baud_flag     = BLE6A20_AUTO_BAUD_RATE_INIT;
static volatile uint8                                   ble6a20_auto_baud_data[3]  = {0x00, 0x01, 0x03};
#endif

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ת����ģ�� ��������
// ����˵��     data            8bit ����
// ���ز���     uint32          ʣ�෢�ͳ��� 0-������� 1-δ�������
// ʹ��ʾ��     ble6a20_send_byte(data);
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
uint32 ble6a20_send_byte (const uint8 data)
{
    uint16 time_count = BLE6A20_TIMEOUT_COUNT;
    while(time_count)
    {
        if(!gpio_get_level(BLE6A20_RTS_PIN))
        {
            uart_write_byte(BLE6A20_INDEX, data);                         // ��������
            break;
        }
        time_count --;
        system_delay_ms(1);
    }
    return (0 == time_count);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ת����ģ�� �������ݿ�
// ����˵��     *buff           ���ͻ�����
// ����˵��     len             �������ݳ���
// ���ز���     uint32          ʣ�෢�ͳ���
// ʹ��ʾ��     ble6a20_send_buffer(buff, 64);
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
uint32 ble6a20_send_buffer (const uint8 *buff, uint32 len)
{
    zf_assert(NULL != buff);
    uint16 time_count = 0;
    while(0 != len)
    {
        if(!gpio_get_level(BLE6A20_RTS_PIN))                              // ���RTSΪ�͵�ƽ �������������
        {
            if(30 <= len)                                                       // ���ݷ� 30byte ÿ������
            {
                uart_write_buffer(BLE6A20_INDEX, buff, 30);               // ��������
                buff += 30;                                                     // ��ַƫ��
                len -= 30;                                                      // ����
                time_count = 0;
            }
            else                                                                // ���� 30byte ������һ���Է������
            {
                uart_write_buffer(BLE6A20_INDEX, buff, len);              // ��������
                len = 0;
                break;
            }
        }
        else                                                                    // ���RTSΪ�ߵ�ƽ ��ģ��æ
        {
            if(BLE6A20_TIMEOUT_COUNT <= (++ time_count))                  // ���������ȴ�ʱ��
            {
                break;                                                          // �˳�����
            }
            system_delay_ms(1);
        }
    }
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ת����ģ�� �����ַ���
// ����˵��     *str            Ҫ���͵��ַ�����ַ
// ���ز���     uint32          ʣ�෢�ͳ���
// ʹ��ʾ��     ble6a20_send_string("Believe in yourself.");
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
uint32 ble6a20_send_string (const char *str)
{
    zf_assert(NULL != str);
    uint16 time_count = 0;
    uint32 len = strlen(str);
    while(0 != len)
    {
        if(!gpio_get_level(BLE6A20_RTS_PIN))                              // ���RTSΪ�͵�ƽ �������������
        {
            if(128 <= len)                                                       // ���ݷ� 128byte ÿ������
            {
                uart_write_buffer(BLE6A20_INDEX, (const uint8 *)str, 128); // ��������
                str += 128;                                                      // ��ַƫ��
                len -= 128;                                                      // ����
                time_count = 0;
            }
            else                                                                // ���� 128byte ������һ���Է������
            {
                uart_write_buffer(BLE6A20_INDEX, (const uint8 *)str, len);// ��������
                len = 0;
                break;
            }
        }
        else                                                                    // ���RTSΪ�ߵ�ƽ ��ģ��æ
        {
            if(BLE6A20_TIMEOUT_COUNT <= (++ time_count))                  // ���������ȴ�ʱ��
            {
                break;                                                          // �˳�����
            }
            system_delay_ms(1);
        }
    }
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ת����ģ�� ��������ͷͼ������λ���鿴ͼ��
// ����˵��     *image_addr     ��Ҫ���͵�ͼ���ַ
// ����˵��     image_size      ͼ��Ĵ�С
// ���ز���     void
// ʹ��ʾ��     ble6a20_send_image(&mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
void ble6a20_send_image (const uint8 *image_addr, uint32 image_size)
{
    zf_assert(NULL != image_addr);
    extern uint8 camera_send_image_frame_header[4];
    ble6a20_send_buffer(camera_send_image_frame_header, 4);
    ble6a20_send_buffer((uint8 *)image_addr, image_size);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ת����ģ�� ��ȡ����
// ����˵��     *buff           ���ջ�����
// ����˵��     len             ��ȡ���ݳ���
// ���ز���     uint32          ʵ�ʶ�ȡ���ݳ���
// ʹ��ʾ��     ble6a20_read_buffer(buff, 32);
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
uint32 ble6a20_read_buffer (uint8 *buff, uint32 len)
{
    zf_assert(NULL != buff);
    uint32 data_len = len;
    fifo_read_buffer(&ble6a20_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ת����ģ�� �����жϻص�����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     ble6a20_callback();
// ��ע��Ϣ     �ú����� ISR �ļ� �����жϳ��򱻵���
//              �ɴ����жϷ��������� wireless_module_uart_handler() ����
//              ���� wireless_module_uart_handler() �������ñ�����
//-------------------------------------------------------------------------------------------------------------------
void ble6a20_callback (void)
{
    uart_query_byte(BLE6A20_INDEX, &ble6a20_data);
    fifo_write_buffer(&ble6a20_fifo, &ble6a20_data, 1);
#if BLE6A20_AUTO_BAUD_RATE                                                // �����Զ�������
    if(BLE6A20_AUTO_BAUD_RATE_START == ble6a20_auto_baud_flag && 3 == fifo_used(&ble6a20_fifo))
    {
        uint32 ble6a20_auto_baud_count = 3;
        ble6a20_auto_baud_flag = BLE6A20_AUTO_BAUD_RATE_GET_ACK;
        fifo_read_buffer(&ble6a20_fifo, (uint8 *)ble6a20_auto_baud_data, (uint32 *)&ble6a20_auto_baud_count, FIFO_READ_AND_CLEAN);
    }
#endif
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ת����ģ�� ��ʼ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     ble6a20_init();
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
uint8 ble6a20_init (void)
{
    uint8 return_state = 0;

    // �ȴ�ģ���ʼ��
    system_delay_ms(50);

    set_wireless_type(BLE6A20, ble6a20_callback);

    fifo_init(&ble6a20_fifo, FIFO_DATA_8BIT, ble6a20_buffer, BLE6A20_BUFFER_SIZE);
    gpio_init(BLE6A20_RTS_PIN, GPI, GPIO_HIGH, GPI_PULL_UP);
#if(0 == BLE6A20_AUTO_BAUD_RATE)                                          // �ر��Զ�������
    // ������ʹ�õĲ�����Ϊ115200 Ϊ����ת����ģ���Ĭ�ϲ����� ������������������������ģ�鲢�޸Ĵ��ڵĲ�����
    uart_init (BLE6A20_INDEX, BLE6A20_BUAD_RATE, BLE6A20_RX_PIN, BLE6A20_TX_PIN);   // ��ʼ������
    uart_rx_interrupt(BLE6A20_INDEX, 1);
#elif(1 == BLE6A20_AUTO_BAUD_RATE)                                        // �����Զ�������
    uint8 rts_init_status = 0;

    ble6a20_auto_baud_flag = BLE6A20_AUTO_BAUD_RATE_INIT;
    ble6a20_auto_baud_data[0] = 0;
    ble6a20_auto_baud_data[1] = 1;
    ble6a20_auto_baud_data[2] = 3;

    rts_init_status = gpio_get_level(BLE6A20_RTS_PIN);
    gpio_init(BLE6A20_RTS_PIN, GPO, rts_init_status, GPO_PUSH_PULL);      // ��ʼ����������

    uart_init (BLE6A20_INDEX, BLE6A20_BUAD_RATE, BLE6A20_RX_PIN, BLE6A20_TX_PIN);   // ��ʼ������
    uart_rx_interrupt(BLE6A20_INDEX, 1);

    system_delay_ms(5);                                                         // ģ���ϵ�֮����Ҫ��ʱ�ȴ�
    gpio_set_level(BLE6A20_RTS_PIN, !rts_init_status);                    // RTS�������ߣ������Զ�������ģʽ
    system_delay_ms(100);                                                       // RTS����֮�������ʱ20ms
    gpio_toggle_level(BLE6A20_RTS_PIN);                                   // RTS����ȡ��

    do
    {
        ble6a20_auto_baud_flag = BLE6A20_AUTO_BAUD_RATE_START;
        uart_write_byte(BLE6A20_INDEX, ble6a20_auto_baud_data[0]);       // �����ض����� ����ģ���Զ��жϲ�����
        uart_write_byte(BLE6A20_INDEX, ble6a20_auto_baud_data[1]);       // �����ض����� ����ģ���Զ��жϲ�����
        uart_write_byte(BLE6A20_INDEX, ble6a20_auto_baud_data[2]);       // �����ض����� ����ģ���Զ��жϲ�����
        system_delay_ms(20);

        if(BLE6A20_AUTO_BAUD_RATE_GET_ACK != ble6a20_auto_baud_flag)     // �����Զ��������Ƿ����
        {
            return_state = 1;                                                   // ���������뵽������� ˵���Զ�������ʧ����
            break;
        }

        if( 0xa5 != ble6a20_auto_baud_data[0] &&                               // �����Զ��������Ƿ���ȷ
            0xff != ble6a20_auto_baud_data[1] &&                               // �����Զ��������Ƿ���ȷ
            0xff != ble6a20_auto_baud_data[2] )                                // �����Զ��������Ƿ���ȷ
        {
            return_state = 1;                                                   // ���������뵽������� ˵���Զ�������ʧ����
            break;
        }
        ble6a20_auto_baud_flag = BLE6A20_AUTO_BAUD_RATE_SUCCESS;

        gpio_init(BLE6A20_RTS_PIN, GPI, 0, GPI_PULL_UP);                        // ��ʼ����������
        system_delay_ms(10);                                                    // ��ʱ�ȴ� ģ��׼������
    }while(0);
#endif
    return return_state;
}
