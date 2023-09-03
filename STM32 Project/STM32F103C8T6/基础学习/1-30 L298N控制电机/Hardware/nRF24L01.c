#include "stm32f10x.h"                  // Device header
#include "NRF24L01_Reg.h"
#include "NRF24L01.h"
#include "sys.h"
#include "Delay.h"

//	while(1)
//	{
//        SEND_BUF(A);
//        Delay_ms(2000);
//        SEND_BUF(B);
//        Delay_ms(2000);
//        SEND_BUF(C);
//        Delay_ms(2000);
//	}//����ʹ�÷���

//    while(1)
//	{
//		
//	  if( PBin(1) == 0) //�����⵽��ʼ��������
//	  {
//		NRF24L01_RxPacket(C); //�����ݷ�������C
//	  }
//	  
//	    OLED_ShowHexNum(2,5,C[0],2);
//	}//����ʹ�÷���


#define TX_ADR_WIDTH    5                                        //5�ֽڵ�ַ���
#define RX_ADR_WIDTH    5                                        //5�ֽڵ�ַ���
#define TX_PLOAD_WIDTH  32                                       //32�ֽ���Ч���ݿ��
#define RX_PLOAD_WIDTH  32                                       //32�ֽ���Ч���ݿ��

const uchar TX_ADDRESS[TX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //���͵�ַ
const uchar RX_ADDRESS[RX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //���͵�ַ


#define NRF_CSN   PAout(4)      //Ƭѡ�źţ��������  ��		                        ��PA4
#define NRF_SCK   PAout(5)      //���ͽ���ģʽѡ���������,                             ��PA5
#define NRF_MISO  PAin(6)       //PC7 MISO  					                        ��PA6
#define NRF_MOSI  PAout(7)      //PC6 MOSI ����������� ���������ʳ�ʼ��PC_CR2�Ĵ�����, ��PA7
#define NRF_CE    PBout(0)      //PC5 �������,				                            ��PB0
#define NRF_IRQ   PBin(1)       //IRQ������������,��������		                        ��PB1
//ʹ�����SPI����ͨ���޸Ĵ˺궨��Ͷ˿ڳ�ʼ����������

void NRF24L01_Init(void)//��ʼ��
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  			//IRQ�ж�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    //���ų�ʼ��
    
    NRF_CE=0;		  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);                          //ѡ��ͨ��0����Ч���ݿ��
	NRF24L01_Write_Reg(FLUSH_RX,0xff);									                //���RX FIFO�Ĵ���,0xff��û�õ�  
  	NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR,(uchar*)TX_ADDRESS,TX_ADR_WIDTH);          //дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(nRF_WRITE_REG+RX_ADDR_P0,(uchar*)RX_ADDRESS,RX_ADR_WIDTH);       //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_AA,0x01);                                       //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(nRF_WRITE_REG+EN_RXADDR,0x01);                                   //ʹ��ͨ��0�Ľ��յ�ַ  
  	NRF24L01_Write_Reg(nRF_WRITE_REG+SETUP_RETR,0x1a);                                  //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_CH,0);                                          //����RFͨ��Ϊ2.400GHz  Ƶ��=2.4+0GHz
  	NRF24L01_Write_Reg(nRF_WRITE_REG+RF_SETUP,0x0F);                                    //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0f);                                      //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF_CE=1;									                                        //CE�øߣ�ʹ�ܷ���
    //���ó�ʼ��
}

uchar SPI_RW(uchar byte)  //��д�Ĵ�����ַ
{
	uchar bit_ctr;
	for(bit_ctr=0;bit_ctr<8;bit_ctr++)                                                  // ���8λ
	{                                          
		if((uchar)(byte&0x80)==0x80)                                           
		NRF_MOSI=1; 			                                                        // MSB TO MOSI����λ����
		else                                           
			NRF_MOSI=0;                                            
		byte=(byte<<1);					                                                // ��λ����
		NRF_SCK=1;						                                                //�ߵ�ƽʱ�໥��������
		byte|=NRF_MISO;	        		                                                // ��¼nrf24l01���������ֽ�
		NRF_SCK=0;						                                                //����ʱ��
	}
	return byte;
}

uchar NRF24L01_Write_Reg(uchar reg,uchar value)//��24L01�ļĴ���дֵ��һ���ֽڣ�,reg   Ҫд�ļĴ�����ַ,value ���Ĵ���д��ֵ
{
	uchar status;

	NRF_CSN=0;                                 //CSN=0;   
  	status = SPI_RW(reg);		               //���ͼĴ�����ַ,����ȡ״ֵ̬
	SPI_RW(value);				               //��д�Ĵ�������дֵ
	NRF_CSN=1;                                 //CSN=1;

	return status;//����״ֵ̬
}

uchar NRF24L01_Read_Reg(uchar reg)//��24L01�ļĴ���ֵ ��һ���ֽڣ���reg  Ҫ���ļĴ�����ַ��value �����Ĵ�����ֵ
{
 	uchar value;

	NRF_CSN=0;                                  //CSN=0;   
  	SPI_RW(reg);			                    //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	value = SPI_RW(NOP);                    
	NRF_CSN=1;             	                    //CSN=1;

	return value;//����value �����Ĵ�����ֵ
}

uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len)//��24L01�ļĴ���ֵ������ֽڣ���reg   �Ĵ�����ַ��*pBuf �����Ĵ���ֵ�Ĵ�����飬len   �����ֽڳ���
{
	uchar status,u8_ctr;
	NRF_CSN=0;                   	            //CSN=0       
	status=SPI_RW(reg);				            //���ͼĴ�����ַ,����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
	pBuf[u8_ctr]=SPI_RW(0XFF);		            //��������
	NRF_CSN=1;                 		            //CSN=1
	return status;        			            //���ض�����״ֵ̬
}

uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len)//��24L01�ļĴ���дֵ������ֽڣ���reg  Ҫд�ļĴ�����ַ��*pBuf ֵ�Ĵ�����飬len   �����ֽڳ���  
{
	uchar status,u8_ctr;
	NRF_CSN=0;   					            //CSN�͵�ƽд
	status = SPI_RW(reg);			            //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
	SPI_RW(*pBuf++); 				            //д������
	NRF_CSN=1;
  return status;          		                //���ض�����״ֵ̬
}							  					   

uchar NRF24L01_RxPacket(uchar *rxbuf)//24L01��������
{
	uchar state;
	state=NRF24L01_Read_Reg(STATUS);  			            //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state);         //0x20+STATUS��ָ��״̬�Ĵ�����state��Ϊ�ˣ������־λ��1��ִ�иó���ͻ�����ж�
	if(state&RX_OK)								            //RX_OK��0x40���������жϽ���
	{
		NRF_CE = 0;											//����CEʹ�����öˣ�CEΪ�͵�ƽ�����õĲ����á���CSN��ʹ��д���ݶˣ��͵�ƽ����д����
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ���ݣ���������ݲ�������CE,ֻ�з��Ͳ�Ҫ
		NRF24L01_Write_Reg(FLUSH_RX,0xff);					//���RX FIFO�Ĵ���
		NRF_CE = 1;					
		Delay_us(150);
		return 0; //�ɹ��յ����� 
	}	   
	return 1;//û�յ��κ�����
}

uchar NRF24L01_TxPacket(uchar *txbuf)//24L01�������ݣ�txbuf  ������������
{
	uchar state;
   
	NRF_CE=0;												            //CE���ͣ�ʹ��24L01���ã���ʱ�ſ���дTX��TX���������棩
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	            //д���ݵ�TX BUF  32���ֽ�
 	NRF_CE=1;												            //CE�øߣ�ʹ�ܷ��ͣ�����Ϊ����
	while(NRF_IRQ==1);										            //�ȴ��������
	state=NRF24L01_Read_Reg(STATUS);  						            //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(nRF_WRITE_REG+STATUS,state); 			        //���TX_DS��MAX_RT�жϱ�־�������Ǹ�λ��0
	if(state&MAX_TX)				 						            //����ﵽ����ط�����
	{       
		NRF24L01_Write_Reg(FLUSH_TX,0xff);					            //���TX FIFO�Ĵ�����ֱ��������ݣ���Ҫ��
		return MAX_TX;                                                  //�ﵽ����ط�����������ʧ��
	}       
	if(state&TX_OK)											            //�������
	{       
		return TX_OK;       
	}       
	return 0xff;											            //����ʧ��
}
  
uchar NRF24L01_Check(void)//���24L01�Ƿ����
{
	uchar check_in_buf[5]={0x11,0x22,0x33,0x44,0x55};
	uchar check_out_buf[5]={0x00};

	NRF_SCK=0;
	NRF_CSN=1;    //��д����
	NRF_CE=0;	  //ʹ������

	NRF24L01_Write_Buf(nRF_WRITE_REG+TX_ADDR, check_in_buf, 5);	//дTX

	NRF24L01_Read_Buf(nRF_READ_REG+TX_ADDR, check_out_buf, 5);	//��TX

	if((check_out_buf[0] == 0x11)&&\
	   (check_out_buf[1] == 0x22)&&\
	   (check_out_buf[2] == 0x33)&&\
	   (check_out_buf[3] == 0x44)&&\
	   (check_out_buf[4] == 0x55))return 0;//���� 
	else return 1;//������
}			

void SEND_BUF(uchar *buf)//�������ݰ�
{
	NRF_CE=0;										        //д�Ĵ���ʱ��Ҫ��CE����
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG,0x0e);   
	NRF_CE=1;
	Delay_us(15);
	NRF24L01_TxPacket(buf);						
	NRF_CE=0;
	NRF24L01_Write_Reg(nRF_WRITE_REG+CONFIG, 0x0f);		    //ת��Ϊ����ģʽ
	NRF_CE=1;	
}
