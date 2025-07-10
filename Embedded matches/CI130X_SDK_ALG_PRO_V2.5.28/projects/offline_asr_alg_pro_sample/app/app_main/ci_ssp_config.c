/**
  ******************************************************************************
  * @file    ci_ssp_config.c
  * @version V1.0.0
  * @date    2021.08.17
  * @brief 
  ******************************************************************************
  */

#include <stdint.h>
#include <stdbool.h>

#include "alg_preprocess.h"
#include "ci_audio_wrapfft.h"
#include "ci_denoise.h"
#include "ci_bf.h"
#include "ci_dereverb.h"
#include "alc_auto_switch.h"
#include "ci_adapt_aec.h"
#include "ci_doa.h"
#include "ci_ai_doa_init_param.h"
#include "ci_pwk.h"
#include "ai_denoise_api.h"
#include "sed_manage.h"

#include "sdk_default_config.h"
#include "status_share.h"

//采音板音频输出配置
const iis_out_audio_config_t iis_out_audio_config =
{
	.alg_enable = true,
	.iis_out_enable = USE_IIS1_OUT_PRE_RSLT_AUDIO,			 //用于采音。
    .uart_out_enable = USE_UART_SEND_PRE_RSLT_AUDIO,         //用于uart采音。
	.iis_left_channel = MICL,		 //左通道输出音频。MICL、MICR、REFL、REFR、DST1、DST2
	.iis_right_channel = DST1,		 //右通道输出音频。MICL、MICR、REFL、REFR、DST1、DST2
	.vad_mark_enable = true,		 //是否附带vad标签，默认左通道输出vad标签。
	.ssp_dst_cover_micl_enble = false //处理过后的音频dst覆盖原始micl数据,16k采样数据
};
//alc_auto_switch模块配置
const alc_auto_switch_config_t alc_auto_switch_config =
{
	.alc_auto_frame_size = AUDIO_CAP_POINT_NUM_PER_FRM * 2,
	.thr_for_non_stationary_on = 10000.0f,			//非稳态噪声下对应的上门限
	.thr_for_non_stationary_off = 3000.0f,			//非稳态噪声下对应的下门限
	.thr_for_stationary_on = 7000.0f,				//稳态噪声下对应的上门限
	.thr_for_stationary_off = 1000.0f,				//稳态噪声下对应的下门限
	.alc_auto_switch_ban_mode = NON_STATIONARY_BAN, //默认非稳态噪声下严格控制，尽可能不进行alc_auto_switch操作。
	.alc_off_codec_adc_gain = 20
};
//stft_istft模块配置
//#define FFT_MODEL 512
const stft_istft_config_t stft_istft_config =
{
//#if 512 == FFT_MODEL
	.alg_enable = true,
#if !INNER_CODEC_AUDIO_IN_USE_RESAMPLE
	.sample_frequency = 16000, //采样率
	.frame_size = 512,		   //窗长
	.frame_shift = AUDIO_CAP_POINT_NUM_PER_FRM,		   //帧移
	.fft_frm_size = 512,	   //fft输入有效长度的点数
	.fft_size = 257,		   //fft的正频分量点数-1+直流分量点数
	.result_out_channel = 1,   // 输出通道数result_out_channel;
	.psd_compute_channel_num = 0,//计算mic的psd
	.time_pre_emphasis_enable = false,
	.downsampled_enable = false, 
	.fe_psd_enable = false		 //是否计算psd，以计算特征
#else
	.sample_frequency = 32000, //采样率
	.frame_size = 1024,		   //窗长
	.frame_shift = AUDIO_CAP_POINT_NUM_PER_FRM * 2,		   //帧移
	.fft_frm_size = 1024,	   //fft输入有效长度的点数
	.fft_size = 513,		   //fft的正频分量点数-1+直流分量点数
	#if DEEP_SEPARATE_ENABLE
	.result_out_channel = 2,   // 输出通道数result_out_channel;
	.psd_compute_channel_num = 1,//计算原mic的psd
	#else
	.result_out_channel = 1,   // 输出通道数result_out_channel;
	.psd_compute_channel_num = 0,//计算原mic的psd
	#endif
	.time_pre_emphasis_enable = false,
	.downsampled_enable = true, 
	#if DEEP_SEPARATE_ENABLE
	.fe_psd_enable = true		//是否计算psd，以计算特征
	#else
	.fe_psd_enable = false		//是否计算psd，以计算特征
	#endif
#endif
};
//denoise模块配置
const denoise_config_t denoise_config =
{
	.alg_enable = true,
	.start_Hz = 0,			  //降噪起始频率 单位Hz
	.end_Hz = 8000,			  //降噪结束频率 单位Hz
	.fre_resolution = 31.25f, //频率分辨率 单位Hz
	.aggr_mode = 1,//算法处理的效果等级:0，1，2，处理效果依次增强，失真也会变大
	.set_denoise_threshold = 7000.0f,	  //默认帧平均幅值>=7000起效
	.set_denoise_thr_window_size = 20 //门限判断窗长
};

//就近唤醒模块配置
pwk_config_t ci_pwk_config = 
{
	.alg_enable = true
};

//ai_denoise模块配置
denoise_nn_config_t ai_denoise_config = 
{
	.alg_enable = true,
	.alpha_forget = 0.75f,
	.denoise_mode = 0
};

//哭声检测模块
const sed_config_t sed_config_cry = 
{
	.alg_enable = true,
	.threshold = 0.55f,
	.mode = 1,
	.times = 3
};
//鼾声检测模块
const sed_config_t sed_config_snore = 
{
	.alg_enable = true,
	.threshold = 0.55f,
	.mode = 1,
	.times = 3
};
//doa模块配置
const doa_config_t doa_config =
{
	.alg_enable = true,
	.distance = 40,
	.min_frebin = 40,
	.max_frebin = 130,
	.samplerate = 16000,
	.doa_resolut = 5
};

//ai_doa模块配置
const doa_nn_config_t doa_nn_config = 
{
	.eliminate62_5 = 2            // 消除62.5Hz以下低频噪声影响，0：关闭，2：开启
};

//dr模块配置,startHZ:算法起效的起始频率  endHZ:算法起效的结束频率  范围:0-8KHZ,调大会增加一定的算法力和内存消耗
//160HZ-4800HZ 消耗28KB内存  0-8000HZ 消耗49KB内存
const dereverb_config_t dereverb_config =
{
	.alg_enable = true,
#if DEREVERB_FREQ_RANGE_INDEX
	.startHz = 0.0f,
	.endHz = 8000.0f
#else
    .startHz = 160.0f,
	.endHz = 4800.0f
#endif
};

//bf模块配置
const bf_config_t bf_config =
{
	.alg_enable = true,
	.distance = 40,
	.angle = 90,
	.freq = 20,
	.frame_wkup = 100,
	.frame_rt = 40,
	.wkup_result_thr = 45,     //ASR分值的阈值，值设置的越大，误识会降低(肯能也会影响识别)，根据具体项目去调整
	.bf_deepse_mode = BF_DEEPSE_MODE,
	.bf_asr_valid_mode = BF_ASR_VALID_MODE
};
// aec模块配置
const aec_config_t aec_config =
{
	.alg_enable = true,
	#if (USE_BEAMFORMING_MODULE && BF_DEEPSE_MODE)
	.mic_channel_num = 2,						   //麦克风信号通道数
	#else
	.mic_channel_num = 1,
	#endif
	.ref_channel_num = 1,						   //参考信号通道数
	.aec_control_mode = ENABLE_PLAYING_STATE_MODE,     //ENABLE_PLAYING_STATE_MODE:根据播报状态进行aec控制;  COMPUTE_REF_AMPL_MODE:根据参考幅值大小进行aec控制
	.aec_gain = 1.0f,							   //增益数值
	.aec_enable_threshold = 2000.0f,	           //参考信号判断门限值
    .aec_mic_div_ref_thr = 0.0f,    
	.nlp_flag = 2,
	.aggr_mode = 1,
	.fft_size = 256,	   //频域处理频点数
	.alc_off_codec_adc_gain_mic = 20, 
    .alc_off_codec_adc_gain_ref = 0
};



//.module_config域配置为NULL表示关闭此项。
ci_ssp_config_t ci_ssp = {
	#if USE_ALC_AUTO_SWITCH_MODULE
	.alc_auto_switch = 	{.module_config = &alc_auto_switch_config},
	#else
	.alc_auto_switch = 	{.module_config = NULL},
	#endif

	.stft = 			{.module_config = &stft_istft_config},

	#if USE_AEC_MODULE
	.aec = 				{.module_config = &aec_config},
	#else
	.aec = 				{.module_config = NULL},
	#endif

	#if USE_AI_DOA
	.doa = 				{.module_config = &doa_nn_config},
	#else
	.doa = 				{.module_config = NULL},
	#endif

	#if USE_DEREVERB_MODULE
	.dereverb = 		{.module_config = &dereverb_config},
	#else
	.dereverb = 		{.module_config = NULL},
	#endif

	#if USE_BEAMFORMING_MODULE
	.bf = 				{.module_config = &bf_config},
	#else
	.bf = 				{.module_config = NULL},
	#endif

	#if USE_DENOISE_NN
	.ai_denoise = 		{.module_config = &ai_denoise_config},
	#else
	.ai_denoise = 		{.module_config = NULL},
	#endif

	#if USE_PWK
	.pwk =              {.module_config = &ci_pwk_config}, 
	#else
	.pwk = 				{.module_config = NULL},
	#endif
	
	#if USE_DENOISE_MODULE
	.denoise = 			{.module_config = &denoise_config},
	#else
	.denoise = 			{.module_config = NULL},
	#endif

	.istft = 			{.module_config = &stft_istft_config},
	.iis_out_audio = 	{.module_config = &iis_out_audio_config},

	#if USE_SED_CRY
	.sed = 				{.module_config = &sed_config_cry},
	#elif USE_SED_SNORE
	.sed = 				{.module_config = &sed_config_snore},
	#else
	.sed = 			{.module_config = NULL},
	#endif

};


audio_capture_t audio_capture =
{
    #if INNER_CODEC_AUDIO_IN_USE_RESAMPLE
    .frame_length = AUDIO_CAP_POINT_NUM_PER_FRM * 2, // frame_length;32k采样，输入点数为320
    #else
    .frame_length = AUDIO_CAP_POINT_NUM_PER_FRM, // frame_length;16k采样，输入点数为160
    #endif

    #if USE_BEAMFORMING_MODULE  || USE_AI_DOA || USE_DEREVERB_MODULE || USE_DUAL_MIC_ANY
    .mic_channel_num = 2, // mic_channel_num;
    #else
    .mic_channel_num = 1,                        // mic_channel_num;
    #endif

    #if USE_AEC_MODULE
    .ref_channel_num = 1,                        //ref_channel_num;
    #else
    .ref_channel_num = 0,                        //ref_channel_num;
    #endif
    // MICL // mic_model;
};


