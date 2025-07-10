#include "device_key.h"
#include "ir_data.h"
#include "common_api.h"
#include "device.h"

/*
初始化IR按键
pIr_Key:从SPI FLASH中读取的按键信息
*/
static unsigned int ui_key_store_id_start = 0xb0000000;
static unsigned short i16KeyCnt = 0;
static ir_data_t KeyData = {0};

//存储IR按键
//bStore: 是否立即存储 1：立即存储，0 不立即存储，需要后面自己调用nv_data_store_all
static int StoreIR(struct _stKey_S* pKey ,ir_data_t* pIr_Key)
{
    if ((NULL == pKey) || (NULL == pIr_Key))
    {
        goto out;
    }

    if(CINV_ITEM_UNINIT == cinv_item_init(pKey->key_id, sizeof(ir_data_t), pIr_Key))
    {
        mprintf("key_id:%d first w size:%d\n", pKey->key_id, sizeof(ir_data_t));
    }

    if(CINV_OPER_SUCCESS != cinv_item_write(pKey->key_id, sizeof(ir_data_t), pIr_Key))
    {
        mprintf("key_id:%d write error\n", pKey->key_id);
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}


//删除IR按键
//bStore: 是否立即存储 1：立即存储，0 不立即存储，需要后面自己调用nv_data_store_all
static int DeleteIR(struct _stKey_S* pKey )
{
    if (NULL == pKey)
    {
        goto out;
    }

    cinv_item_delete(pKey->key_id);

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}


//学习的播报
static int PlayVoice(struct _stKey_S* pKey, int iplay_id)
{
    if (NULL == pKey)
    {
        goto out;
    }

    common_PlayVoiceAsrIndex(iplay_id, -1);

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

//发送IR按键
static int SendIR(struct _stKey_S* pKey)
{
    uint16_t real_len = 0;
	E_StoreFlag eStoreStatus = FLAG_NO_STORE;

    if (NULL == pKey)
    {
        goto out;
    }

	eStoreStatus = pKey->checkIRLearn(pKey);
    if (FLAG_NO_STORE == eStoreStatus)
    {
        common_PlayVoiceAsrIndex(MAIN_VOICE_NO_IR_DATA, -1);
        goto out;
    }
    else
    {
    	if (FLAG_STORE_LEARN == eStoreStatus)
    	{
			if((CINV_OPER_SUCCESS != cinv_item_read(pKey->key_id , sizeof(ir_data_t), &KeyData, &real_len)) \
				|| (sizeof(ir_data_t) != real_len))
			{
				goto out;
			}
			else
			{
				/*发送IR数据*/
				ir_Send_IR(&KeyData);
			}
		}
		else if (FLAG_STORE_DEFAULT_NEC == eStoreStatus)
		{
			send_nec_key(pKey->Nec);
		}

    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static E_StoreFlag CheckIRLearn(struct _stKey_S* pKey)
{
    uint16_t real_len = 0;
	unsigned char Nec[4] = {0};

    if (NULL == pKey)
    {
        goto out;
    }

    if(CINV_OPER_SUCCESS != cinv_item_read(pKey->key_id , sizeof(ir_data_t), &KeyData, &real_len))
    {
        // 判断是否有NEC设置
    	if (0 == memcmp(Nec, pKey->Nec, 4))
    	{
			goto out;
		}

		return FLAG_STORE_DEFAULT_NEC;
    }

    if((0 == KeyData.bits_length) || (KeyData.bits_length > 800))
    {
        goto out;
    }

    return FLAG_STORE_LEARN;

out:
    Errprint();
    return FLAG_NO_STORE;
}

int init_key_fun_parm(struct _stKey_S* pKey, int iKeyNum)
{
    stKey* TempKey = NULL;

    if (NULL == pKey)
    {
        goto out;
    }

    TempKey = pKey;
    while(iKeyNum--)
    {
        if (NULL == TempKey)
        {
            goto out;
        }

        TempKey->keymode = KEY_MODE_NORMAL;
        TempKey->storeIR = StoreIR;
        TempKey->deleteIR = DeleteIR;
        TempKey->playVoice = PlayVoice;
        TempKey->sendIR = SendIR;
        TempKey->checkIRLearn = CheckIRLearn;
        TempKey++;

    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;

}

//clean ircode data
int key_clean_IrCodeData(stKey* pKey, int keylen)
{
    stKey* TempKey = NULL;

    if (NULL == pKey)
    {
        return RETURN_OK;
    }

    for (int i = 0; i < keylen; i++)
    {
        TempKey = pKey + i;

        if (CINV_OPER_SUCCESS != cinv_item_delete(TempKey->key_id))
        {
            goto out;
        }
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

int init_keydata(stKey* key, int keylen)
{
    stKey* Tempkey = NULL;
    int ret = RETURN_ERR;

    if ((0 >= keylen) || (NULL == key))
    {
        goto out;
    }

    for (int i = 0; i < keylen ;i++)
    {
        Tempkey = key + i;
        Tempkey->key_id = ui_key_store_id_start + i16KeyCnt;
        cinv_register_hotid(Tempkey->key_id);
        i16KeyCnt++;
    }

    return RETURN_OK;

out:
    return RETURN_ERR;
}

//clean keychan mode
int key_clean_allkey_mode(stKey* pKey, int keylen)
{
    stKey* TempKey = NULL;

    if (NULL == pKey)
    {
        return RETURN_OK;
    }

    for (int i = 0; i < keylen; i++)
    {
        TempKey = pKey + i;
        TempKey->keymode = KEY_MODE_NORMAL;
    }

    return RETURN_OK;
}

stKey* key_get_learnkey(stKey* pKey, int keylen)
{
    stKey* TempKey = NULL;

    if (NULL == pKey)
    {
        goto out;
    }

    for (int i = 0; i < keylen; i++)
    {
        TempKey = pKey + i;
        if (TempKey->keymode == KEY_MODE_LEARN)
        {
            break;
        }
    }

    return TempKey;

out:
    Errprint();
    return NULL;
}

int checkAllLearnKey(stKey* pKey, int keylen)
{
    int i = 0;
    int iNoLearnNum = 0;
    stKey* TempKey = NULL;

    if (NULL == pKey)
    {
        goto out;
    }

    for (i = 0; i < keylen; i++)
    {
        TempKey = pKey + i;
        if (TempKey->checkIRLearn)
        {
            if (FLAG_NO_STORE == TempKey->checkIRLearn(TempKey))
            {
                iNoLearnNum++;
            }
        }
    }

    //所有按键都没有定义，意思就是没有匹配按键
    if (iNoLearnNum >= keylen)
    {
        goto out;
    }

    return RETURN_OK;
out:
    return RETURN_ERR;
}

