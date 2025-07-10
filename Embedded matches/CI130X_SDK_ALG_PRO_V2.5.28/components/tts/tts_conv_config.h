/**
  ******************************************************************************
  * @file    audio_in_manage.h
  * @version V1.0.0
  * @date    2019.04.04
  * @brief
  ******************************************************************************
  */

#ifndef __TTS_CONV_CONFIG_H_
#define __TTS_CONV_CONFIG_H_
#include "tts_dnn.h"
#include "tts_conv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACU_MODEL_CRC -768
#define DUR_MODEL_CRC -776
#define RES_BLOCK      1
#define IN_PUT_LEN     8
dnn_tts_config_t dnn_tts_acu_layer0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 16,     
  .pack_len = 28,   
	.scale_cnt = 20,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer0_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_acu_layer2_0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer2_0_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_acu_layer2_1_conv =
{
	.dim_toal_len = 32,   
	.dim_len = 32,     
  .pack_len = 32,   
	.scale_cnt = 1,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer2_1_bn =
{          
  .bn_len = 256,        
};

dnn_tts_config_t dnn_tts_acu_layer2_2_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer2_2_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_acu_layer3_0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer3_0_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_acu_layer3_1_conv =
{
	.dim_toal_len = 32,   
	.dim_len = 32,     
  .pack_len = 32,   
	.scale_cnt = 1,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer3_1_bn =
{          
  .bn_len = 256,        
};

dnn_tts_config_t dnn_tts_acu_layer3_2_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer3_2_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_acu_layer4_0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer4_0_bn =
{          
  .bn_len = 1024,        
};

dnn_tts_config_t dnn_tts_acu_layer4_1_conv =
{
	.dim_toal_len = 32,   
	.dim_len = 32,     
  .pack_len = 32,   
	.scale_cnt = 1,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer4_1_bn =
{          
  .bn_len = 256,        
};

dnn_tts_config_t dnn_tts_acu_layer4_2_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_acu_layer4_2_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_acu_input_conv =
{
	.dim_toal_len = 192,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 6,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};

dnn_tts_config_t dnn_tts_acu_output_conv =
{
	.dim_toal_len = 186,   
	.dim_len = 48,     
  .pack_len = 32,   
	.scale_cnt = 6,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};


dnn_tts_config_t dnn_tts_dur_layer0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 16,     
  .pack_len = 28,   
	.scale_cnt = 20,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer0_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_dur_layer2_0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer2_0_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_dur_layer2_1_conv =
{
	.dim_toal_len = 32,   
	.dim_len = 32,     
  .pack_len = 32,   
	.scale_cnt = 1,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer2_1_bn =
{          
  .bn_len = 256,        
};

dnn_tts_config_t dnn_tts_dur_layer2_2_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer2_2_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_dur_layer3_0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer3_0_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_dur_layer3_1_conv =
{
	.dim_toal_len = 32,   
	.dim_len = 32,     
  .pack_len = 32,   
	.scale_cnt = 1,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer3_1_bn =
{          
  .bn_len = 256,        
};

dnn_tts_config_t dnn_tts_dur_layer3_2_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer3_2_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_dur_layer4_0_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer4_0_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_dur_layer4_1_conv =
{
	.dim_toal_len = 32,   
	.dim_len = 32,     
  .pack_len = 32,   
	.scale_cnt = 1,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer4_1_bn =
{          
  .bn_len = 256,        
};

dnn_tts_config_t dnn_tts_dur_layer4_2_conv =
{
	.dim_toal_len = 64,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 2,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};
dnn_bn_tts_config_t dnn_tts_dur_layer4_2_bn =
{          
  .bn_len = 512,        
};

dnn_tts_config_t dnn_tts_dur_input_conv =
{
	.dim_toal_len = 192,   
	.dim_len = 64,     
  .pack_len = 32,   
	.scale_cnt = 6,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};

dnn_tts_config_t dnn_tts_dur_output_conv =
{
	.dim_toal_len = 1,   
	.dim_len = 1,     
  .pack_len = 32,   
	.scale_cnt = 6,      
  .x_input = IN_PUT_LEN,           
  .x_dqt[0] = DQT_FAETURE,       
	.x_dqt[1] = 1.0f, 
};

#ifdef __cplusplus
}
#endif

#endif
