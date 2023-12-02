#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "IIC.h"
#include "TEA5767.h"

unsigned long g_frequency = TEA5767_MIN_KHZ;

static uint8_t s_radioWriteData[5] = {0x31,0xA0,0x20,0x11,0x00};          // Ҫд��TEA5767�����ݣ�Ĭ�ϴ�̨��PLL,104.3MHz
static uint8_t s_radioReadData[5] = {0};                                  // TEA5767������״̬
static uint32_t s_pll = 0;

void TEA5767_Init(void)
{
    IIC_Init();
}

void TEA5767_Write(void)
{
    uint8_t i;
    
    IIC_Start();
    IIC_SendByte(TEA5767_ADDR_W);
    IIC_WaitAck();
    for(i=0;i<5;i++)
    {
        IIC_SendByte(s_radioWriteData[i]);
        IIC_Ack();
    }
    IIC_Stop();
}

void TEA5767_Read(void)
{
    uint8_t i;
    uint8_t tempLow;
    uint8_t tempHigh;
    s_pll = 0;
    
    IIC_Start();
    IIC_SendByte(TEA5767_ADDR_R);
    IIC_WaitAck();
    for(i=0;i<5;i++)
    {
        s_radioReadData[i]=IIC_ReceiveByte();
    }
    IIC_Stop();
    tempHigh = s_radioReadData[0];
    tempLow = s_radioReadData[1];
    tempHigh &= 0x3f;
    s_pll = tempHigh*256 + tempLow;
}

void TEA5767_GetPLL(void)//����PLL
{
    uint8_t HLSI;
    HLSI = s_radioWriteData[2] & 0x10;                                  // HLSIλ
    if(HLSI)
    {
        s_pll = (uint32_t)((float)((g_frequency+225)*4)/(float)32.768);   // Ƶ�ʵ�λ:k
    }
    else
    {
        s_pll = (uint32_t)((float)((g_frequency-225)*4)/(float)32.768);   // Ƶ�ʵ�λ:k
    }
}

uint32_t TEA5767_GetFrequency(void)//��PLL����Ƶ��
{
    uint8_t HLSI;
    uint32_t pll = 0;
    pll = s_pll;
    HLSI = s_radioWriteData[2] & 0x10;
    if(HLSI)
    {
        g_frequency = (unsigned long)((float)(pll)*(float)8.192-225);   // Ƶ�ʵ�λ:KHz
    }
    else
    {
        g_frequency = (unsigned long)((float)(pll)*(float)8.192+225);   // Ƶ�ʵ�λ:KHz
    }
    
    return g_frequency;
}


void TEA5767_SetFrequency(uint32_t frequency)//����Ƶ��
{
    g_frequency = frequency;
    TEA5767_GetPLL();
    s_radioWriteData[0] = s_pll / 256;
    s_radioWriteData[1] = s_pll % 256;
    s_radioWriteData[2] = 0x20;
    s_radioWriteData[3] = 0x11;
    s_radioWriteData[4] = 0x00;
    
    TEA5767_Write();
}

void TEA5767_Search(uint8_t mode)//�ֶ���̨ mode=1��������/mode=0��������
{
    TEA5767_Read();                                                     // ��ȡ��ǰƵ��ֵ 
    if(mode==1)                                                            // ��������
    {
        g_frequency += 100;
        if(g_frequency > TEA5767_MAX_KHZ)                               // Ƶ�ʴﵽ���ֵ
        {
            g_frequency = TEA5767_MIN_KHZ;
        }
    }
    else if(mode==2)
    {
        g_frequency -= 100;
        if(g_frequency < TEA5767_MIN_KHZ)
        {
            g_frequency = TEA5767_MAX_KHZ;
        }
    }          
    TEA5767_GetPLL();                                                   // ����PLLֵ
    s_radioWriteData[0] = s_pll / 256;
    s_radioWriteData[1] = s_pll % 256;
    s_radioWriteData[2] = 0x20;
    s_radioWriteData[3] = 0x11;
    s_radioWriteData[4] = 0x00;
    TEA5767_Write();
    TEA5767_Read();
    if(s_radioReadData[0] & 0x80)										// ��̨�ɹ���RF=1���ɱ�����Ƶ��ֵ����
    {
        
    }    
}

void TEA5767_AutoSearch(uint8_t mode)
{
    uint8_t radioRf = 0;													// 1=����һ����̨��0=δ�ҵ���̨
    uint8_t radioIf = 0;													// ��Ƶ�������
    uint8_t radioLev = 0;													// �źŵ�ƽADC���
	
    // ֱ����̨�ɹ���RF=1,0x31<IF<0x3E
    while((radioRf==0) || ((0x31>=radioIf)||(radioIf>=0x3E)))
    {
        if(mode)														// Ƶ��������̨
        {
            s_radioWriteData[2] = 0xC0;									// SUD=1,SSLadc=7,HLSI=0
            g_frequency += 100;
            if(g_frequency > TEA5767_MAX_KHZ)							// Ƶ�ʴﵽ���ֵ
            {
                g_frequency = TEA5767_MIN_KHZ;
            }
        }
        else															// Ƶ�ʼ�С��̨
        {
            s_radioWriteData[2] = 0x40;									// SUD=0,SSLadc=7,HLSI=0
            g_frequency -= 100;
            if(g_frequency < TEA5767_MIN_KHZ)
            {
                g_frequency = TEA5767_MAX_KHZ;
            }			
        }
        TEA5767_GetPLL();												// ת��ΪPLLֵ
        s_radioWriteData[0] = s_pll / 256 + 0xC0;						// MUTE=1,SM=1
        s_radioWriteData[1] = s_pll % 256;
        s_radioWriteData[3] = 0x11;
        s_radioWriteData[4] = 0x00;
        TEA5767_Write();												// д��5���ֽ�����
        TEA5767_Read();													// ��ȡ��ǰƵ��ֵ 
        radioRf = s_radioReadData[0] & 0x80;
        radioIf = s_radioReadData[2] & 0x7F;
        radioLev = s_radioReadData[3] >> 4;
        radioLev = radioLev;
    }
    TEA5767_GetPLL();                                                   // ת��ΪPLLֵ
    s_radioWriteData[0] = s_pll / 256;									// MUTE=0,SM=0
    s_radioWriteData[1] = s_pll % 256;
    s_radioWriteData[3] = 0x11;
    s_radioWriteData[4] = 0x00;
    TEA5767_Write();                                                    // д��5���ֽ�����
    TEA5767_Read();														// ��ȡ��ǰƵ��ֵ 
}

void TEA5767_Mute(uint8_t mode)//����ģʽ
{		
    if(mode == TEA5767_MUTE_ON)
    {
        s_radioWriteData[0] = s_radioWriteData[0] | 0x80;
    }
    else
    {
        s_radioWriteData[0] = s_radioWriteData[0] & 0x7F;
    }
    
    TEA5767_Write();
}
