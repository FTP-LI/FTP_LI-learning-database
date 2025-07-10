#include "string.h"
#include "ci_log.h"
#include "buzzer_voice.h"
#include "FreeRTOS.h"

#if PLAY_BEEP_CMD_EN

static stBeepInfo beepinfo = {0};

//tone define
typedef unsigned short stToneType;

//   index                  0    1    2    3    4    5    6    7    8    9    10   11   12   13
//                         低7    1    2    3    4    5    6    7    高1   高2   高3   高4   高5   不发音
static stToneType tone[] = {2470,2620,2940,3300,3490,3920,4400,2940,5230,5870,6590,6980,7840,10000};//音频数据表

//check Freq
static int BeepVoiceCheckFreq(int freq)
{
    int i = 0;
    int toneNum = sizeof(tone)/sizeof(stToneType);

    for (i = 0; i < toneNum; i++)
    {
        if (freq == tone[i])
        {
            return RETURN_OK;
        }
    }

    return RETURN_ERR;
}

//func
static int BeepVoicePwmStop(void)
{
    if (1 != beepinfo.bInit)
    {
        goto out;
    }

    pwm_stop(beepinfo.cfg.PwmBase);
    dpmu_set_io_reuse(beepinfo.cfg.PinName,beepinfo.cfg.IoFun);/*gpio function*/
    gpio_set_output_level_single(beepinfo.cfg.GpioBase, beepinfo.cfg.PinNum, 0);

    return RETURN_OK;

out:
    return RETURN_ERR;
}

static int BeepVoiceCfgFreq(int freq)
{
    pwm_init_t BeePwmInit;

    if ((1 != beepinfo.bInit) ||(RETURN_OK != BeepVoiceCheckFreq(freq)))
    {
        goto out;
    }

    if (tone[13] == freq)//10000 为不发音，所以需要关掉PWM
    {
        BeepVoicePwmStop();
        return RETURN_OK;
    }

    scu_set_device_gate(beepinfo.cfg.PwmBase,ENABLE);
    dpmu_set_io_reuse(beepinfo.cfg.PinName,beepinfo.cfg.PwmFun);
    BeePwmInit.freq = freq;
    BeePwmInit.duty = 700;
    BeePwmInit.duty_max = 1000;
    pwm_init(beepinfo.cfg.PwmBase,BeePwmInit);
    pwm_set_duty(beepinfo.cfg.PwmBase,BeePwmInit.duty,BeePwmInit.duty_max);
    pwm_start(beepinfo.cfg.PwmBase);

    return RETURN_OK;

out:
    return RETURN_ERR;
}

static void BeepVoiceCallBack(TimerHandle_t xTimer)
{
    if ((1 != beepinfo.bInit) || (NULL == beepinfo.pBuff))
    {
        goto out;
    }

    beepinfo.BeepIndex++;

    if (beepinfo.BeepIndex > beepinfo.BeepCnt)
    {
        xTimerStop(beepinfo.beepTimer, 100);
        xTimerDelete(beepinfo.beepTimer, 100);
        beepinfo.beepTimer = NULL;
        beepinfo.BeepCnt = 0;
        beepinfo.BeepIndex = 0;
        beepinfo.pBuff = NULL;
        beepinfo.TimerID = 0;

        BeepVoicePwmStop();
        goto out;
    }

    if (0 == beepinfo.pBuff->ms)
    {
        goto out;
    }

    BeepVoiceCfgFreq(tone[beepinfo.pBuff->tone]);

    xTimerChangePeriod(beepinfo.beepTimer, pdMS_TO_TICKS(beepinfo.pBuff->ms), 0);

    beepinfo.pBuff++;

out:
    return;
}

//
int BeepVoiceStart(char* buff, int len)
{
    if ((NULL == buff) || (0 != (len%(sizeof(stBuff)))))
    {
        goto out;
    }

    if (NULL != beepinfo.beepTimer)
    {
        xTimerStop(beepinfo.beepTimer, 100);
        xTimerDelete(beepinfo.beepTimer, 100);
        beepinfo.beepTimer = NULL;
        beepinfo.BeepCnt = 0;
        beepinfo.BeepIndex = 0;
        beepinfo.pBuff = NULL;
        beepinfo.TimerID = 0;

        BeepVoicePwmStop();
    }

    beepinfo.pBuff = (stBuff*)buff;
    beepinfo.beepTimer = xTimerCreate("beepTimer", pdMS_TO_TICKS(100),
        pdTRUE, &beepinfo.TimerID, BeepVoiceCallBack);
    if(NULL == beepinfo.beepTimer)
    {
        mprintf("beepTimer error\n");
    }

    beepinfo.BeepCnt = len/(sizeof(stBuff));
    beepinfo.BeepIndex = 0;
    xTimerStart(beepinfo.beepTimer, 100);

    return RETURN_OK;

out:
    return RETURN_ERR;
}

int BeepVoiceInit(stBeepCfg* cfg)
{
    if ((NULL == cfg) || (1 == beepinfo.bInit))
    {
        goto out;
    }

    memcpy(&(beepinfo.cfg), cfg, sizeof(stBeepCfg));

    beepinfo.bInit = 1;

    return RETURN_OK;

out:
    return RETURN_ERR;
}


unsigned short PowerOnbeepBuff[] =
{
    3,200, 4,200, 5,200,
};

unsigned short wakeupbeepBuff[] =
{
    3,200, 4,200,
};

unsigned short exitwakeupbeepBuff[] =
{
    5,200,4,200,3,200,
};

unsigned short cmdbeepBuff[] =
{
    3,200,
};

unsigned short match_fail_beepBuff[] =
{
    3,200, 13,100, 3,200,
};

unsigned short match_success_beepBuff[] =
{
    3,200, 3,200,
};

unsigned short exit_match_beepBuff[] =
{
    3,1000,
};


#if 0
unsigned short beepBuff[] =
{
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,6,800,13,400,13,400,
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,3,800,13,400,13,400,
    2,200,2,400,3,200,5,400,3,200,5,200,6,400,3,200,2,200,1,800,
    6,200,6,400,5,200,6,200,5,200,3,200,6,200,5,800,13,400,13,400,
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,6,800,13,400,13,400,
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,3,800,13,400,13,400,
    2,200,2,400,3,200,5,400,3,200,5,200,6,400,3,200,2,200,1,800,
    6,200,6,400,5,200,6,200,5,200,3,200,6,200,1,800,13,400,8,200,
    9,200,10,200,10,400,9,200,8,200,10,200,9,200,8,200,6,800,13,400,
    6,200,8,200,9,200,9,400,8,200,6,200,9,200,8,200,6,200,5,800,
    13,400,2,200,3,200,5,200,5,400,3,200,5,200,5,500,6,200,8,600,
    7,200,6,400,6,200,10,200,9,200,9,400,8,200,6,400,5,200,6,200,
    8,1200,
};
#endif


void power_on_beep(void)
{
    BeepVoiceStart((char*)PowerOnbeepBuff, sizeof(PowerOnbeepBuff));//后续调用这个接口
}

void wake_up_beep(void)
{
    BeepVoiceStart((char*)wakeupbeepBuff, sizeof(wakeupbeepBuff));//后续调用这个接口
}

void exit_wake_up_beep(void)
{
    BeepVoiceStart((char*)exitwakeupbeepBuff, sizeof(exitwakeupbeepBuff));//后续调用这个接口
}

void cmd_beep(void)
{
    BeepVoiceStart((char*)cmdbeepBuff, sizeof(cmdbeepBuff));//后续调用这个接口
}

void match_fail_beep(void)
{
    BeepVoiceStart((char*)match_fail_beepBuff, sizeof(match_fail_beepBuff));//后续调用这个接口
}

void match_success_beep(void)
{
    BeepVoiceStart((char*)match_success_beepBuff, sizeof(match_success_beepBuff));//后续调用这个接口
}

void exit_match_beep(void)
{
    BeepVoiceStart((char*)exit_match_beepBuff, sizeof(exit_match_beepBuff));//后续调用这个接口
}


//#define TEST_BEEP_VOICE

#ifdef TEST_BEEP_VOICE  //just test Beep

/*

typedef struct
{
    unsigned short tone;
    unsigned short ms;
}stBuff;


beepBuff buffer里面的数据排布如下所示，里面都是stBuff的结构体，一个音调 tone， 一个时长(ms),依次排列
tone: 是stToneType tone[] (音频数据表)的索引index
*/
unsigned short beepBuff[] =
{
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,6,800,13,400,13,400,
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,3,800,13,400,13,400,
    2,200,2,400,3,200,5,400,3,200,5,200,6,400,3,200,2,200,1,800,
    6,200,6,400,5,200,6,200,5,200,3,200,6,200,5,800,13,400,13,400,
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,6,800,13,400,13,400,
    5,200,5,400,6,200,8,200,7,200,6,200,5,200,3,800,13,400,13,400,
    2,200,2,400,3,200,5,400,3,200,5,200,6,400,3,200,2,200,1,800,
    6,200,6,400,5,200,6,200,5,200,3,200,6,200,1,800,13,400,8,200,
    9,200,10,200,10,400,9,200,8,200,10,200,9,200,8,200,6,800,13,400,
    6,200,8,200,9,200,9,400,8,200,6,200,9,200,8,200,6,200,5,800,
    13,400,2,200,3,200,5,200,5,400,3,200,5,200,5,500,6,200,8,600,
    7,200,6,400,6,200,10,200,9,200,9,400,8,200,6,400,5,200,6,200,
    8,1200,
};


//test sample
int BeepVoiceTestSample(void)
{
    stBeepCfg cfg;

    cfg.PinName    = PWM2_PAD;
    cfg.GpioBase   = GPIO2;
    cfg.PinNum     = gpio_pin_1;
    cfg.PwmFun     = SECOND_FUNCTION,
    cfg.IoFun      = FIRST_FUNCTION;
    cfg.PwmBase    = PWM2;

    BeepVoiceInit(&cfg);//需要初始化一次

    BeepVoiceStart((char*)beepBuff, sizeof(beepBuff));//后续调用这个接口
    return 0;
}

#endif
#endif
