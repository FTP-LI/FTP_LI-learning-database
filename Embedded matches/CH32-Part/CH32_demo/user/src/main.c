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
* �ļ�����          main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MounRiver Studio V1.8.1
* ����ƽ̨          CH32V307VCT6
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����                                      ����                             ��ע
* 2022-09-15        ��W            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"


// *************************** ����Ӳ������˵�� ***************************
// ���İ��������缴�� �����������


// *************************** ���̲���˵�� ***************************
//�������ⲿ����ʶ��Ϊ7��ʶ��һ��


// **************************** �������� ****************************
#define LED1                    (E2 )

#define KEY1                    (C9 )                                           // ʹ�õ��ⲿ�ж��������� ����޸� ��Ҫͬ����Ӧ�޸��ⲿ�жϱ���� isr.c �еĵ���

#define KEY1_EXTI               (EXTI9_5_IRQn)                                  // ��Ӧ�ⲿ�жϵ��жϱ��

#define UART_INDEX              (DEBUG_UART_INDEX   )                           // Ĭ�� UART_3
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)                           // Ĭ�� 115200
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )                           // Ĭ�� UART3_MAP0_TX_B10
#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )                           // Ĭ�� UART3_MAP0_RX_B11

#define UART_PRIORITY           (USART3_IRQn)                                   // ��Ӧ�����жϵ��жϱ�� �� ch32v30x.h ͷ�ļ��в鿴 IRQn_Type ö����

uint8 exti_state[1];

uint8       uart_get_data[64];                                                  // ���ڽ������ݻ�����
uint8       fifo_get_data[64];                                                  // fifo �������������

uint8       get_data = 0;                                                       // �������ݱ���
uint32      fifo_data_count = 0;                                                // fifo ���ݸ���

fifo_struct uart_data_fifo;

uint8 Send_buf[10] = {0} ;

void SendCmd(uint8 len)
{

    uart_write_byte(UART_INDEX,0x7E); //��ʼ

	uart_write_buffer(UART_INDEX,Send_buf,len); //��������

    uart_write_byte(UART_INDEX,0xEF) ;//����
}

void DoSum( uint8 *Str, uint8 len)
{
    uint16 xorsum = 0;
    uint8 i;

    for(i=0; i<len; i++)
    {
        xorsum  = xorsum + Str[i];
    }
	xorsum     = 0 -xorsum;
	*(Str+i)   = (uint8)(xorsum >>8);
	*(Str+i+1) = (uint8)(xorsum & 0x00ff);
}

void Uart_SendCMD(uint8 CMD ,uint8 feedback , uint16 dat)
{
    Send_buf[0] = 0xff;    //�����ֽ� 
    Send_buf[1] = 0x06;    //����
    Send_buf[2] = CMD;     //����ָ��
    Send_buf[3] = feedback;//�Ƿ���Ҫ����
    Send_buf[4] = (uint8)(dat >> 8);//datah
    Send_buf[5] = (uint8)(dat);     //datal
    DoSum(&Send_buf[0],6);        //У��
    SendCmd(8);       //���ʹ�֡����
}


int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // ��ʼ��оƬʱ�� ����Ƶ��Ϊ 120MHz
    debug_init();                       // ��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���

    // �˴���д�û����� ���������ʼ�������

    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // ��ʼ�� LED1 ��� Ĭ�ϸߵ�ƽ �������ģʽ

    exti_init(KEY1, EXTI_TRIGGER_RISING);                                       // ��ʼ�� KEY1 Ϊ�ⲿ�ж����� �����ش���

    interrupt_set_priority(KEY1_EXTI, (0<<5) || 1);                             // ���� KEY1 ��Ӧ�ⲿ�жϵ��ж���ռ���ȼ�0�������ȼ�1

    //------------------------------------------------------------------------------------------------------------//
    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // ��ʼ�� fifo ���ػ�����

    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // ��ʼ��������ģ�������� �������������ģʽ
    uart_rx_interrupt(UART_INDEX, ZF_ENABLE);                                   // ���� UART_INDEX �Ľ����ж�
    interrupt_set_priority(UART_PRIORITY, (0<<5) || 1);                         // ���ö�Ӧ UART_INDEX ���ж���ռ���ȼ�0�������ȼ�1
    system_delay_ms(2000);
    exti_state[0] = 0; 
    //------------------------------------------------------------------------------------------------------------//
    // �˴���д�û����� ���������ʼ�������
    while(1)//����״̬��
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        if(gpio_get_level(KEY1) == 1)
        {
            exti_state[0] = 1;                                                      // �ⲿ�жϴ��� ��־λ��λ
        }

        if(exti_state[0] == 1)//��һ��Ƶ�׶�
        {
            Uart_SendCMD(0x03,0,0x02);//���ŵ�һ��Ƶ
            exti_state[0] = 2;
            system_delay_ms(10000);
            if(gpio_get_level(KEY1) == 1)
            {
                exti_state[0] = 1;                                                      // �ⲿ�жϴ��� ��־λ��λ
            } 
        }
        else if(exti_state[0] == 2)//�ڶ���Ƶ�׶�
        {
            Uart_SendCMD(0x03,0,0x01);
            system_delay_ms(1000);                                           // ��ʱ
            exti_state[0] = 0;                                                  // ����ⲿ�жϴ�����־λ
        }
        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     KEY1 �İ����ⲿ�жϴ����� ����������� KEY1 ���Ŷ�Ӧ���ⲿ�жϵ��� ��� isr.c
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     key1_exti_handler();
//-------------------------------------------------------------------------------------------------------------------
void key1_exti_handler (void)
{
    // �����ش����������жϵ�ʱ��GPIO״̬�Ǹߵ�ƽ��
    if(gpio_get_level(KEY1) == 1)
    {
        exti_state[0] = 1;                                                      // �ⲿ�жϴ��� ��־λ��λ
    }

}

//-------------------------------------------------------------------------------------------------------------------
// �������     UART_INDEX �Ľ����жϴ����� ����������� UART_INDEX ��Ӧ���жϵ��� ��� isr.c
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler (void)
{
//    get_data = uart_read_byte(UART_INDEX);                                      // �������� while �ȴ�ʽ ���������ж�ʹ��
    uart_query_byte(UART_INDEX, &get_data);                                     // �������� ��ѯʽ �����ݻ᷵�� TRUE û�����ݻ᷵�� FALSE
    fifo_write_buffer(&uart_data_fifo, &get_data, 1);                           // ������д�� fifo ��
}

// **************************** �������� ****************************
