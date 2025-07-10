#include "LinkMsgProc.h"
#include <string.h>
#include "ci_log.h"

#ifdef USE_UART_COMMUNIT

uint32_t LinkMsgCode(uint8_t *sourceBuf, uint16_t sourceSize, uint8_t *desBuf, uint16_t desMaxSize)
{
	uint32_t i = 0;
	uint32_t dwTailLen = 0;
	uint32_t dwOutLen = 0;
	uint32_t ucBcc = 0;
	uint32_t ucTemp;
	if (sourceBuf == 0 || sourceSize == 0 || desBuf == 0 || desMaxSize == 0)
	{
		return 0;
	}
	memset(desBuf, 0, desMaxSize);
	i = 0;
	dwTailLen = 0;
	dwOutLen = 0;
	ucBcc = 0;

	if (sourceSize < desMaxSize)
	{
		ucBcc = 0;

		desBuf[0] = COMHEAD; //A5

		dwOutLen++;
		for (i = 0; i < sourceSize; i++)
		{
			ucTemp = (unsigned char)sourceBuf[i];

			ucBcc ^= ucTemp;
			desBuf[dwOutLen] = ucTemp;
			dwOutLen++;
			if ((dwOutLen + 1) >= MAX_PACK_SIZE)
			{
				break;
			}
			else if ((ucTemp == COMHEAD) || (ucTemp == COMTAIL))
			{
				desBuf[dwOutLen] = ucTemp;
				dwOutLen++;
				if ((dwOutLen + 1) >= MAX_PACK_SIZE)
				{
					break;
				}
			}
		}

		if ((ucBcc == COMHEAD) || (ucBcc == COMTAIL))
		{
			dwTailLen = 1;
		}

		if ((dwOutLen + 3 + dwTailLen) < MAX_PACK_SIZE)
		{
			desBuf[dwOutLen] = ucBcc;
			dwOutLen++;
			if ((ucBcc == COMHEAD) || (ucBcc == COMTAIL))
			{
				desBuf[dwOutLen] = ucBcc;
				dwOutLen++;
			}
			desBuf[dwOutLen] = COMTAIL;
			dwOutLen++;
			desBuf[dwOutLen] = 0x00;
			dwOutLen++;
		}
	}
	return dwOutLen;
}

uint32_t LinkMsgDeCode(uint8_t *ucData, uint8_t *msgBuf, uint16_t bufMaxSize, DeCodeInfo *deCodeInfo)
{
	uint32_t deCodeRet = LINK_DECODE_RET_NULL;
	uint8_t *status = 0;
	uint32_t *bcc = 0;
	uint32_t *inLen = 0;
	if (bufMaxSize == 0 || msgBuf == 0 || deCodeInfo == 0)
	{
		return LINK_DECODE_RET_ERROR;
	}

	status = &(deCodeInfo->status);
	bcc = &(deCodeInfo->bcc);
	inLen = &(deCodeInfo->inLen);

	switch (*status)
	{

	case COM_ANA_IDLE:
		if (COMHEAD == *ucData)
		{
			*status = COM_ANA_HEAD;
			*bcc = 0;
			*inLen = 1;
			msgBuf[0] = COMHEAD;
		}
		break;
	case COM_ANA_NEXT_H:
		*status = COM_ANA_HEAD;
		*bcc = 0;
		*inLen = 1;
		msgBuf[0] = COMHEAD;
		break;
	case COM_ANA_HEAD:
		if (COMHEAD == *ucData)
		{
			*status = COM_ANA_HH;
		}
		else if (COMTAIL == *ucData)
		{
			*status = COM_ANA_TT;
		}
		else
		{
			msgBuf[*inLen] = *ucData;
			(*inLen)++;
			*bcc ^= *ucData;
		}
		break;
	case COM_ANA_HH:
		if (COMHEAD == *ucData)
		{
			msgBuf[*inLen] = *ucData;
			(*inLen)++;
			*bcc ^= *ucData;
			*status = COM_ANA_HEAD;
		}
		else if (COMTAIL == *ucData)
		{
			*status = COM_ANA_TT;
		}
		else
		{
			*status = COM_ANA_HEAD;
		}
		break;
	case COM_ANA_TT:
		if (COMTAIL == *ucData)
		{
			msgBuf[*inLen] = *ucData;
			(*inLen)++;
			*bcc ^= *ucData;
			*status = COM_ANA_HEAD;
		}
		else if ((0x00 == *ucData) || (COMHEAD == *ucData))
		{
			if (*bcc == 0)
			{
				if (COMHEAD == *ucData)
				{
					msgBuf[*inLen] = COMTAIL;
					(*inLen)++;
					deCodeRet = *inLen;
				}
				else
				{
					msgBuf[*inLen] = COMTAIL;
					(*inLen)++;
					msgBuf[*inLen] = 0x00;
					(*inLen)++;
					deCodeRet = *inLen;
				}
			}
			else
			{
				mprintf("------uart data check error1!!!-----\n");
				//校验不过，丢包了
				deCodeRet = LINK_DECODE_RET_ERROR;
			}
			if (COMHEAD == *ucData)
			{
				*status = COM_ANA_NEXT_H;
			}
			else
			{
				*status = COM_ANA_IDLE;
			}
		}
		break;
	default:
		break;
	}

	if (*inLen >= bufMaxSize)
	{
		mprintf("------uart data size error2!!!-----\n");
		deCodeRet = LINK_DECODE_RET_ERROR;
		*status = COM_ANA_IDLE;
		*inLen = 0;
	}
	return deCodeRet;
}

uint8_t LinkMsgGetAppMsg(uint8_t *linkMsg, uint16_t linkMsgSize, uint8_t *appBuf, uint16_t appBufSize)
{
	if (linkMsg == 0 || appBuf == 0 || appBufSize < linkMsgSize - 4)
	{
		return 0;
	}
	//包头，BCC校验，包尾，结束符共4个字节
	memcpy(appBuf, linkMsg + 1, linkMsgSize - 3);
	return linkMsgSize - 4;
}

#endif
