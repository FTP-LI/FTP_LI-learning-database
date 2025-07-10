#include "device.h"
#include "ir_data.h"
#include "common_api.h"

static SemaphoreHandle_t DeviceMutex = NULL;
static stDevice *device_all = NULL;//管理全部的设备

/*此数据是用于公共的*/
static stCommonInfo CommonData =
{
    .current_deal_index = -1,
    .device_mode = E_DEVICE_MODE_NORMAL,
    .CommonDevice = NULL,
};

SemaphoreHandle_t getDeviceMutex(void)
{
    return DeviceMutex;
}

stDevice *getFirstDevice(void)
{
    if (NULL == device_all)
    {
        goto out;
    }

    return device_all;

out:
    Errprint();
    return NULL;
}

static int checkDeviceBeforAdd(stDevice *device)
{
    stDevice *device_temp = NULL;

    if (NULL == device)
    {
        goto out;
    }

    device_temp = device_all;
    while(device_temp)
    {
        if (device_temp->device_index == device->device_index)
        {
            goto out;
        }

        device_temp = device_temp->NextDevice;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

static stDevice* findFinalDevice(void)
{
    stDevice *device = NULL;
    if (NULL == device_all)
    {
        goto out;
    }

    device = device_all;

    while(device->NextDevice)
    {
        device = device->NextDevice;
    }

    return device;

out:
    Errprint();
    return NULL;
}

stDevice* findDevicebyID(int device_id)
{
    stDevice *device_temp = NULL;

    if ((NULL == device_all) || (device_id < 0))
    {
        goto out;
    }

    device_temp = device_all;

    while(device_temp)
    {
        if (device_temp->device_index == device_id)
        {
            return device_temp;
        }

        device_temp = device_temp->NextDevice;
    }

out:
    Errprint();
    return NULL;
}

static int AddDevice(stDevice *device)
{
    stDevice *device_temp = NULL;

    if (NULL == device)
    {
        goto out;
    }

    device->device_index = (unsigned int)device;

    if (NULL == device_all)
    {
        device_all = device;
    }
    else
    {
        if (RETURN_ERR == checkDeviceBeforAdd(device))
        {
            goto out;
        }

        device_temp = findFinalDevice();
        if (NULL == device_temp)
        {
            goto out;
        }

        device->CommonDevice = (void*)&CommonData;
        device_temp->NextDevice = device;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

int DeviceClearnAllLearnMode(void)
{
    stDevice *device_temp = NULL;

    if (NULL == device_all)
    {
        goto out;
    }

    device_temp = device_all;

    while(device_temp)
    {
        if (E_DEVICE_MODE_LEARN == device_temp->ir_mode)
        {
             device_temp->ir_mode = E_DEVICE_MODE_NORMAL;
        }

        device_temp = device_temp->NextDevice;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}


int DeviceSetDeviceLearnMode(stDevice *device)
{
    stDevice *device_temp = NULL;

    if ((NULL == device) || (NULL == device_all))
    {
        goto out;
    }

    device_temp = findDevicebyID(device->device_index);
    if (NULL == device_temp)
    {
        goto out;
    }

    if (RETURN_OK != DeviceClearnAllLearnMode())
    {
        goto out;
    }

    device_temp->ir_mode = E_DEVICE_MODE_LEARN;

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

stDevice* DeviceGetLearnModeDevice(void)
{
    stDevice *device_temp = NULL;

    if (NULL == device_all)
    {
        goto out;
    }

    device_temp = device_all;

    while(device_temp)
    {
        if (E_DEVICE_MODE_LEARN == device_temp->ir_mode)
        {
            break;
        }
        device_temp = device_temp->NextDevice;
    }

    if (NULL == device_temp)
    {
        goto out;
    }

    return device_temp;

out:
    Errprint();
    return NULL;
}

static int CreatDeviceLock(void)
{
    //初始化DeviceMutex
    DeviceMutex = xSemaphoreCreateMutex();
    if (NULL == DeviceMutex)
    {
        goto out;
    }

    return RETURN_OK;

out:
    return RETURN_ERR;
}


/**
 * @brief 红外发送回调函数
 *
 * @param event 发送事件
 */
void IrSendCallback(ir_send_event_t event)
{
    mprintf("send event = %d\n",event);
    // prompt_play_by_cmd_id(1005,-1,NULL,true);
}


int DeviceInit(void)
{
    int ret = RETURN_ERR;
    stIrPinInfo irPinInfo;
    stDevice *device_Now = NULL;

    if (RETURN_OK != CreatDeviceLock())
    {
        goto out;
    }

    memset(&irPinInfo, 0x00, sizeof(stIrPinInfo));

    //config outpin
    irPinInfo.outPin.PinName    = PA2;
    irPinInfo.outPin.GpioBase   = PA;
    irPinInfo.outPin.PinNum     = pin_2;
    irPinInfo.outPin.PwmFun     = FIFTH_FUNCTION;
    irPinInfo.outPin.IoFun      = FIRST_FUNCTION;
    irPinInfo.outPin.PwmBase    = PWM0;

    //config revpin
    irPinInfo.revPin.PinName    = PA4;
    irPinInfo.revPin.GpioBase   = PA;
    irPinInfo.revPin.PinNum     = pin_4;
    irPinInfo.revPin.IoFun      = FIRST_FUNCTION;
    irPinInfo.revPin.GpioIRQ    = PA_IRQn;

    //timer config
    irPinInfo.irTimer.ir_use_timer      = TIMER2;
    irPinInfo.irTimer.ir_use_timer_IRQ  = TIMER2_IRQn;

    //set io info
    ir_init(&irPinInfo);

    /*小夜灯初始化*/
    common_device_InitNightLevel();

#ifdef XTIMERDEVICE
    DeviceTimerStart();
#endif

    //初始化common dvice
    CommonData.CommonDevice = get_main_device();

    //添加设备

    AddDevice(get_main_device());//添加公共设备
    AddDevice(get_air_device());//添加设备空调

#ifdef DEVICE_SUPPORT_TV
    AddDevice(get_tv_device());//添加设备电视
#endif

#ifdef DEVICE_SUPPORT_FAN
    AddDevice(get_fan_device());//添加设备电视
#endif
#ifdef DEVICE_SUPPORT_LIGHT
    AddDevice(get_light_device());//添加设备机顶盒
#endif

    //初始化设备
    device_Now = getFirstDevice();
    while(device_Now)
    {
        if (device_Now->Init)
        {
            ret = device_Now->Init(device_Now);
            if (RETURN_OK != ret)
            {
                mprintf("device:%d init fail\n", device_Now->device_index);
                goto out;
            }
        }
        device_Now = device_Now->NextDevice;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}

int DeviceFindAsr(uint32_t cmd_handle, int * device_id)
{
    int ret = RETURN_ERR;
    stDevice *device_Now = NULL;
    stDevice *device_Common = NULL;

    if (NULL == device_id)
    {
        goto out;
    }

    /*如果已经是某一个设备的学习，只查找当前进入学习模式的设备*/
    device_Now = DeviceGetLearnModeDevice();

    if (NULL != device_Now)
    {
        ret = device_Now->GetAsrId(device_Now, cmd_handle, device_id);
        if ((RETURN_OK != ret) || (*device_id != device_Now->device_index))
        {
            /*当前设备没有找到，需要在主设备里面去找*/
            device_Common = get_main_device();
            if (device_Common->GetAsrId)
            {
                ret = device_Common->GetAsrId(device_Common, cmd_handle, device_id);
                if ((RETURN_OK != ret) || (*device_id != device_Common->device_index))
                {
                    goto out;
                }

            }
        }
    }
    else
    {
        device_Now = getFirstDevice();
        while(device_Now)
        {
            if (device_Now->GetAsrId)
            {
                ret = device_Now->GetAsrId(device_Now, cmd_handle, device_id);
                if ((RETURN_OK == ret) && (*device_id == device_Now->device_index))
                {
                    break;
                }
            }

            device_Now = device_Now->NextDevice;
        }
    }

    if (NULL == device_Now)
    {
        goto out;
    }

    return RETURN_OK;

out:
    Errprint();
    return RETURN_ERR;
}
#ifdef XTIMERDEVICE
void DeviceTimerStart(void)
{
    xTimerStart(xTimerDevice,0);
}

void DeviceTimerStop(void)
{
    xTimerStop(xTimerDevice,0);
}

void DeviceTimerCallback(TimerHandle_t xTimer)
{
    //每隔DEVICE_TIME的时间 发一次心跳
    device_common_sendmsg(TYPE_DEVICEMSG_SOFTTIM, NULL, NULL, NULL);
}
#endif
