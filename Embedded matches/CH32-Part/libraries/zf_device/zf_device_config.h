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
* �ļ�����          zf_driver_adc
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
#ifndef _zf_device_config_h_
#define _zf_device_config_h_

extern const unsigned char image_frame_header[4];
extern const unsigned char imu660ra_config_file[8192];
extern const unsigned char dl1b_config_file[135];


unsigned char   mt9v03x_sccb_check_id           (void *soft_iic_obj);
unsigned char   mt9v03x_sccb_set_config         (const short int buff[10][2]);
unsigned char   mt9v03x_sccb_set_exposure_time  (unsigned short int light);
unsigned char   mt9v03x_sccb_set_reg            (unsigned char addr, unsigned short int data);

unsigned char   scc8660_sccb_check_id           (void *soft_iic_obj);
unsigned char   scc8660_sccb_set_config         (const short int buff[11][2]);
unsigned char   scc8660_sccb_set_brightness     (unsigned short int brightness);
unsigned char   scc8660_sccb_set_manual_wb      (unsigned short int manual_wb);
unsigned char   scc8660_sccb_set_reg            (unsigned char reg, unsigned short int data);
#endif
