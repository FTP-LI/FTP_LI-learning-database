#ifndef _DEVICE_KEY_H
#define _DEVICE_KEY_H

#include <stdint.h>
#include "ir_data.h"


typedef enum
{
    KEY_MODE_NORMAL,
    KEY_MODE_LEARN,
}E_KeyMode;

typedef enum
{
    FLAG_NO_STORE,
    FLAG_STORE_LEARN,
    FLAG_STORE_DEFAULT_NEC,
}E_StoreFlag;

/*按键接口*/
typedef struct _stKey_S
{
    uint32_t iplay_befor_normal;//学习前的播报,正常播报
    uint32_t iplay_get_irdata;//学习成功获取IR播报

	unsigned char Nec[4];

    /*当前模式 学键模式/正常模式*/
    E_KeyMode keymode;
    unsigned short key_id; //红外码ID  ir_data_t.key_id 暂时不用， 从 0xb0000000 - 0xb0010000

    /*按键相关数据*/
    //ir_data_t *key_data;

    /*按键相关接口*/
    int (*initKey)(struct _stKey_S* pKey); //初始化IR按键
    int (*storeIR)(struct _stKey_S* pKey, ir_data_t* pIr_Key);//存储IR按键
    int (*deleteIR)(struct _stKey_S* pKey);//存储IR按键
    int (*sendIR)(struct _stKey_S* pKey);//发送IR按键
    E_StoreFlag (*checkIRLearn)(struct _stKey_S* pKey);//判断是否学习

    int (*playVoice)(struct _stKey_S* pKey, int iplay_id);//播报方法某一个ID

}stKey;

typedef struct
{
    stKey* key;
    int jumpCnt;
}stKeyJump;

int init_key_fun_parm(struct _stKey_S* pKey, int iKeyNum);
int init_keydata(stKey* key, int keylen);

int key_unlinkKey(stKey** pKeyLink);
int key_clean_allkey_mode(stKey* pKey, int keylen);
int key_clean_IrCodeData(stKey* pKey, int keylen);
int checkAllLearnKey(stKey* pKey, int keylen);

stKey* key_get_learnkey(stKey* pKey, int keylen);

#endif

