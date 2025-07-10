#ifndef _DEVICE_H
#define _DEVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "ci130x_timer.h"
#include "user_config.h"
#include "device_key.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"
#include "ci_nvdata_manage.h"


#define IR_DEVICE_ALL  //open all device

//#define DEBUG
#ifdef DEBUG
#define Errprint(x) (mprintf("err: func:%s   line:%d\n",__FUNCTION__, __LINE__))
#else
#define Errprint(x)
#endif

#define DEVICE_SUPPORT_TV
#define DEVICE_SUPPORT_FAN
#define DEVICE_SUPPORT_LIGHT



#define XTIMERDEVICE

#define DEVICE_GET_IR_WITH_PLAY //可以在播报的时候接收红外

#define DEVICE_TIME (1*1000)   //Timer Tick 1S

typedef struct
{
    int keyNum;         //统计设备包含的按键个数
    int learnKeyIndex;  //学习按键的时候，学习到哪里了？
    int cmdKeyNum;      //命令词按键对应表个数
    char power;         //设备电源状态
    char iMatchFailCnt; //匹配次数
    char bPlay; //是否播报


    //为播报准备，客户用的
    unsigned int cmd_handle;//临时存储
    unsigned int semantic_id;//临时存储
}stDevicePrivateData;

typedef struct
{
    unsigned int cmd_handle;
    unsigned int semantic_id;
    unsigned int device_id;
    char bNeedWakeUp;
    char bPlay;
}stAsrInfo;

typedef enum _eDeviceMsgType
{
    TYPE_DEVICEMSG_LEARN_IR,            //来自语音数据的处理
    TYPE_DEVICEMSG_LEARN_KEY,           //timer来的数据，接收到的IR压缩后的数据(学码)
    TYPE_DEVICEMSG_AIR_MATCH,           //timer来的数据，接收到的IR压缩后的数据(匹配)
    TYPE_DEVICEMSG_SOFTTIM,             //软timer,1S超时，计时心跳统计
    TYPE_DEVICEMSG_SEND_KEY,            //连续发送按键的msg
    TYPE_DEVICEMSG_UART,                //连续发送按键的msg
    TYPE_DEVICEMSG_AIRC_CHECK_CALLBACK, //空调动作能否执行检测回调
    TYPE_DEVICEMSG_AIRC_SEND_CALLBACK,  //空调数据发送后回信息
    TYPE_DEVICEMSG_LEARN_SEND_CALLBACK, //学码数据发送回调信息
    TYPE_DEVICEMSG_LEARN_KEY_UART,      //UART串口学码
    TYPE_DEVICEMSG_MAX,
} eDeviceMsgType;

typedef enum
{
    E_DEVICE_MODE_NORMAL,//无效模式
    E_DEVICE_MODE_LEARN,//学习模式
}eDevice_Mode;

/*每个设备接口*/
typedef struct _stDevice_S
{
    int device_index;//设备ID   自动赋值
    eDevice_Mode ir_mode;

    int (*Init)(struct _stDevice_S* pdevice);   //初始化相应设备
    int (*Deinit)(struct _stDevice_S* pdevice); //去初始化相应设备
    int (*GetAsrId)(struct _stDevice_S* pDevice, uint32_t cmd_handle, int* device_index); //查找设备的ASR索引
    int (*PlayId)(struct _stDevice_S* pdevice, stAsrInfo* pAsrInfo);//播放相应处理
    int (*LearnIR)(struct _stDevice_S* pdevice, stKey* key, eDeviceMsgType type, void* data);//学习IR按键
    int (*ContinueSendIR)(struct _stDevice_S* pdevice, stKey* key, eDeviceMsgType type, void* data);//连续发码接口
    int (*UartCallBack)(struct _stDevice_S* pdevice, unsigned char* data);   //串口回调函数
    int (*CheckKey)(struct _stDevice_S* pdevice); //检查IR按键是否有存储红外码
    int (*CleanIrData)(void);//清码

    void *CommonDevice;
    void *PrivateData;

    struct _stDevice_S* NextDevice;//指向下一个设备
}stDevice;

typedef struct
{
	int index;
	stKey* pKey;//最大1个
}AsrCmd_Key_TypeDef;


int DeviceInit(void);
int DeviceDealASRMsg(stAsrInfo* pDeviceMsg);
int DeviceFindAsr(uint32_t cmd_handle, int * device_id);
int DeviceSetDeviceLearnMode(stDevice *device);
int DeviceClearnAllLearnMode(void);
int device_common_sendmsg(eDeviceMsgType msgtyp, stDevice *pDevice, stKey* pKey, void* data);
int common_send_progNumIR(int iProgNum, stKey** pKeyGrup, int GrupLen);
void DeviceTaskProcess(void*pvParameters);
#ifdef XTIMERDEVICE
extern xTimerHandle xTimerDevice;
extern QueueHandle_t device_queue;
void DeviceTimerCallback( TimerHandle_t xTimer );
void DeviceTimerStop(void);
void DeviceTimerStart(void);
#endif
stDevice* findDevicebyID(int device_id);
stDevice* DeviceGetLearnModeDevice(void);
stDevice *getFirstDevice(void);

SemaphoreHandle_t getDeviceMutex(void);

#define DEVICE_LOCK()   do{ \
    xSemaphoreTake(getDeviceMutex(), portMAX_DELAY); \
}while(0);

#define DEVICE_UNLOCK() do{ \
    xSemaphoreGive(getDeviceMutex()); \
}while(0);

#endif
