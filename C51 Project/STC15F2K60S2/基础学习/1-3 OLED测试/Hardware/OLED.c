#include "OLED.h"
#include "OLED_Font.h"
#include "Delay.h"

//延时，处理IIC，ACK应答位
void IIC_Delay(void)
{
	uint8_t D_t=1;
	while(D_t--);
}

//IIC Start
void IIC_Start()
{
   OLED_SCL_High;		
   OLED_SDA_High;
	 IIC_Delay();
   OLED_SDA_Low;
	 IIC_Delay();
   OLED_SCL_Low;
}

//IIC Stop
void IIC_Stop()
{
   OLED_SCL_Low;
   OLED_SDA_Low;
	 IIC_Delay();
	 OLED_SCL_High;		
   OLED_SDA_High;
}

//等待信号响应
uint8_t I2C_WaitAck(void) //测数据信号的电平
{
	uint8_t Ack_Sta;
	OLED_SDA_High;
	IIC_Delay();
	OLED_SCL_High;
	IIC_Delay();
	if(OLED_SDA)
	{
		Ack_Sta = 1;
	}
	else
	{
		Ack_Sta = 0;
	}
	OLED_SCL_Low;
	IIC_Delay();
	return Ack_Sta;
}

// 通过I2C总线写一个字节
uint8_t Write_IIC_Byte(uint8_t IIC_Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		if(IIC_Byte & 0x80)
		{
			OLED_SDA_High;
		}
		else
		{
			OLED_SDA_Low;
		}
		IIC_Delay();
		OLED_SCL_High;
		IIC_Delay();
		OLED_SCL_Low;
		IIC_Byte<<=1;
	}
  return I2C_WaitAck();
}

//OLED写数据
void WriteDat(uint8_t IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);
	Write_IIC_Byte(0x40);			//write data
	Write_IIC_Byte(IIC_Data);
	IIC_Stop();
}

//OLED写命令
void WriteCmd(uint8_t IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);            //Slave address,SA0=0
	Write_IIC_Byte(0x00);			//write command
	Write_IIC_Byte(IIC_Command);
	IIC_Stop();
}

void OLED_Init(void)//初始化
{
	OLED_RES_Low;
	delay_ms(200);//上电延迟
	OLED_RES_High;
    
	WriteCmd(0xAE); //关闭显示
	
	WriteCmd(0x20);	//Set Memory Addressing Mode  //设置内存水平寻址方式	
	WriteCmd(0x10);	//00,水平的寻址模式;01,垂直的寻址模式;10,页寻址 (RESET);11,无效的
	
	WriteCmd(0xB0);	//Set Page Start Address for Page Addressing Mode,0-7  //“页面寻址方式”选择“页面起始地址”
	
	WriteCmd(0xC8);	//Set COM Output Scan Direction C0 //正常模式(RESET)扫描从COM0到COM[N -1] C8 正常模式(RESET)扫描从COM[N -1]到COM0
	
	WriteCmd(0x00); //---set low column address//设置页面寻址模式的下列起始低位地址
	WriteCmd(0x10); //---set high column address//设置页面寻址模式的下列起始高位位地址
	
	WriteCmd(0x40); //--set start line address //设置显示RAM显示起始行寄存器
	
	WriteCmd(0x81); //--set contrast control register//设置对比度，也就是亮度
	WriteCmd(0xCF); //亮度调节 0x00~0xff
	
	WriteCmd(0xA1); //--set segment re-map 0 to 127 //A0 列地址0映射到SEG0 A1列地址127映射为SEG0
	
	WriteCmd(0xA6); //--set normal display  //A6 0灭1亮 A7 1灭0亮
	
	WriteCmd(0xA8); //--set multiplex ratio(1 to 64) //MUX配比设置为N+1 MUX
	WriteCmd(0x3F); //1/64duty  //设置要刷的COM数 16-64有效
	
	WriteCmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content //A4 输出RAM内容 A5 忽略RAM内容
	
	WriteCmd(0xD3); //-set display offset //设置垂直移动由COM从0d~63dreset后该值被重置为00h
	WriteCmd(0x00); //-not offset
	
	WriteCmd(0xD5); //--set display clock divide ratio/oscillator frequency//设置显示时钟分频/振荡器频率
	WriteCmd(0xF0); //--set divide ratio
	
	WriteCmd(0xD9); //--set pre-charge period//设置pre-charge周期
	WriteCmd(0x22); //  设置周期
	
	WriteCmd(0xDA); //--set com pins hardware configuration  设置COM引脚硬件配置 
	WriteCmd(0x12);  //备用COM引脚配置 禁用COM左/右重映射
	
	WriteCmd(0xDB); //--set vcomh //设置VCOMH取消勾选“级别”
	WriteCmd(0x20); //0x20,0.77xVcc
	
	WriteCmd(0x8D); //--set DC-DC enable//设置直流电源
	WriteCmd(0x14); //
	
	WriteCmd(0xAF); //--turn on oled panel  //打开屏幕显示
	OLED_Fill(0x00); //初始清屏
} 


//OLED 光标设置
void OLED_SetPos(uint8_t x, uint8_t y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);//“页面寻址方式”选择“页面起始地址”
	WriteCmd(((x&0xf0)>>4)|0x10);  //设置页面寻址模式的下列起始高位地址
	WriteCmd((x&0x0f)|0x01);   //设置页面寻址模式的下列起始低位地址
}

//OLED全屏填充
void OLED_Fill(uint8_t fill_Data)
{
	uint8_t m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);
		WriteCmd(0x00);		
		WriteCmd(0x10);		
		for(n=0;n<X_WIDTH;n++) 
			{
				WriteDat(fill_Data);
				
			}
	}
}
//OLED清屏
void OLED_Clear(void)
{
	OLED_Fill(0x00);
}

//将OLED从休眠中唤醒

void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}

//让OLED休眠
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}

void OLED_ShowString(uint8_t x, uint8_t y, uint8_t ch[])//(x:0~127, y:0~7)
{
	uint8_t c = 0,i = 0,j = 0;
	while(ch[j] != '\0')
        {
			c = ch[j] - 32;
			if(x > 120)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x,y);
			for(i=0;i<8;i++)
				WriteDat(F8X16[c*16+i]);
			OLED_SetPos(x,y+1);
			for(i=0;i<8;i++)
				WriteDat(F8X16[c*16+i+8]);
			x += 8;
			j++;
		}
}


void OLED_ShowHalfString(uint8_t x, uint8_t y, uint8_t ch[])
{
	uint8_t c = 0,i = 0,j = 0;
	while(ch[j] != '\0')
		{
			c = ch[j] - 32;
			if(x > 126)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x,y);
			for(i=0;i<6;i++)
				WriteDat(F6x8[c][i]);
			x += 6;
			j++;
		}
}

void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t N)//(x:0~127, y:0~7) N为对应字库汉字
{
	uint8_t wm=0;
	uint16_t  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// 函数      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// 参数说明     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t BMP[])
{
	uint16_t j=0;
	uint8_t x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}
//OLED图片切换显示测试
void OLED_DrawBMP_Test(void)
{
	
		OLED_Fill(0x00); //屏全灭

		OLED_DrawBMP(0,0,128,8,BMP1);

		delay_ms(3000);
		OLED_Fill(0x00); //屏全灭
		
		OLED_DrawBMP(20,0,101,8,BMP2);  //图片显示(图片显示慎用，生成的字表较大，会占用较多空间，FLASH空间8K以下慎用)
		delay_ms(3000);
		
		OLED_Fill(0x00); //屏全灭
		
		OLED_DrawBMP(20,0,114,8,BMP3);  //图片显示(图片显示慎用，生成的字表较大，会占用较多空间，FLASH空间8K以下慎用)
		delay_ms(3000);
}


///**
// * 功能   		    OLED水平滚动显示
// * 参数说明   		start_page  : PAGE0~PAGE7
// * 参数说明       end_page  :  PAGE0~PAGE7    (end_page必须大于start_page)
// * 参数说明       frame  : 0~7
// * 参数说明      	dir  : 0 右滚   1 左滚
// * @retval      	无
// */
//void OLED_Scrolling_Display(uint8_t start_page, uint8_t end_page,uint8_t frame,uint8_t dir)
//{
//    WriteCmd(0x2E);	/* 停止滚动 调用后,RAM数据需要重新写入 */
//    
//    WriteCmd(dir ? 0x26 : 0x27);	/* 0,向右滚动 1,向左移动 */
//    WriteCmd(0x00);            		/* 发送空字节 设置为0x00即可 */
//    WriteCmd(start_page & 0x07);	/* 起始页地址 */
//    WriteCmd(frame & 0x07); 		/* 设置滚动步长时间间隔,帧为单位 */
//    WriteCmd(end_page & 0x07);	/* 终止页地址 */

//    WriteCmd(0x00);   /* 发送空字节 设置为0x00即可 */
//    WriteCmd(0xFF);   /* 发送空字节 设置为0xFF即可 */
//    WriteCmd(0x2F);   /* 启动滚动 禁止对RAM访问,改变水平滚动配置 */
//}

