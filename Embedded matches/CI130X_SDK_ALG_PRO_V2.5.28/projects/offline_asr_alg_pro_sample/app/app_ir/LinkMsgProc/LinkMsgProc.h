/*struct linkMsgStruct      //链路层协议
	{
		unsigned char head;			//恒定为0XA5
		char buf[1];					//应用层协议，变长，最大值没限制，自行定义MAX_PACK_SIZE
		unsigned char	bcc;				//BCC校验
		unsigned char tail;				//恒定为0XA4
		unsigned char end;				//恒定为0X00
	};

注意：当消息体内容buf内含有A5，或者A4字节时，链路层将自动将该字节增一位，以此区别出包头包尾和内容。
例如： 应用层协议为
	0XA5 0X46 0XA4时，
从链路层封包后将变为
	0XA5 0XA5 0XA5 0X46 0XA4 0XA4 bcc 0XA4 0X00
由于链路层会针对特殊应用数据（包头包尾）进行二次封装，
所以链路层数据区需要比应用层数据区长，在资源允许的情况下建议是2倍的应用层协议长度 + 4（包头 + BCC校验 + 包尾 + 结束符）
例如，应用协议64字节，
则链路层缓冲区最好定义为64 * 2 + 4 = 132字节，若资源不允许，则需要考虑应用数据有没有可能出现有A5或者A4的情况适当调整。
*/


#ifndef __LINKMSGPROC_H__
#define __LINKMSGPROC_H__

#include <stdbool.h>
#include <stdint.h>
#include "ir_data.h"
#ifdef USE_UART_COMMUNIT

#ifdef __cplusplus
 extern "C" {
#endif


#define MAX_PACK_SIZE (2048)

enum
{
	COMHEAD = 0xA5, 			// 包头
	COMTAIL = 0xA4 				//包尾
};
enum
{
	COM_ANA_IDLE = 0,
	COM_ANA_HEAD = 1,
	COM_ANA_HH = 2,
	COM_ANA_TT = 3,
	COM_ANA_NEXT_H = 4
};

//解码上下文
typedef struct DeCodeInfoStruct
{
	uint8_t status;  //解码状态机
	uint32_t bcc;    //校验
	uint32_t inLen;   //长度
}DeCodeInfo;

enum
{
	LINK_DECODE_RET_ERROR = -1,
	LINK_DECODE_RET_NULL = 0,

};


//该函数作用是将应用协议打包为链路协议
//返回编码后的真实长度,0表示失败
//appBuf 编码前缓冲区
//appSize 编码前协议长度
//linkBuf 编码后协议缓冲区
//linkMaxSize 编码后缓冲区最大长度
uint32_t LinkMsgCode(uint8_t * appBuf,uint16_t appSize,uint8_t * linkBuf,uint16_t linkMaxSize) ;


//该函数作用是检测是否有一个完整的链路包
//返回若大于0，表示已经有一个完整的数据包，并返回该数据包的长度，等于0，表示包还没有收齐
//ucData 接收数据字节
//linkBuf 链路数据区指针
//linkMaxSize 链路数据区最大长度
uint32_t  LinkMsgDeCode(uint8_t * ucData,uint8_t * linkBuf,uint16_t linkMaxSize,	DeCodeInfo * deCodeInfo);


//该函数作用从一个完整的链路层包里面截取应用消息
//返回若大于0，表示已经有一个完整的数据包，并返回该数据包的长度，等于0，表示失败
uint8_t LinkMsgGetAppMsg(uint8_t * linkMsg, uint16_t linkMsgSize, uint8_t * appBuf, uint16_t appBufSize);

#ifdef __cplusplus
}
#endif
#endif
#endif

