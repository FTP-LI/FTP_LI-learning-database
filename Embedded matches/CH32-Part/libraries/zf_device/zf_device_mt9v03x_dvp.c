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
* �ļ�����          zf_device_mt9v03x
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MounRiver Studio V1.8.1
* ����ƽ̨          CH32V307VCT6
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����                                      ����                             ��ע
* 2021-12-23        ��W            ����ͷ�ɼ���ɱ�־λ����volatile����
* 2022-03-26        ��W            �޸Ĳ��ֲ���Ҫ��ע��
* 2022-09-15        ��W            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   TXD                 �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_COF_UART_TX        �궨��
*                   RXD                 �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_COF_UART_RX        �궨��
*                   D0                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D0_PIN             �궨��
*                   D1                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D1_PIN             �궨��
*                   D2                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D2_PIN             �궨��
*                   D3                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D3_PIN             �궨��
*                   D4                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D4_PIN             �궨��
*                   D5                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D5_PIN             �궨��
*                   D6                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D6_PIN             �궨��
*                   D7                  �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_D7_PIN             �궨��
*                   PCLK                �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_PCLK_PIN           �궨��
*                   VSYNC               �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_VSY_PIN            �궨��
*                   HSYNC               �鿴 zf_device_mt9v03x_dvp.h �� MT9V03X_HERF_PIN           �궨��
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_driver_delay.h"
#include "zf_driver_dvp.h"
#include "zf_driver_soft_iic.h"


#include "zf_device_camera.h"
#include "zf_device_type.h"
#include "zf_device_mt9v03x_dvp.h"
#include "zf_device_config.h"

vuint8 mt9v03x_finish_flag = 0;                                                 // һ��ͼ��ɼ���ɱ�־λ
__attribute__((aligned(4))) uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

static fifo_struct          *camera_receiver_fifo;
static mt9v03x_type_enum    mt9v03x_type = MT9V03X_SCCB;
static uint16               mt9v03x_version = 0x00;

//-------------------------------------------------------------------------------------------------------------------
// �������     ��������ͷ�ڲ�������Ϣ
// ����˵��     buff            ����������Ϣ�ĵ�ַscc8660_sccb_set_config
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_set_config(mt9v03x_set_confing_buffer);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 mt9v03x_set_config (const int16 buff[MT9V03X_CONFIG_FINISH][2])
{
    uint8   uart_buffer[4];
    uint8   return_state        = 1;
    uint16  temp_value          = 0;
    uint16  timeout_count       = 0;
    uint32  loop_count          = 0;
    uint32  uart_buffer_index   = 0;

    switch(mt9v03x_version)
    {
        case 0x0230:    loop_count = MT9V03X_PCLK_MODE;  break;
        default:        loop_count = MT9V03X_GAIN;       break;
    }
    // ���ò���  ������ο���������ֲ�
    // ��ʼ��������ͷ�����³�ʼ��
    for(; loop_count < MT9V03X_SET_DATA; loop_count --)
    {
        uart_buffer[0]  = 0xA5;
        uart_buffer[1]  = buff[loop_count][0];
        temp_value      = buff[loop_count][1];
        uart_buffer[2]  = temp_value >> 8;
        uart_buffer[3]  = (uint8)temp_value;
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        system_delay_ms(2);
    }

    do
    {
        if(3 <= fifo_used(camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            if((0xff == uart_buffer[1]) || (0xff == uart_buffer[2]))
            {
                return_state = 0;
                break;
            }
        }
        system_delay_ms(1);
    }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
    // ���ϲ��ֶ�����ͷ���õ�����ȫ�����ᱣ��������ͷ��51��Ƭ����eeprom��
    // ����set_exposure_time�����������õ��ع����ݲ��洢��eeprom��
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ����ͷ�ڲ�������Ϣ
// ����˵��     buff            ����������Ϣ�ĵ�ַ
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_get_config(mt9v03x_get_confing_buffer);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 mt9v03x_get_config (int16 buff[MT9V03X_CONFIG_FINISH - 1][2])
{
    int8    loop_count          = 0;
    uint8   uart_buffer[4];
    uint8   return_state        = 0;
    uint16  temp_value          = 0;
    uint16  timeout_count       = 0;
    uint32  uart_buffer_index   = 0;

    switch(mt9v03x_version)
    {
        case 0x0230:    loop_count = MT9V03X_PCLK_MODE;  break;
        default:        loop_count = MT9V03X_GAIN;       break;
    }

    for(loop_count = loop_count - 1; 0 <= loop_count; loop_count --)
    {
        if((0x0230 > mt9v03x_version) && (MT9V03X_PCLK_MODE == buff[loop_count][0]))
        {
            continue;
        }
        uart_buffer[0]  = 0xA5;
        uart_buffer[1]  = MT9V03X_GET_STATUS;
        temp_value      = buff[loop_count][0];
        uart_buffer[2]  = temp_value >> 8;
        uart_buffer[3]  = (uint8)temp_value;
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        timeout_count = 0;
        do
        {
            if(3 <= fifo_used(camera_receiver_fifo))
            {
                uart_buffer_index = 3;
                fifo_read_buffer(camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                buff[loop_count][1] = uart_buffer[1] << 8 | uart_buffer[2];
                break;
            }
            system_delay_ms(1);
        }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
        if(MT9V03X_INIT_TIMEOUT < timeout_count)                                // ��ʱ
        {
            return_state = 1;
            break;
        }
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     MT9V03X UART ��ȡ����ͷ ID
// ����˵��     void
// ���ز���     uint8           0-�ɹ� x-ʧ��
// ʹ��ʾ��     mt9v03x_check_id();
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 mt9v03x_uart_check_id (void)
{
    uint8   uart_buffer[4] = {0};
    uint8   return_state        = 1;
    uint16  timeout_count       = 0;
    uint32  uart_buffer_index   = 0;


    if(fifo_used(camera_receiver_fifo))
    {
        fifo_clear(camera_receiver_fifo);
    }

    uart_write_byte(MT9V03X_COF_UART, 0xFF);
    uart_buffer[0] = 0xA5;
    uart_buffer[1] = MT9V03X_COLOR_GET_WHO_AM_I;
    uart_buffer[2] = 0;
    uart_buffer[3] = 0;
    uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

    timeout_count = 0;
    do
    {
        if(3 <= fifo_used(camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            return_state = !(0x01 == uart_buffer[2] || 0x02 == uart_buffer[2]);
            break;
        }
        system_delay_ms(1);
    }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     MT9V03X ͨ�Ŵ��ڻص�����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     mt9v03x_uart_handler();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
static void mt9v03x_uart_handler (void)
{
    uint8 data = 0;
    uart_query_byte(MT9V03X_COF_UART, &data);
    if(NULL != camera_receiver_fifo)
    {
        if(0xA5 == data)
        {
            fifo_clear(camera_receiver_fifo);
        }

        fifo_write_element(camera_receiver_fifo, data);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     MT9V03X����ͷ���ж�
// ����˵��     NULL
// ���ز���     void
//  @since      v1.0
// ʹ��ʾ��     ��isr.c�����ȴ�����Ӧ���жϺ�����Ȼ����øú���(֮�����������жϱ�־λ)
//-------------------------------------------------------------------------------------------------------------------
void mt9v03x_dvp_handler(void)
{
    //�Ѿ��޸�Ϊѭ���ɼ�ģʽ������Ҫ���õ�ַ�Ϳ���DVP
    //DVP->DMA_BUF0 = (uint32)camera_buffer_addr;       // �ָ�DMA��ַ
    mt9v03x_finish_flag = 1;                        // ����ͷ�ɼ���ɱ�־λ��1
    //�Ѿ��޸�Ϊѭ���ɼ�ģʽ������Ҫ���õ�ַ�Ϳ���DVP
    //DVP->CR0 |= RB_ENABLE;                          // ���´�DVP���ɼ���һ��ͼ��
}




//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ����ͷ�̼��汾
// ����˵��     void
// ���ز���     uint16          0-��ȡ���� N-�汾��
// ʹ��ʾ��     mt9v03x_get_version();                                          // ���øú���ǰ���ȳ�ʼ������
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
uint16 mt9v03x_get_version (void)
{
    uint8   uart_buffer[4];
    uint16  timeout_count       = 0;
    uint16  return_value        = 0;
    uint32  uart_buffer_index   = 0;

    do
    {
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = MT9V03X_GET_STATUS;
        uart_buffer[2] = (uint8)((MT9V03X_GET_VERSION >> 8) & 0xFF);
        uart_buffer[3] = (uint8)(MT9V03X_GET_VERSION & 0xFF);
        uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

        do
        {
            if(3 <= fifo_used(camera_receiver_fifo))
            {
                uart_buffer_index = 3;
                fifo_read_buffer(camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                return_value = uart_buffer[1] << 8 | uart_buffer[2];
                break;
            }
            system_delay_ms(1);
        }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
    }while(0);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������������ͷ�ع�ʱ��
// ����˵��     light           �趨�ع�ʱ��
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_set_exposure_time(100);                 // ���øú���ǰ���ȳ�ʼ������
// ��ע��Ϣ     �����ع�ʱ��Խ��ͼ��Խ��
//              ����ͷ�յ������ݷֱ��ʼ�FPS��������ع�ʱ��������õ����ݹ���
//              ��ô����ͷ��������������ֵ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_set_exposure_time (uint16 light)
{
    uint8   uart_buffer[4];
    uint8   *receiver_buffer    = NULL;
    uint8   return_state        = 0;
    uint16  temp_value          = 0;
    uint16  timeout_count       = 0;
    uint32  uart_buffer_index   = 0;

    if(MT9V03X_UART == mt9v03x_type)
    {
        do
        {
            receiver_buffer = (uint8 *)malloc(MT9V03X_COF_BUFFER_SIZE);
            if(NULL == receiver_buffer)
            {
                zf_log(0, "FIFO buffer malloc error.");
                return_state = 1;
                break;
            }
            camera_receiver_fifo = (fifo_struct *)malloc(sizeof(fifo_struct));
            if(NULL == camera_receiver_fifo)
            {
                zf_log(0, "FIFO malloc error.");
                return_state = 1;
                break;
            }
            fifo_init(camera_receiver_fifo, FIFO_DATA_8BIT, receiver_buffer, MT9V03X_COF_BUFFER_SIZE);

            uart_buffer[0]  = 0xA5;
            uart_buffer[1]  = MT9V03X_SET_EXP_TIME;
            temp_value      = light;
            uart_buffer[2]  = temp_value >> 8;
            uart_buffer[3]  = (uint8)temp_value;
            uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

            do
            {
                if(3 <= fifo_used(camera_receiver_fifo))
                {
                    uart_buffer_index = 3;
                    fifo_read_buffer(camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                    temp_value = uart_buffer[1] << 8 | uart_buffer[2];
                    break;
                }
                system_delay_ms(1);
            }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
            if((temp_value != light) || (MT9V03X_INIT_TIMEOUT <= timeout_count))
            {
                return_state = 1;
            }
        }while(0);
    }
    else
    {
        return_state = mt9v03x_sccb_set_exposure_time(light);
    }

    free(receiver_buffer);
    free(camera_receiver_fifo);
    camera_receiver_fifo = NULL;
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ͷ�ڲ��Ĵ�������д����
// ����˵��     addr            ����ͷ�ڲ��Ĵ�����ַ
// ����˵��     data            ��Ҫд�������
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     mt9v03x_set_reg(addr, data);                    // ���øú���ǰ���ȳ�ʼ������
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_set_reg (uint8 addr, uint16 data)
{
    uint8   uart_buffer[4];
    uint8   *receiver_buffer    = NULL;
    uint8   return_state        = 0;
    uint16  temp_value          = 0;
    uint16  timeout_count       = 0;
    uint32  uart_buffer_index   = 0;

    if(MT9V03X_UART == mt9v03x_type)
    {
        do
        {
            receiver_buffer = (uint8 *)malloc(MT9V03X_COF_BUFFER_SIZE);
            if(NULL == receiver_buffer)
            {
                zf_log(0, "FIFO buffer malloc error.");
                return_state = 1;
                break;
            }
            camera_receiver_fifo = (fifo_struct *)malloc(sizeof(fifo_struct));
            if(NULL == camera_receiver_fifo)
            {
                zf_log(0, "FIFO malloc error.");
                return_state = 1;
                break;
            }
            fifo_init(camera_receiver_fifo, FIFO_DATA_8BIT, receiver_buffer, MT9V03X_COF_BUFFER_SIZE);

            uart_buffer[0]  = 0xA5;
            uart_buffer[1]  = MT9V03X_SET_ADDR;
            temp_value      = addr;
            uart_buffer[2]  = temp_value >> 8;
            uart_buffer[3]  = (uint8)temp_value;
            uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

            system_delay_ms(10);
            uart_buffer[0]  = 0xA5;
            uart_buffer[1]  = MT9V03X_SET_DATA;
            temp_value      = data;
            uart_buffer[2]  = temp_value >> 8;
            uart_buffer[3]  = (uint8)temp_value;
            uart_write_buffer(MT9V03X_COF_UART, uart_buffer, 4);

            do
            {
                if(3 <= fifo_used(camera_receiver_fifo))
                {
                    uart_buffer_index = 3;
                    fifo_read_buffer(camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                    temp_value = uart_buffer[1] << 8 | uart_buffer[2];
                    break;
                }
                system_delay_ms(1);
            }while(MT9V03X_INIT_TIMEOUT > timeout_count ++);
            if((temp_value != data) || (MT9V03X_INIT_TIMEOUT <= timeout_count))
            {
                return_state = 1;
            }
        }while(0);
    }
    else
    {
        return_state = mt9v03x_sccb_set_reg(addr, data);
    }

    free(receiver_buffer);
    free(camera_receiver_fifo);
    camera_receiver_fifo = NULL;
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     MT9V03X SCCB ��ʼ��
// ����˵��     void
// ���ز���     uint8           0-�ɹ� x-ʧ��
// ʹ��ʾ��     mt9v03x_sccb_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_sccb_init (void)
{
    uint8 return_state  = 1;

    mt9v03x_type = MT9V03X_SCCB;
    soft_iic_info_struct mt9v03x_iic_struct;
    soft_iic_init(
        &mt9v03x_iic_struct, 0,
        MT9V03X_COF_IIC_DELAY,
        MT9V03X_COF_IIC_SCL,
        MT9V03X_COF_IIC_SDA);

    if(!mt9v03x_sccb_check_id(&mt9v03x_iic_struct))
    {
        // ��Ҫ���õ�����ͷ������ �����������޸Ĳ���
        const int16 mt9v03x_set_confing_buffer[MT9V03X_CONFIG_FINISH][2]=
        {
            {MT9V03X_INIT,              0},                                     // ����ͷ��ʼ��ʼ��

            {MT9V03X_AUTO_EXP,          MT9V03X_AUTO_EXP_DEF},                  // �Զ��ع�����
            {MT9V03X_EXP_TIME,          MT9V03X_EXP_TIME_DEF},                  // �ع�ʱ��
            {MT9V03X_FPS,               MT9V03X_FPS_DEF},                       // ͼ��֡��
            {MT9V03X_SET_COL,           MT9V03X_W},                             // ͼ��������
            {MT9V03X_SET_ROW,           MT9V03X_H},                             // ͼ��������
            {MT9V03X_LR_OFFSET,         MT9V03X_LR_OFFSET_DEF},                 // ͼ������ƫ����
            {MT9V03X_UD_OFFSET,         MT9V03X_UD_OFFSET_DEF},                 // ͼ������ƫ����
            {MT9V03X_GAIN,              MT9V03X_GAIN_DEF},                      // ͼ������
            {MT9V03X_PCLK_MODE,         MT9V03X_PCLK_MODE_DEF},                 // ����ʱ��ģʽ
        };
        return_state = mt9v03x_sccb_set_config(mt9v03x_set_confing_buffer);
    }

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     MT9V03X UART ��ʼ��
// ����˵��     void
// ���ز���     uint8           0-�ɹ� x-ʧ��
// ʹ��ʾ��     mt9v03x_uart_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_uart_init (void)
{
    uint8 return_state  = 1;
    uint8 loop_count    = 0;
    uint8 *receiver_buffer  = NULL;

    mt9v03x_type = MT9V03X_UART;
    do
    {
        receiver_buffer = (uint8 *)malloc(MT9V03X_COF_BUFFER_SIZE);             // ���뻺����
        if(NULL == receiver_buffer)                                             // �ѿռ䲻��
        {
            zf_log(0, "FIFO buffer malloc error.");                             // �޷���ȡ���ݻ�������ʾ
            return_state = 2;                                                   // �쳣��
            break;
        }
        camera_receiver_fifo = (fifo_struct *)malloc(sizeof(fifo_struct));      // ���� FIFO ����
        if(NULL == camera_receiver_fifo)                                        // �ѿռ䲻��
        {
            zf_log(0, "FIFO malloc error.");                                    // �޷���ȡ FIFO ������ʾ
            free(receiver_buffer);
            return_state = 2;                                                   // �쳣��
            break;
        }
        fifo_init(camera_receiver_fifo, FIFO_DATA_8BIT, receiver_buffer, MT9V03X_COF_BUFFER_SIZE);



        // ��ʼ������ ��������ͷ
        uart_init(MT9V03X_COF_UART, MT9V03X_COF_BAUR, MT9V03X_COF_UART_RX, MT9V03X_COF_UART_TX);
        uart_rx_interrupt(MT9V03X_COF_UART, 1);

        mt9v03x_version = mt9v03x_get_version();

        if(mt9v03x_version != 0)
        {
            if(mt9v03x_uart_check_id())
            {
                uart_rx_interrupt(MT9V03X_COF_UART, 0);
                return_state = 3;
                break;
            }
        }


        // ��Ҫ���õ�����ͷ������ �����������޸Ĳ���
        const int16 mt9v03x_set_confing_buffer[MT9V03X_CONFIG_FINISH][2]=
        {
            {MT9V03X_INIT,              0},                                     // ����ͷ��ʼ��ʼ��

            {MT9V03X_AUTO_EXP,          MT9V03X_AUTO_EXP_DEF},                  // �Զ��ع�����
            {MT9V03X_EXP_TIME,          MT9V03X_EXP_TIME_DEF},                  // �ع�ʱ��
            {MT9V03X_FPS,               MT9V03X_FPS_DEF},                       // ͼ��֡��
            {MT9V03X_SET_COL,           MT9V03X_W},                             // ͼ��������
            {MT9V03X_SET_ROW,           MT9V03X_H},                             // ͼ��������
            {MT9V03X_LR_OFFSET,         MT9V03X_LR_OFFSET_DEF},                 // ͼ������ƫ����
            {MT9V03X_UD_OFFSET,         MT9V03X_UD_OFFSET_DEF},                 // ͼ������ƫ����
            {MT9V03X_GAIN,              MT9V03X_GAIN_DEF},                      // ͼ������
            {MT9V03X_PCLK_MODE,         MT9V03X_PCLK_MODE_DEF},                 // ����ʱ��ģʽ
        };

        // ������ͷ�ڲ���ȡ������������ �����������޸Ĳ���
        int16 mt9v03x_get_confing_buffer[MT9V03X_CONFIG_FINISH - 1][2]=
        {
            {MT9V03X_AUTO_EXP,          0},                                     // �Զ��ع�����
            {MT9V03X_EXP_TIME,          0},                                     // �ع�ʱ��
            {MT9V03X_FPS,               0},                                     // ͼ��֡��
            {MT9V03X_SET_COL,           0},                                     // ͼ��������
            {MT9V03X_SET_ROW,           0},                                     // ͼ��������
            {MT9V03X_LR_OFFSET,         0},                                     // ͼ������ƫ����
            {MT9V03X_UD_OFFSET,         0},                                     // ͼ������ƫ����
            {MT9V03X_GAIN,              0},                                     // ͼ������
            {MT9V03X_PCLK_MODE,         0},                                     // ����ʱ��ģʽ���� PCLKģʽ < ������� MT9V034 V1.5 �Լ����ϰ汾֧�ָ����� >
        };

        if(mt9v03x_set_config(mt9v03x_set_confing_buffer))                      // �������������
        {
            // �������������˶�����Ϣ ������ʾ����λ��������
            // ��ô���Ǵ���ͨ�ų�����ʱ�˳���
            // ���һ�½�����û������ ���û������ܾ��ǻ���
            zf_log(0, "MT9V03X set config error.");
            uart_rx_interrupt(MT9V03X_COF_UART, 0);
            return_state = 1;
            break;
        }

        // ��ȡ���ñ��ڲ鿴�����Ƿ���ȷ
        if(mt9v03x_get_config(mt9v03x_get_confing_buffer))
        {
            // �������������˶�����Ϣ ������ʾ����λ��������
            // ��ô���Ǵ���ͨ�ų�����ʱ�˳���
            // ���һ�½�����û������ ���û������ܾ��ǻ���
            zf_log(0, "MT9V03X get config error.");
            uart_rx_interrupt(MT9V03X_COF_UART, 0);
            return_state = 1;
            break;
        }

        for(loop_count = 0; MT9V03X_CONFIG_FINISH - 1 > loop_count; loop_count ++)
        {
            if( mt9v03x_set_confing_buffer[loop_count + 1][0] != mt9v03x_get_confing_buffer[loop_count][0] ||
                mt9v03x_set_confing_buffer[loop_count + 1][1] != mt9v03x_get_confing_buffer[loop_count][1])
            {
                break;
            }
        }
        return_state = (MT9V03X_CONFIG_FINISH - 2 <= loop_count) ? 0 : 1;
    }while(0);

    free(receiver_buffer);
    free(camera_receiver_fifo);
    camera_receiver_fifo = NULL;

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     MT9V03X ����ͷ��ʼ��
// ����˵��     void
// ���ز���     uint8           0-�ɹ� x-ʧ��
// ʹ��ʾ��     mt9v03x_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 mt9v03x_init (void)
{

    uint8 return_state  = 0;

    do
    {
        set_camera_type(CAMERA_GRAYSCALE, NULL, mt9v03x_dvp_handler);
        if(mt9v03x_sccb_init())
        {
            set_camera_type(CAMERA_GRAYSCALE, mt9v03x_uart_handler, mt9v03x_dvp_handler);
            return_state = mt9v03x_uart_init();
        }

        if(!return_state)
        {
           // DVP���ų�ʼ��
            dvp_gpio_init(
                MT9V03X_D0_PIN, MT9V03X_D1_PIN, MT9V03X_D2_PIN, MT9V03X_D3_PIN,
                MT9V03X_D4_PIN, MT9V03X_D5_PIN, MT9V03X_D6_PIN, MT9V03X_D7_PIN,
                MT9V03X_PCLK_PIN, MT9V03X_VSY_PIN, MT9V03X_HERF_PIN);

            // DVP�ӿڳ�ʼ��
            dvp_camera_init((uint32 *)&mt9v03x_image[0], (uint32 *)&mt9v03x_image[0], MT9V03X_W*MT9V03X_H, MT9V03X_H);
            break;
        }
        set_camera_type(NO_CAMERE, NULL, NULL);
    }while(0);

    return return_state;
}
