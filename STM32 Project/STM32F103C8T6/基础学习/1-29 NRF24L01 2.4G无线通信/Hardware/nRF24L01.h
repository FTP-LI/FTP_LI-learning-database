#ifndef __nRF24L01_H
#define __nRF24L01_H

#include "NRF24L01_Reg.h"
#define uchar unsigned char
#define uint unsigned int
uchar SPI_RW(uchar byte);//��д�Ĵ�����ַ
uchar NRF24L01_Write_Reg(uchar reg,uchar value);//��24L01�ļĴ���дֵ��һ���ֽڣ�
uchar NRF24L01_Read_Reg(uchar reg);//��24L01�ļĴ���ֵ ��һ���ֽڣ�
uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len);//��24L01�ļĴ���ֵ������ֽڣ�
uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len);//��24L01�ļĴ���дֵ������ֽڣ�
uchar NRF24L01_RxPacket(uchar *rxbuf);//24L01��������
uchar NRF24L01_TxPacket(uchar *txbuf);//24L01��������
uchar NRF24L01_Check(void);//���24L01�Ƿ����
void SEND_BUF(uchar *buf);//�������ݰ�
void NRF24L01_Init(void);//��ʼ��

#endif
