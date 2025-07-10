/**
 * @file sdk_default_config.h
 * @brief sdk配置文件
 * @version 1.0
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 * 
 */
#ifndef _SDK_DEFAULT_CONFIG_H_
#define _SDK_DEFAULT_CONFIG_H_

#if defined(CI_CONFIG_FILE)
#include CI_CONFIG_FILE
#endif  


/******************************** sdk config *********************************/
#define MERGE_OFFLINE_SDK_VERSION     "2.1.8"
// SDK VER
#define SDK_VERSION                    2
#define SDK_SUBVERSION                 5
#define SDK_REVISION                   28
#define SDK_TYPE                       "ALG_PRO"

/******************************** 开发板选择 **********************************/
//板级支持配置
#ifndef BOARD_PORT_FILE
#define BOARD_PORT_FILE                "CI-D06GT01D.c"
#endif

// //示例板级支持配置
// #ifndef BOARD_PORT_FILE
// #define BOARD_PORT_FILE            "board_port_xxx.c"
// #endif

/******************************** 灯控功能选择 **********************************/
// 使用VAD指示灯控
#ifndef USE_VAD_LIGHT
#define USE_VAD_LIGHT                   0       
#endif

// 使用BLINK闪烁灯控
#ifndef USE_BLINK_LIGHT
#define USE_BLINK_LIGHT                 0       
#endif

// 使用NIGHT小夜灯灯控
#ifndef USE_NIGHT_LIGHT
#define USE_NIGHT_LIGHT                 0       
#endif

// 使用COLOR三色彩灯灯控
#ifndef USE_COLOR_LIGHT
#define USE_COLOR_LIGHT                 0       
#endif

/******************************** 外设功能选择 **********************************/
// 使用IIC引脚配置
#ifndef USE_IIC_PAD
#define USE_IIC_PAD                     0       
#endif

// 退出唤醒时间
#ifndef EXIT_WAKEUP_TIME
#define EXIT_WAKEUP_TIME                15000   //default exit wakeup time,unit ms
#endif

// volume set
#ifndef VOLUME_MAX
#define VOLUME_MAX                      7       //voice play max volume level
#endif
#ifndef VOLUME_MID
#define VOLUME_MID                      5       //voice play middle volume level
#endif
#ifndef VOLUME_MIN
#define VOLUME_MIN                      1       //voice play min volume level
#endif

#ifndef VOLUME_DEFAULT
#define VOLUME_DEFAULT                  5       //voice play default volume level
#endif

// customer Ver (user define)
#ifndef USER_VERSION_MAIN_NO
#define USER_VERSION_MAIN_NO            1
#endif
#ifndef USER_VERSION_SUB_NO
#define USER_VERSION_SUB_NO             0
#endif
#ifndef USER_TYPE
#define USER_TYPE                       "CustomerAA"
#endif


// 使用COLOR三色彩灯灯控
#ifndef GS0XJ_BOARD_PA_AUTO
#define GS0XJ_BOARD_PA_AUTO             0      //GS0XJ系列板子PA是否开启自动检测功能。0、关闭（默认低电平有效）。1、开启。    
#endif


/* 串口调试LOG输出 */
#ifndef CONFIG_CI_LOG_UART
#define CONFIG_CI_LOG_UART                  HAL_UART0_BASE
#endif

/* 是否开启命令行控制台 */
#ifndef COMMAND_LINE_CONSOLE_EN
#define COMMAND_LINE_CONSOLE_EN         0
#endif

/* 驱动中断采用事件标准组配置为1，采用全局变量配置为0 */
#ifndef DRIVER_OS_API
#define DRIVER_OS_API                       0
#endif

#ifndef CONFIG_DIRVER_BUF_USED_FREEHEAP_EN
#define CONFIG_DIRVER_BUF_USED_FREEHEAP_EN  1
#endif

/* 在播放中是否暂停语音输入，一般来讲开启aec则不开启暂停语音输入否则开启 */
#ifndef PAUSE_VOICE_IN_WITH_PLAYING
#define PAUSE_VOICE_IN_WITH_PLAYING         1
#endif

/*是否允许关闭提示播报音 */
#ifndef PROMPT_CLOSE_EN
#define PROMPT_CLOSE_EN                     1
#endif

/*异常复位跳过上电播报 */
#ifndef EXCEPTION_RST_SKIP_BOOT_PROMPT
#define EXCEPTION_RST_SKIP_BOOT_PROMPT      1
#endif

/*默认模型分组ID，开机后第一次运行的识别环境 */
#ifndef DEFAULT_MODEL_GROUP_ID
#define DEFAULT_MODEL_GROUP_ID              0
#endif

/* 启用非唤醒状态切换模型 */
#ifndef USE_SEPARATE_WAKEUP_EN
#define USE_SEPARATE_WAKEUP_EN              1
#endif 

/* 启用降频模式,超过0.7M模型不能开降频,如:923模型 */
#ifndef USE_LOWPOWER_DOWN_FREQUENCY        
#define USE_LOWPOWER_DOWN_FREQUENCY         0       
#endif

/* 启用低功耗模式 */
// #ifndef USE_LOWPOWER_OSC_FREQUENCY
// #define USE_LOWPOWER_OSC_FREQUENCY          0       //暂不能使用
// #endif

/*************************** 解码参数配置 ************************************/
#ifndef DECODER_MIN_ACTIVE
#define DECODER_MIN_ACTIVE  (400)   //默认400，若节点小于8000，可尝试修改到300；最小值200，最大值800
#endif

#ifndef DECODER_BEAM    
#define DECODER_BEAM    (9.5f)  //最小值3.5 最大值15.5
#endif

#ifndef DEFAULT_CONFIDENCE 
#define DEFAULT_CONFIDENCE      (60) // 配置默认置信度宏 :Excel未配置置信度的词条的默认值, 理论范围[1-255],建议范围[30-150]
#endif 

#ifndef DEFAULT_CNT 
#define DEFAULT_CNT             (6) // 配置默认cnt宏 :Excel未配置cnt的词条的默认值,理论范围[1-255],建议范围[4-20]
#endif 

//以下宏和响应速度相关
#ifndef ADAPTIVE_CNT_ENABLE    
#define ADAPTIVE_CNT_ENABLE     (1)  //配置自适应cnt使能宏 ，1：打开自适应 cnt; 0：关闭，加快响应速度，但必须手动在excel配置短词的cnt；
#endif 

//以下宏和响应速度相关
#ifndef MAX_STOP_VAD_FRM
#define MAX_STOP_VAD_FRM        (60)  //配置VAD_END帧数宏 最大停顿静音间隔帧数 30=300ms 值越大识别响应时间会加长;过短导致出现截断，长词识别为短词 ,建议：英文/小语种 该值至少35,理论范围 [1-60],建议范围[20-50]
#endif

//以下宏和响应速度相关
#ifndef MAX_STOP_CFD_ENABLE
#define MAX_STOP_CFD_ENABLE      (0) // 打分超过阈值提前结束功能 0 关闭 1 开启(可提升响应速度)
#endif 

#ifndef MAX_STOP_CFD_NOCNT
#define MAX_STOP_CFD_NOCNT      (50) // MAX_STOP_CFD_ENABLE为1有效 无cnt配置的词，最大结束阈值,理论范围 [5-200],建议范围[50-150]； 
#endif

#ifndef MAX_STOP_CFD_CNT
#define MAX_STOP_CFD_CNT        (60) // MAX_STOP_CFD_ENABLE为1有效 有cnt配置的词，最大结束阈值,至少要比 MAX_STOP_CFD_NOCNT 大,理论范围 [5-200],建议范围[50-150]；
#endif

#ifndef DEFAULT_STOP_SILPROB
#define DEFAULT_STOP_SILPROB    (0.9f) // 识别结束停顿静音概率阈值,理论范围[0-1]，值越大支持的停顿时间越长
#endif

//以下宏和响应速度相关
#ifndef DEFAULT_STOP_SILCNT
#define DEFAULT_STOP_SILCNT     (15)  // 识别结束静音连续次数宏：识别结束停顿静音,值越大支持的停顿时间越长,理论范围 [1-200] 建议范围 [1-10]
#endif

#ifndef RECOVER_RESULT_ENABLE
#define RECOVER_RESULT_ENABLE    (1)  //语音识别结果回收使能配置（整段语音结束，无有效结果，再回溯搜索打分满足阈值[CNT不满足]的结果）, 开启人声噪音识别率会增加，但集外误识别会提升
#endif 

#ifndef RECOVER_RESULT_MODE
#define RECOVER_RESULT_MODE      (0) // RECOVER_RESULT_ENABLE 为1 有效，语音识别结果回收使能作用模式，0 命令词和唤醒词; 1 唤醒词
#endif 

#ifndef RECOVER_RESULT_MAX_FRM
#define RECOVER_RESULT_MAX_FRM   (0) // RECOVER_RESULT_ENABLE 为1 有效，回收使能 在CNT基础上再增加超时帧数 , 10=300 ms ,理论范围 [0-200],建议范围[0-20]
#endif 

/*************************** 语音前端算法 config *****************************/

/*单MIC DENOISE*/
#ifndef USE_DENOISE_MODULE
#define USE_DENOISE_MODULE             0
#endif
/*单MIC 深度学习降噪*/
#ifndef USE_DENOISE_NN
#define USE_DENOISE_NN                 0   
#endif

/*单MIC 深度学习降噪（通话降噪）*/
#ifndef USE_DENOISE_NN_RTC
#define USE_DENOISE_NN_RTC            0    
#endif

/*单MIC 均衡器*/
#ifndef USE_EQ_MODULE
#define USE_EQ_MODULE                  0    
#endif

/*单MIC 动态范围压缩*/
#ifndef USE_DRC_MODULE
#define USE_DRC_MODULE                 0    
#endif

/*双MIC AI DOA*/
#ifndef USE_AI_DOA
#define USE_AI_DOA                     0  
#endif

/*双MIC DEREVERB*/
#ifndef USE_DEREVERB_MODULE
#define USE_DEREVERB_MODULE            0
#endif

/*双MIC BEAMFORMING*/
#ifndef USE_BEAMFORMING_MODULE
#define USE_BEAMFORMING_MODULE         0
#endif

/*回声消除 AEC*/
#ifndef USE_AEC_MODULE
#define USE_AEC_MODULE                 0    
#endif

/*啸叫抑制 AHS*/
#ifndef USE_AHS_MODULE
#define USE_AHS_MODULE                 0    
#endif

/*动态alc:ALC AUTO SWITCH*/
#ifndef USE_ALC_AUTO_SWITCH_MODULE
#define USE_ALC_AUTO_SWITCH_MODULE                0
#endif

/*是否codec录音只支持单个声道,需根据算法选择配合使用*/
#ifndef AUDIO_CAPTURE_USE_SINGLE_CHANNEL
#define AUDIO_CAPTURE_USE_SINGLE_CHANNEL   1
#endif

/*深度分离使能*/
#ifndef DEEP_SEPARATE_ENABLE
#define DEEP_SEPARATE_ENABLE              0
#endif

#ifndef ASR_FE_REDUCE_MEM             
#define ASR_FE_REDUCE_MEM                0             //默认关闭  0:原方案 1:省内存方案(15K 左右)
#endif

#ifndef USE_DUAL_MIC_ANY
#define USE_DUAL_MIC_ANY   0//使用任意MIC都可以识别
#endif


/*是否使用重采样*/
#ifndef INNER_CODEC_AUDIO_IN_USE_RESAMPLE
#define INNER_CODEC_AUDIO_IN_USE_RESAMPLE   1   //默认打开   0:不开重采样   1：打开重采样
#endif


/*内部CODEC ALC的最大增益*/
#ifndef INNER_CODEC_ALC_MAX_GAIN    
#define INNER_CODEC_ALC_MAX_GAIN            INNER_CODEC_ALC_PGA_MAX_GAIN_28_5dB
#endif


/*通话降噪hp输出音量调节*/
#ifndef RTC_HPOUT_DEFAULT_VOL   
#define RTC_HPOUT_DEFAULT_VOL               50
#endif


/*通话降噪输出降噪前，还是降噪后的数据*/
#ifndef RTC_OUT_IS_VIA_DENOISE                  
#define RTC_OUT_IS_VIA_DENOISE              1// 0：降噪之前的数据，1：降噪之后的数据
#endif

/*通话降噪EQ等参数配置使用的串口*/
#ifndef RTC_EQ_UART_PORT
#define RTC_EQ_UART_PORT                    (HAL_UART1_BASE)
#endif


/*根据codec使用的声道数来配置使用内存大小*/
#if !CONFIG_DIRVER_BUF_USED_FREEHEAP_EN
#if AUDIO_CAPTURE_USE_SINGLE_CHANNEL
    #define AUDUI_CAPTURE_NO_USE_MALLOC_BLOCK_SIZE (512U)
#else
    #define AUDUI_CAPTURE_NO_USE_MALLOC_BLOCK_SIZE (1024U)
#endif
#endif

/*关识别流程，仅保留前端算法处理流程*/
#ifndef NO_ASR_FLOW
#define NO_ASR_FLOW             0
#endif


/*事件检测算法*/
#ifndef USE_SED
#define USE_SED                 0
#endif

/*TTS算法*/
#ifndef USE_TTS
#define USE_TTS                 0
#endif

/******************************** VP config **********************************/

//1:开启VP计算；0：关闭VP计算
#ifndef USE_VPR
#define USE_VPR                             0
#endif

#ifndef MAX_VP_TEMPLATE_NUM
#define MAX_VP_TEMPLATE_NUM                  4
#endif

#ifndef MAX_VP_REG_TIME
#define MAX_VP_REG_TIME                      10 //注册声纹时最大超时等待时间（秒）
#endif

#ifndef VP_USE_FRM_LEN  
#define VP_USE_FRM_LEN                      500//声纹计算的窗长，单位为ms
#endif

#ifndef VP_CMPT_SKIP_NUM  
#define VP_CMPT_SKIP_NUM                    1//声纹计算的窗长，单位为ms
#endif

#ifndef VP_THR_FOR_MATCH                    
#define VP_THR_FOR_MATCH                    (0.52f)//声纹阈值
#endif


/******************************** command word self-learning config **********************************/
#ifndef USE_CWSL
#define USE_CWSL                             0 //命令词自学习(command word self-learning)
#endif

#ifndef CICWSL_TOTAL_TEMPLATE
#define CICWSL_TOTAL_TEMPLATE                16 //可存储模板数量
#endif


/**************************** audio player config ****************************/
/* 注册prompt解码器 */
#ifndef USE_PROMPT_DECODER
#define USE_PROMPT_DECODER                   1
#endif

/* 注册mp3解码器 */
#ifndef USE_MP3_DECODER
#define USE_MP3_DECODER                     1
#endif

/* 注册m4a(aac)解码器 */
#ifndef USE_AAC_DECODER
#define USE_AAC_DECODER                     0
#endif

/* 注册ms wav解码器 */
#ifndef USE_MS_WAV_DECODER
#define USE_MS_WAV_DECODER                  0
#endif

/* 注册flac解码器 */
#ifndef USE_FLAC_DECODER
#define USE_FLAC_DECODER                    0
#endif

/* 命令词支持MP3格式音频，注意目前仅支持具有CI特殊头格式的MP3文件 */
#ifndef AUDIO_PLAY_SUPPT_MP3_PROMPT
#define AUDIO_PLAY_SUPPT_MP3_PROMPT         0
#endif
#if AUDIO_PLAY_SUPPT_MP3_PROMPT
#if !USE_MP3_DECODER
#error "if AUDIO_PLAY_SUPPT_MP3_PROMPT enabled please config USE_MP3_DECODER enable in your user_config.h"
#endif
#endif

/* 命令词支持FLAC格式音频，注意目前仅支持具有CI特殊头格式的FLAC文件 */
#ifndef AUDIO_PLAY_SUPPT_FLAC_PROMPT
#define AUDIO_PLAY_SUPPT_FLAC_PROMPT        0
#endif
#if AUDIO_PLAY_SUPPT_FLAC_PROMPT
#if !USE_FLAC_DECODER
#error "if AUDIO_PLAY_SUPPT_FLAC_PROMPT enabled please config USE_FLAC_DECODER enable in your user_config.h"
#endif
#endif

/* 命令词支持标准IMA ADPCM格式音频 */
#ifndef AUDIO_PLAY_SUPPT_IMAADPCM_PROMPT
#define AUDIO_PLAY_SUPPT_IMAADPCM_PROMPT    1
#endif

/* 解码器内存使用动态分配 */
#ifndef AUDIO_PLAY_USE_DYNAMIC_DECODER_MEM
#define AUDIO_PLAY_USE_DYNAMIC_DECODER_MEM  0
#endif

/* 数据组合播报 */
#ifndef AUDIO_PLAY_USE_QSPI_FLASH_LIST
#define AUDIO_PLAY_USE_QSPI_FLASH_LIST      0
#endif

/* 启用网络播放（需要lwip支持） */
#ifndef AUDIO_PLAY_USE_NET
#define AUDIO_PLAY_USE_NET                  0
#endif

/* 启用自定义外部数据源播放 */
#ifndef AUDIO_PLAY_USE_OUTSIDE
#define AUDIO_PLAY_USE_OUTSIDE              0
#endif

/* 启用自定义外部数据源播放 */
#ifndef AUDIO_PLAY_USE_OUTSIDE_V2
#define AUDIO_PLAY_USE_OUTSIDE_V2           0
#endif

/* 启用文件系统播放 */
#ifndef AUDIO_PLAY_USE_SD_CARD
#define AUDIO_PLAY_USE_SD_CARD              0
#endif

/*! 启用播放器双声道混音到右声道功能 */
#ifndef AUDIO_PLAY_USE_MIX_2_CHANS
#define AUDIO_PLAY_USE_MIX_2_CHANS                     0
#endif

/*! 自动音频识别文件头(消耗额外内存,播放m4a、flac、非单声道16Kwav音频格式时必须打开) */
#ifndef AUDIO_PLAYER_CONFIG_AUTO_PARSE_AUDIO_FILE
#define AUDIO_PLAYER_CONFIG_AUTO_PARSE_AUDIO_FILE      0
#endif

/*! 针对TTS音频倍速播放功能，注意仅支持加/减速播放TTS人声，不支持音乐 */
#ifndef AUDIO_PLAY_USE_SPEEDING_SPEECH
#define AUDIO_PLAY_USE_SPEEDING_SPEECH                 0
#endif


/*! 唤醒词播放词常驻内存 */
#ifndef AUDIO_PLAY_SUPPT_WAKEUP_VOICE_IN_RAM
#define AUDIO_PLAY_SUPPT_WAKEUP_VOICE_IN_RAM           0
#endif

/*! 用于解决应用程序可能存在的偏移不对齐问题 */
#ifndef AUDIO_PLAYER_FIX_OFFSET_ISSUE
#define AUDIO_PLAYER_FIX_OFFSET_ISSUE                  0
#endif


/****************************** 功放控制 config *******************************/

/*Whether the PA switch is controled by audio player*/
#ifndef PLAYER_CONTROL_PA
#define PLAYER_CONTROL_PA                              0
#endif


/****************************** 串口协议 config *******************************/
#ifndef MSG_COM_USE_UART_EN
#define MSG_COM_USE_UART_EN                0
#endif


#ifndef UART_PROTOCOL_NUMBER
#define UART_PROTOCOL_NUMBER	          (HAL_UART1_BASE)// HAL_UART0_BASE ~ HAL_UART2_BASE
#endif

#ifndef UART_PROTOCOL_BAUDRATE
#define UART_PROTOCOL_BAUDRATE            (UART_BaudRate9600)
#endif

#ifndef UART_PROTOCOL_VER
#define UART_PROTOCOL_VER                 2   //串口协议版本号，1：一代协议，2：二代协议，255：平台生成协议(只有发送没有接收)
#endif

#ifndef UART0_PAD_OPENDRAIN_MODE_EN
#define UART0_PAD_OPENDRAIN_MODE_EN        0   //UART引脚开启开漏模式，支持5V电平。0:关闭开漏模式；1：开启开漏模式。
#endif

#ifndef UART1_PAD_OPENDRAIN_MODE_EN
#define UART1_PAD_OPENDRAIN_MODE_EN        0   //UART引脚开启开漏模式，支持5V电平。0:关闭开漏模式；1：开启开漏模式。
#endif

#ifndef UART2_PAD_OPENDRAIN_MODE_EN
#define UART2_PAD_OPENDRAIN_MODE_EN        0   //UART引脚开启开漏模式，支持5V电平。0:关闭开漏模式；1：开启开漏模式。
#endif



#ifndef UART_BAUDRATE_CALIBRATE
#define UART_BAUDRATE_CALIBRATE             0               //是否使能波特率校准功能
#endif

#if UART_BAUDRATE_CALIBRATE

#ifndef BAUDRATE_SYNC_PERIOD
#define BAUDRATE_SYNC_PERIOD        300000       // 波特率同步周期，单位毫秒
#endif

#ifndef BAUDRATE_FAST_SYNC_PERIOD
#define BAUDRATE_FAST_SYNC_PERIOD   5000         // 一次波特率校准识别后，下一次同步周期，单位毫秒
#endif

// #ifndef UART_CALIBRATE_FIX_BAUDRATE
// #define UART_CALIBRATE_FIX_BAUDRATE         9600            //通信波特率
// #endif

// #ifndef UART_CALIBRATE_MAX_ERR_PROPORITION
// #define UART_CALIBRATE_MAX_ERR_PROPORITION  0.08            //允许的最大误差比8%
// #endif

// #ifndef UART_CALIBRATE_ARRAY_SIZE
// #define UART_CALIBRATE_ARRAY_SIZE           16              //最大数据缓存
// #endif

// #ifndef UART_CALIBRATE_IO_BASE
// #define UART_CALIBRATE_IO_BASE              PA              //校准波特率的IO口(RX引脚)
// #endif

// #ifndef UART_CALIBRATE_IO_PIN
// #define UART_CALIBRATE_IO_PIN               pin_2
// #endif

// #ifndef UART_CALIBRATE_IO_PAD
// #define UART_CALIBRATE_IO_PAD               PA2
// #endif

// #ifndef UART_CALIBRATE_IRQ
// #define UART_CALIBRATE_IRQ                  PA_IRQn
// #endif

// #ifndef UART_CALIBRATE_TIMER
// #define UART_CALIBRATE_TIMER                TIMER2
// #endif

// #ifndef UART_CALIBRATE_TIMER_BASE
// #define UART_CALIBRATE_TIMER_BASE           HAL_TIMER2_BASE
// #endif

 #endif


/***************************** flash加密 config ******************************/
/*flash防拷贝加密 */
#ifndef COPYRIGHT_VERIFICATION
#define COPYRIGHT_VERIFICATION          0 //1：使能flash加密校验，用于防止flash拷贝。需要在lds文件增加ROM段大小。0：禁止flash加密校验
#endif

#if (COPYRIGHT_VERIFICATION == 1)
#define ENCRYPT_DEFAULT                 0 //默认加密算法
#define ENCRYPT_USER_DEFINE             1 //用户自定义加密算法

#define ENCRYPT_ALGORITHM               ENCRYPT_DEFAULT //设置加密方式
#endif


/****************************** i2c communicate config *******************************/
#ifndef MSG_USE_I2C_EN
#define MSG_USE_I2C_EN                0                  /*使能IIC通信协议*/
#endif

#ifndef I2C_PROTOCOL_SPEED
#define I2C_PROTOCOL_SPEED            (100)              /*IIC传输速度*/
#endif


// 播报音配置
#ifndef PLAY_WELCOME_EN
#define PLAY_WELCOME_EN                     1 //欢迎词播报          =1是 =0否
#endif
#ifndef PLAY_ENTER_WAKEUP_EN
#define PLAY_ENTER_WAKEUP_EN                1 //唤醒词播报          =1是 =0否
#endif
#ifndef PLAY_EXIT_WAKEUP_EN
#define PLAY_EXIT_WAKEUP_EN                 1 //退出唤醒播报        =1是 =0否
#endif
#ifndef PLAY_OTHER_CMD_EN
#define PLAY_OTHER_CMD_EN                   1 //命令词播报          =1是 =0否
#endif
/******************************ASR config************************************/
#ifndef ADAPTIVE_THRESHOLD
#define ADAPTIVE_THRESHOLD                  0   //ASR 自适应阈值    =1 开启  =0 关闭
#endif

#ifndef ASR_SKIP_FRAME_CONFIG
#define ASR_SKIP_FRAME_CONFIG               0   //跳帧模式  0:关闭跳帧，1:只有命令词网络跳帧，2:所有模型都跳帧
#endif
#endif /*_SDK_DEFAULT_CONFIG_H_*/


#define FLASH_CPU_READ_BASE_ADDR	(0x50000000U)



#define VAD_SENSITIVITY_HIGH        1        // VAD 高灵敏度，适用于低音量语音,安静，或者各种噪声环境,但功耗偏高。
#define VAD_SENSITIVITY_MID         3        // VAD 中灵敏度，适用于安静及轻度噪声环境，功耗适中。
#define VAD_SENSITIVITY_LOW         9        // VAD 低灵敏度，仅适用于安静环境，降低误触发，也可以降低功耗。


#ifndef VAD_SENSITIVITY
#define VAD_SENSITIVITY         VAD_SENSITIVITY_HIGH    //用于配置VAD灵敏度
#endif



//语音相关的宏
/*********************** 使用外部IIS输入语音进行识别 *************************/
#ifndef AUDIO_IN_BUFFER_NUM
    #if USE_DENOISE_NN || USE_AI_DOA
    #if USE_LARGE_MODEL
    #define AUDIO_IN_BUFFER_NUM (20)//只为DENOISE_NN和USE_AI_DOA留这么多
    #else
    #define AUDIO_IN_BUFFER_NUM (12)//只为DENOISE_NN和USE_AI_DOA留这么多
    #endif
    #else
    #define AUDIO_IN_BUFFER_NUM (4)
    #endif
#endif


//使用IIS输出算法处理之后的语音
#ifndef USE_IIS1_OUT_PRE_RSLT_AUDIO
#define USE_IIS1_OUT_PRE_RSLT_AUDIO  (0)
#endif

//vad标签，调试使用-不可修改
#ifndef VAD_PCM_MARK_EN              
#define VAD_PCM_MARK_EN              (1)
#endif
//使用HP输出算法处理之后的语音(RTC)
#ifndef USE_HP_OUT_PRE_RSLT_AUDIO
#define USE_HP_OUT_PRE_RSLT_AUDIO  (0)
#endif


//根据 HOST_MIC_RECORD_CODEC_ID 确定
// #ifndef MIC_FROM_WHICH_IIS
// #define MIC_FROM_WHICH_IIS  1//0：MIC接外部CODEC  1：MIC接内部CODEC 2：MIC使用PDM
// #endif


//根据 PLAY_CODEC_ID 确定
// #ifndef USE_OUTSIDE_CODEC_PLAY
// #define USE_OUTSIDE_CODEC_PLAY      0//使用外部codec进行播放
// #endif

#ifndef HOST_MIC_USE_NUMBER              
#define HOST_MIC_USE_NUMBER     1      //使用mic数量
#endif

#ifndef HOST_MIC_RECORD_CODEC_ID              
#define HOST_MIC_RECORD_CODEC_ID     1   //主MIC录音codec id: 0：主MIC接外部CODEC  1：主MIC接内部CODEC 2：主MIC使用PDM
#endif

#ifndef HOST_CODEC_CHA_NUM              
#define HOST_CODEC_CHA_NUM     1   //主COEDC的通道数，使用双麦算法和单MIC AEC的时候都应是2
#endif




#ifndef ASSIST_MIC_RECORD_CODEC_ID              
#define ASSIST_MIC_RECORD_CODEC_ID     2   //副MIC录音codec id: 0：副MIC接外部CODEC  1：副MIC接内部CODEC 2：副MIC使用PDM
#endif


#ifndef REF_RECORD_CODEC_ID              
#define REF_RECORD_CODEC_ID     0   //参考信号录音codec id: 0：参考信号接外部CODEC  1：参考信号接内部CODEC 2：参考信号使用PDM，
#endif                              //只在参考和MIC使用不一样的CODEC时使用


#ifndef REF_RECORD_CODEC_CHA_NUM              
#define REF_RECORD_CODEC_CHA_NUM     1   //主COEDC的通道数，使用双麦算法和单MIC AEC的时候都应是2
#endif                                   //只在参考和MIC使用不一样的CODEC时使用

#if ((HOST_MIC_RECORD_CODEC_ID + ASSIST_MIC_RECORD_CODEC_ID + REF_RECORD_CODEC_ID) != 3)
#error "error\n"
#endif

#if (ASSIST_MIC_RECORD_CODEC_ID == HOST_MIC_RECORD_CODEC_ID)
#error "error\n"
#endif


#ifndef IF_USE_ANOTHER_CODEC_TO_GET_REF           
#define IF_USE_ANOTHER_CODEC_TO_GET_REF           0   //是否使用另外的CODEC采集参考信号，0:否  1:是
#endif


#ifndef PLAY_CODEC_ID              
#define PLAY_CODEC_ID           1   //播音codec id  0：播报使用接外部CODEC  1：播报使用内部CODEC
#endif


#ifndef PLAY_PRE_AUDIO_CODEC_ID
#define PLAY_PRE_AUDIO_CODEC_ID      0  //播放预处理之后的语音的codec id  只能使用外部IIS，只能是0
#endif


#ifndef AUDIO_CAP_POINT_NUM_PER_FRM    
#define AUDIO_CAP_POINT_NUM_PER_FRM    (256)//采音每帧的点数  16K采样率下一帧的采样点数，160
#endif


#ifndef USE_SOFT_AEC_REF    
#define USE_SOFT_AEC_REF    (0)             //是否使用软回采
#endif


#ifndef AUDIO_CAP_DIV_NUM    
#define AUDIO_CAP_DIV_NUM    (4)            //软回采的时候，将原来的一帧分为多少段
#endif


#ifndef AUDIO_IN_FROM_DMIC
#define AUDIO_IN_FROM_DMIC                  0       //是否使用数字MIC输入音频
#endif


/**********I2S interface config**********/
#ifndef USE_I2S_INTERFACE_SCK_LRCK_32
#define USE_I2S_INTERFACE_SCK_LRCK_32              0//I2S 传输语音数据到WIFI时，SCK与LRCK的比值是否为32，否则为64
#endif


#ifndef IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY
#define IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY    0//没有播报的时候只关HP：0：没有播报的时候关DAC（省电，但是蓝牙播报的时候会干扰ADC）
#endif    


/************* 时钟源配置选择 *************************/
#ifndef USE_EXTERNAL_CRYSTAL_OSC
#define USE_EXTERNAL_CRYSTAL_OSC             1  //是否使用外部晶振。0：不使用外部晶振时钟；1：使用外部晶振时钟；默认使用外部晶振。
#endif


/************* 电源配置选择 *************************/
#ifndef USE_INNER_LDO3
#define USE_INNER_LDO3                      1
#endif


/*! 在线应用相关支持的开关 */
#ifndef ON_LINE_SUPPORT
//设置为 1 时，lds调整向后移4K
#define ON_LINE_SUPPORT                     0    // 0: 关闭在线应用相关支持；1: 开启在线应用相关支持。
#endif


/*! 离线双麦算法相关支持的开关，打开之后需要调整lds少8K */
#ifndef OFFLINE_DUAL_MIC_ALG_SUPPORT
#define OFFLINE_DUAL_MIC_ALG_SUPPORT        0    // 0: 关闭离线双麦算法相关支持；1: 开启离线双麦算法相关支持。
#endif


#if USE_EXTERNAL_CRYSTAL_OSC
#undef UART_BAUDRATE_CALIBRATE
#define UART_BAUDRATE_CALIBRATE             0               //使用外部晶振时，不需要校准波特率。
#endif

#ifndef MIC_DIFF_SINGLE
#define MIC_DIFF_SINGLE                     0              //1，单端。0，差分（通用模块都是差分模式，省成本的模块为单端（MICN_L 接GND）时，需要配置为SINGLE）
#endif

#define CIAS_FLASH_IMAGE_UPGRADE_FLAG_ADDR      (0X9000)   //for ota,固定不可修改
#ifndef CI_OTA_ENABLE
#define CI_OTA_ENABLE                       0
#endif

#if (USE_LOWPOWER_DOWN_FREQUENCY || USE_SOFT_AEC_REF)
#undef INNER_CODEC_AUDIO_IN_USE_RESAMPLE
#define INNER_CODEC_AUDIO_IN_USE_RESAMPLE   0
#endif

#if USE_SOFT_AEC_REF
#undef  IF_USE_ANOTHER_CODEC_TO_GET_REF
#define IF_USE_ANOTHER_CODEC_TO_GET_REF 0
#endif

#ifndef ONE_SHOT_ENABLE
#define ONE_SHOT_ENABLE                     0              //1：开启oneshot功能。0：关闭oneshot功能
#endif

#ifndef USE_PWK
#define USE_PWK                             0               //单麦就近唤醒
#endif

#ifndef MULT_INTENT
#define MULT_INTENT                         1               //意图数量  max 3
#endif


#define NLP_CMD_CNT_DEFAULT                 30
#define NLP_CMD_CNT_END                     12
#define NLP_STL_LEN                         6
#define NLP_NODES_CFD_TIMES                 25             //节点置信度次数，用于判断是否有语音输入
#define NLP_POWER_ON_WAIT_TIMES             4              //开机与下一个词之间的间隔次数，用于是否消除开机
#define NLP_POWER_OFF_WAIT_TIMES            12             //风扇关闭与下一个词之间的间隔次数，用于是否消除开机


#ifndef CLOUD_UART_PROTOCOL_EN
#define CLOUD_UART_PROTOCOL_EN              0              //1：开启启英平台协议。0：关闭启英平台协议
#endif


/*--蓝牙BLE配置开关--*/
#ifndef EXTERNAL_CRYSTAL_OSC_FROM_RF
#define EXTERNAL_CRYSTAL_OSC_FROM_RF           0            //蓝牙端使用外部晶振, 语音端时钟由蓝牙端提供-用户不可修改
#endif
#ifndef CIAS_BLE_CONNECT_MODE_ENABLE
#define CIAS_BLE_CONNECT_MODE_ENABLE           0          //ble连接模式使能
#endif
#ifndef CIAS_BLE_SCAN_ENABLE
#define CIAS_BLE_SCAN_ENABLE                   0          //ble连接模式使能, 同时广播开启扫描功能
#if     (CIAS_BLE_SCAN_ENABLE == 1 && CIAS_BLE_CONNECT_MODE_ENABLE != 1)
#define CIAS_BLE_CONNECT_MODE_ENABLE           0
#endif
#endif
#ifndef CIAS_BLE_ADV_GROUP_MODE_ENABEL
#define CIAS_BLE_ADV_GROUP_MODE_ENABEL         0          //ble纯广播模式
#endif
#if CIAS_BLE_CONNECT_MODE_ENABLE && CIAS_BLE_ADV_GROUP_MODE_ENABEL
#error "CIAS_BLE_CONNECT_MODE_ENABLE and CIAS_BLE_ADV_GROUP_MODE_ENABEL ONLY ENABLE ONE"
#endif
#ifndef CIAS_BLE_DEBUG_ENABLE
#define CIAS_BLE_DEBUG_ENABLE                  0          //ble 测试模式-客户一般用不上
#endif
#if (CIAS_BLE_DEBUG_ENABLE == 1)                          //开启AT指令
#define CIAS_BLE_AT_ENABLE                     0
#endif 
#ifndef BLE_CONNECT_TIMEOUT
#define BLE_CONNECT_TIMEOUT                8              //蓝牙连接超时时间(S),超时会重启蓝牙协议栈,设置为0则表示不进行超时判断
#endif
#ifndef BLE_NAME_MAX_LEN
#define BLE_NAME_MAX_LEN                   18             //蓝牙广播名称最大长度
#endif
#ifndef RF_RX_TX_MAX_LEN
#define RF_RX_TX_MAX_LEN                   20             //蓝牙收发数据最大长度为20,一般不用修改
#endif