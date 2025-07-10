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
* �ļ�����          zf_device_wireless_ble6a20.h
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


#ifndef _zf_device_ble6a20_h_
#define _zf_device_ble6a20_h_

#include "zf_common_typedef.h"

#define BLE6A20_INDEX        UART_7                                      // �������ڶ�Ӧʹ�õĴ��ں�
#define BLE6A20_BUAD_RATE    115200                                      // �������ڶ�Ӧʹ�õĴ��ڲ�����,
                                                                         // ģ�鲨���ʽ�֧�� 9600 57600 115200 230400 460800 921600 1500000 2000000
#define BLE6A20_TX_PIN       UART7_MAP3_RX_E13                           // �������ڶ�Ӧģ��� TX Ҫ�ӵ���Ƭ���� RX
#define BLE6A20_RX_PIN       UART7_MAP3_TX_E12                           // �������ڶ�Ӧģ��� RX Ҫ�ӵ���Ƭ���� TX
#define BLE6A20_RTS_PIN      E8                                          // �������ڶ�Ӧģ��� RTS ����


// ------------------------------------ �Զ������� ------------------------------------
// ע������1�������Զ��������������RTS���� ����Ὺ��ʧ�ܡ�
// ע������2��ģ���Զ�������ʧ�ܵĻ� ���Գ��Զϵ�����

// �����Զ�����������Ķ��������� ע������
// �����Զ�����������Ķ��������� ע������
// �����Զ�����������Ķ��������� ע������

// 0: �ر��Զ�������
// 1: �����Զ������� �Զ������ʵ��������޸� BLE6A20_BAUD ֮����Ҫ��ģ��������� ģ����Զ�����Ϊ��Ӧ�Ĳ�����
// ģ�鲨���ʽ�֧�� 9600 57600 115200 230400 460800 921600 1500000 2000000


#define BLE6A20_AUTO_BAUD_RATE    ( 0 )

#if (1 == BLE6A20_AUTO_BAUD_RATE)
typedef enum
{
    BLE6A20_AUTO_BAUD_RATE_SUCCESS,
    BLE6A20_AUTO_BAUD_RATE_INIT,
    BLE6A20_AUTO_BAUD_RATE_START,
    BLE6A20_AUTO_BAUD_RATE_GET_ACK,
}ble6a20_auto_baudrate_state_enum;
#endif
// ------------------------------------ �Զ������� ------------------------------------

#define BLE6A20_BUFFER_SIZE       ( 64   )
#define BLE6A20_TIMEOUT_COUNT     ( 0x64 )

uint32      ble6a20_send_byte         (const uint8 data);
uint32      ble6a20_send_buffer       (const uint8 *buff, uint32 len);
uint32      ble6a20_send_string       (const char *str);
void        ble6a20_send_image        (const uint8 *image_addr, uint32 image_size);

uint32      ble6a20_read_buffer       (uint8 *buff, uint32 len);

void        ble6a20_callback          (void);

uint8       ble6a20_init              (void);

#endif
