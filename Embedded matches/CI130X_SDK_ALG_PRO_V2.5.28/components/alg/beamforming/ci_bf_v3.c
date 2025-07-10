
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "ci_bf.h"
#include "ci_basic_alg.h"
#include "ci_audio_wrapfft.h"
#include "status_share.h"
#include "user_config.h"
#include "debug_time_consuming.h"

extern void *ci_algbuf_malloc(size_t size_in_byte);
extern void *ci_algbuf_calloc(size_t size_in_byte, size_t size_of_byte);
extern void *bf_buffer_malloc(size_t size_of_byte);
extern void *bf_buffer_calloc(size_t size_in_byte, size_t size_of_byte);

#define		  MICRO_NUM	           (2)
#define		  FFTSize		           (512)
#define     FFTShift             (256)
#define		  ACCURACY             (1E-6f)	
#define     STABLE_EPS           (1E-3f)
#define     BF_FORGET            (0.996f)
#define     VAR_BIAS             (0.005f) 
#define     BF_DIAGLOAD          (1E-6f)	
#define     CI_BF_VERSION        (30021)
#define     SPEECH_PDF_DETECTION   (0)

extern void* ci_denoise_for_bf_create(float start_Hz,float end_Hz);
extern float ci_denoise_for_bf_deal( void *denoise_bf,float *fft_in,float *fft_out);
static void *denoise_bf_l =NULL;
static void *denoise_bf_r =NULL;

static inline float ci_sqrt_f32(float x)
{
    register float y;
    asm volatile("fsqrt.s %0,%1"
                 : "=f"(y)
                 : "f"(x));
    return y;
}


static inline void *ci_bf_malloc(size_t size_in_byte)
{
  void* ret = 0;
  #if ON_LINE_SUPPORT
  ret = bf_buffer_malloc(size_in_byte);
  #else
  ret = ci_algbuf_malloc(size_in_byte);
  #endif
  return ret;
}


static inline void *ci_bf_calloc(size_t size_in_byte, size_t size_of_byte)
{
	void* ret = 0;
	#if ON_LINE_SUPPORT
  ret = bf_buffer_calloc(size_in_byte,size_of_byte);
	#else
	ret = ci_algbuf_calloc(size_in_byte,size_of_byte);
	#endif
	return ret;
}

int ci_bf_version( void )
{
  return  CI_BF_VERSION;
}
typedef struct
{
    Complex(*w_demix)[4];
    Complex(*C1_weight)[4];
    Complex(*C2_weight)[4];
    float** bss_out;
    float* ci_denoise_l;
    float* ci_denoise_r;
    float* Prob1;
    float* Prob2;
    float* Power_sum1;
    float* Power_sum2;  
    int frame_count;
    int channel_flag;
    int freq_low;
    int frame_length1;
    int frame_length2;
    int wkup_result_thr;
    int ch_choice;
}IvaParer;

static inline float xpsd(Complex a)
{
    float x, y, r;
    x = a.real * a.real;
    y = a.image * a.image;
    r = x + y;
    return r;
}

static inline Complex Add1(Complex a, Complex b)
{
    register  Complex r;
    r.real = a.real + b.real;
    r.image = a.image + b.image;
    return r;
}

static inline Complex Minus1(Complex a, Complex b)
{
    register  Complex r;
    r.real = a.real - b.real;
    r.image = a.image - b.image;
    return r;
}

static inline Complex Multi1(Complex a, Complex b)
{
    register Complex r;
    r.real = a.real * b.real - a.image * b.image;
    r.image = a.real * b.image + a.image * b.real;
    return r;
}

static  inline Complex Multi1_conj(Complex a, Complex b)
{
    register Complex r;
    r.real = a.real * b.real + a.image * b.image;
    r.image =  a.image * b.real - a.real * b.image ;
    return r;
}

static inline Complex Multi3(Complex a, float b)
{
    register  Complex r;
    r.real = a.real * b;
    r.image = a.image * b;
    return r;
}

static Complex Div1(Complex a, Complex b)
{
    register Complex r;
    float tmp;
    float tmp1;
    tmp = xpsd(b);// b.real* b.real + b.image * b.image;
    tmp1 = 1.0f / (tmp);// +ACCURACY);
    r = Multi1_conj(a, b);
    r.real = r.real * tmp1;
    r.image = r.image * tmp1;
    return r;
}

static inline Complex Div3(Complex a, float b)
{
    register Complex r;
    register float tmp;
    tmp = 1.0f / (b + ACCURACY);
    r.real = a.real * tmp;
    r.image = a.image * tmp;
    return r;
}

ci_ss_wakeup_state_t pre_wakeup_state = CI_SS_NO_WAKEUP;
ci_ss_wakeup_state_t now_wakeup_state = CI_SS_NO_WAKEUP;
bf_config_t *g_bf_config;

void* ci_bf_create(void* module_config)
{
    #if ON_LINE_SUPPORT
    bf_buffer_init();
    #endif
    
    IvaParer* pIvaParer = (IvaParer*)ci_bf_malloc(sizeof(IvaParer));
    bf_config_t* bf_config = (bf_config_t*)module_config;
    ciss_set(CI_SS_CMD_ASR_CHA_NUM, 2);
    g_bf_config = (bf_config_t*)module_config;
    int distance = bf_config->distance;
    int angle = bf_config->angle;
    pIvaParer->freq_low = bf_config->freq;   
    pIvaParer->frame_length1 = bf_config->frame_wkup;
    pIvaParer->frame_length2 = bf_config->frame_rt;
    pIvaParer->wkup_result_thr = bf_config->wkup_result_thr;
    pIvaParer->frame_count = 0;
    pIvaParer->channel_flag = 0;
    pIvaParer->ch_choice = 0;
    
    Complex(*w_demix)[4] = (Complex(*)[4])ci_bf_calloc(FFTShift * 4,sizeof(Complex));
    Complex(*C1_weight)[4] = (Complex(*)[4])ci_bf_calloc(FFTShift * 4, sizeof(Complex));
    Complex(*C2_weight)[4] = (Complex(*)[4])ci_bf_calloc(FFTShift * 4, sizeof(Complex)); 
    pIvaParer->bss_out = (float**)ci_bf_calloc(MICRO_NUM, sizeof(float*));

    for (int i = 0; i < FFTShift; i++)
    {
      w_demix[i][0].real = 1.0f;
      w_demix[i][0].image = 0.0f;
      w_demix[i][1].real = 0.0f;
      w_demix[i][1].image = 0.0f;
      w_demix[i][2].real = 0.0f;
      w_demix[i][2].image = 0.0f;
      w_demix[i][3].real = 1.0f;
      w_demix[i][3].image = 0.0f;
      
      C1_weight[i][0].real = STABLE_EPS ;
      C1_weight[i][0].image = 0.0f ;
      C1_weight[i][1].real = 0.0f ;
      C1_weight[i][1].image = 0.0f ;
      C1_weight[i][2].real = 0.0f ;
      C1_weight[i][2].image = 0.0f ;
      C1_weight[i][3].real = STABLE_EPS;
      C1_weight[i][3].image = 0.0f;
      
      C2_weight[i][0].real = STABLE_EPS ;
      C2_weight[i][0].image = 0.0f ;
      C2_weight[i][1].real = 0.0f ;
      C2_weight[i][1].image = 0.0f ;
      C2_weight[i][2].real = 0.0f ;
      C2_weight[i][2].image = 0.0f ;
      C2_weight[i][3].real = STABLE_EPS;
      C2_weight[i][3].image = 0.0f;
    }
    pIvaParer->w_demix = w_demix;
    pIvaParer->C1_weight = C1_weight;
    pIvaParer->C2_weight = C2_weight;
    
    for (int i = 0; i < MICRO_NUM; i++)
    {
      pIvaParer->bss_out[i] = (float*)ci_bf_calloc(FFTSize, sizeof(float));
    }
	
    if(bf_config->bf_deepse_mode == 0)
    {
      pIvaParer->Prob1 = (float*)ci_bf_calloc(pIvaParer->frame_length1, sizeof(float));
      pIvaParer->Prob2 = (float*)ci_bf_calloc(pIvaParer->frame_length1, sizeof(float));
      pIvaParer->Power_sum1 = (float*)ci_bf_calloc(pIvaParer->frame_length1, sizeof(float));
      pIvaParer->Power_sum2 = (float*)ci_bf_calloc(pIvaParer->frame_length1, sizeof(float));
      denoise_bf_l = ci_denoise_for_bf_create(100,2000);
      denoise_bf_r = ci_denoise_for_bf_create(100,2000);
      //mprintf("bf_config->bf_deepse_mode = %d\n",bf_config->bf_deepse_mode);
    }
    return pIvaParer;
}

static Complex eye_matrix0 = { BF_DIAGLOAD * 1.0f,0.0f };
static Complex eye_matrix1 = { 0.0f,0.0f };
static Complex eye_matrix2 = { 0.0f,0.0f };
static Complex eye_matrix3 = { BF_DIAGLOAD * 1.0f,0.0f }; 

#pragma GCC optimize("O3")
void BlindSourceSeparation (IvaParer* handle,float *leftbuff,float *rightbuff)
{
    IvaParer* pIvaParer = (IvaParer*)handle;

    Complex* complex_left_buff = (Complex*)leftbuff;
    Complex* complex_right_buff = (Complex*)rightbuff;

    Complex(* w_demix_t)[4] = pIvaParer->w_demix;
    Complex(*C1_weight_t)[4] = pIvaParer->C1_weight;
    Complex(*C2_weight_t)[4] = pIvaParer->C2_weight;
    
    Complex H_trans[MICRO_NUM * MICRO_NUM];
    Complex D_matrix[MICRO_NUM * MICRO_NUM];
    Complex Inverse_D[MICRO_NUM * MICRO_NUM];
    Complex Yt[2];
    Complex Corr[MICRO_NUM * MICRO_NUM];
    Complex Corr_tmp[MICRO_NUM * MICRO_NUM];
    Complex tmp1, tmp2, tmp3, tmp4;
    Complex a1_weight;
    Complex a2_weight;

    float tmpf1;
    float tmpf2;
    float phi1 = 0.0f;
    float phi2 = 0.0f;

    for (int i = 0; i < FFTShift; i++)
    {      
        Complex X0 = complex_left_buff[i];
        Complex X1 = complex_right_buff[i];
        float eng = X0.real * X0.real + X0.image * X0.image;       
        if (eng < 100.0f)
        {
          continue;
        }
        
        X0 = Div3(X0 , 32767.0f);
        X1 = Div3(X1 , 32767.0f);
        
        tmp1 = Multi1(w_demix_t[i][0], X0);
        tmp2 = Multi1(w_demix_t[i][1], X1);
        tmp3 = Multi1(w_demix_t[i][2], X0);
        tmp4 = Multi1(w_demix_t[i][3], X1);

        Yt[0] = Add1(tmp1, tmp2);
        Yt[1] = Add1(tmp3, tmp4);
        
        pIvaParer->bss_out[0][i * MICRO_NUM + 0] = Yt[0].real; //*32767.0f;
        pIvaParer->bss_out[0][i * MICRO_NUM + 1] = Yt[0].image;// *32767.0f;

        pIvaParer->bss_out[1][i * MICRO_NUM + 0] = Yt[1].real; //*32767.0f;
        pIvaParer->bss_out[1][i * MICRO_NUM + 1] = Yt[1].image; //*32767.0f;

        tmpf1 = xpsd(Yt[0]);
        tmpf2 = xpsd(Yt[1]);
        phi1 += tmpf1;
        phi2 += tmpf2;
        
    }

    phi1 = (1 - BF_FORGET) * ci_sqrt_f32(1 / (phi1 + VAR_BIAS));
    phi2 = (1 - BF_FORGET) * ci_sqrt_f32(1 / (phi2 + VAR_BIAS));
    
    for (int i = 0; i < FFTShift; i++)
    {
        Complex X0 = complex_left_buff[i];
        Complex X1 = complex_right_buff[i];
        float eng = X0.real * X0.real + X0.image * X0.image;
        
        if (eng < 100.0f)
        {
          continue;
        }
        X0 = Div3(X0 , 32767.0f);
        X1 = Div3(X1 , 32767.0f);
        
        Corr[0] = Multi1_conj(X0, X0);
        Corr[1] = Multi1_conj(X0, X1);
        Corr[2] = Multi1_conj(X1, X0);
        Corr[3] = Multi1_conj(X1, X1);

        Corr_tmp[0] = Multi3(Corr[0], phi1);
        Corr_tmp[1] = Multi3(Corr[1], phi1);
        Corr_tmp[2] = Multi3(Corr[2], phi1);
        Corr_tmp[3] = Multi3(Corr[3], phi1);

        tmp1 = Multi3(C1_weight_t[i][0], BF_FORGET);
        tmp2 = Multi3(C1_weight_t[i][1], BF_FORGET);
        tmp3 = Multi3(C1_weight_t[i][2], BF_FORGET);
        tmp4 = Multi3(C1_weight_t[i][3], BF_FORGET);

        C1_weight_t[i][0] = Add1(tmp1, Corr_tmp[0]);
        C1_weight_t[i][1] = Add1(tmp2, Corr_tmp[1]);
        C1_weight_t[i][2] = Add1(tmp3, Corr_tmp[2]);
        C1_weight_t[i][3] = Add1(tmp4, Corr_tmp[3]);

        Corr_tmp[0] = Multi3(Corr[0], phi2);
        Corr_tmp[1] = Multi3(Corr[1], phi2);
        Corr_tmp[2] = Multi3(Corr[2], phi2);
        Corr_tmp[3] = Multi3(Corr[3], phi2);

        tmp1 = Multi3(C2_weight_t[i][0], BF_FORGET);
        tmp2 = Multi3(C2_weight_t[i][1], BF_FORGET);
        tmp3 = Multi3(C2_weight_t[i][2], BF_FORGET);
        tmp4 = Multi3(C2_weight_t[i][3], BF_FORGET);

        C2_weight_t[i][0] = Add1(tmp1, Corr_tmp[0]);
        C2_weight_t[i][1] = Add1(tmp2, Corr_tmp[1]);
        C2_weight_t[i][2] = Add1(tmp3, Corr_tmp[2]);
        C2_weight_t[i][3] = Add1(tmp4, Corr_tmp[3]);

        //   update demixing matrix
        tmp1 = Multi1(w_demix_t[i][0], C1_weight_t[i][0]);
        tmp2 = Multi1(w_demix_t[i][1], C1_weight_t[i][2]);
        H_trans[0] = Add1(tmp1, tmp2);

        tmp1 = Multi1(w_demix_t[i][0], C1_weight_t[i][1]);
        tmp2 = Multi1(w_demix_t[i][1], C1_weight_t[i][3]);
        H_trans[1] = Add1(tmp1, tmp2);

        tmp1 = Multi1(w_demix_t[i][2], C1_weight_t[i][0]);
        tmp2 = Multi1(w_demix_t[i][3], C1_weight_t[i][2]);
        H_trans[2] = Add1(tmp1, tmp2);

        tmp1 = Multi1(w_demix_t[i][2], C1_weight_t[i][1]);
        tmp2 = Multi1(w_demix_t[i][3], C1_weight_t[i][3]);
        H_trans[3] = Add1(tmp1, tmp2);

        H_trans[0] = Add1(H_trans[0], eye_matrix0);
        H_trans[1] = Add1(H_trans[1], eye_matrix1);
        H_trans[2] = Add1(H_trans[2], eye_matrix2);
        H_trans[3] = Add1(H_trans[3], eye_matrix3);

        tmp1 = Multi1(H_trans[0], H_trans[3]);
        tmp2 = Multi1(H_trans[1], H_trans[2]);
        tmp3 = Minus1(tmp1, tmp2);
        tmp1 = Div1(H_trans[3], tmp3);
        tmp2 = Div1(H_trans[2], tmp3);
 
        D_matrix[0].real = tmp1.real;
        D_matrix[0].image = -tmp1.image;
        D_matrix[1].real = -tmp2.real;
        D_matrix[1].image = tmp2.image;

        tmp1 = Multi1(w_demix_t[i][0], C2_weight_t[i][0]);
        tmp2 = Multi1(w_demix_t[i][1], C2_weight_t[i][2]);
        H_trans[0] = Add1(tmp1, tmp2);

        tmp1 = Multi1(w_demix_t[i][0], C2_weight_t[i][1]);
        tmp2 = Multi1(w_demix_t[i][1], C2_weight_t[i][3]);
        H_trans[1] = Add1(tmp1, tmp2);

        tmp1 = Multi1(w_demix_t[i][2], C2_weight_t[i][0]);
        tmp2 = Multi1(w_demix_t[i][3], C2_weight_t[i][2]);
        H_trans[2] = Add1(tmp1, tmp2);

        tmp1 = Multi1(w_demix_t[i][2], C2_weight_t[i][1]);
        tmp2 = Multi1(w_demix_t[i][3], C2_weight_t[i][3]);
        H_trans[3] = Add1(tmp1, tmp2);

        H_trans[0] = Add1(H_trans[0], eye_matrix0);
        H_trans[1] = Add1(H_trans[1], eye_matrix1);
        H_trans[2] = Add1(H_trans[2], eye_matrix2);
        H_trans[3] = Add1(H_trans[3], eye_matrix3);

        tmp1 = Multi1(H_trans[0], H_trans[3]);
        tmp2 = Multi1(H_trans[1], H_trans[2]);
        tmp3 = Minus1(tmp1, tmp2);
        tmp1 = Div1(H_trans[1], tmp3);
        tmp2 = Div1(H_trans[0], tmp3);

        D_matrix[2].real = -tmp1.real;
        D_matrix[2].image = tmp1.image;
        D_matrix[3].real = tmp2.real;
        D_matrix[3].image = -tmp2.image;

        tmp1 = Multi1(D_matrix[0], D_matrix[3]);
        tmp2 = Multi1(D_matrix[1], D_matrix[2]);
        tmp3 = Minus1(tmp1, tmp2);

        Inverse_D[0] = Div1(D_matrix[3], tmp3);
        Inverse_D[1] = Div1(D_matrix[1], tmp3);
        Inverse_D[2] = Div1(D_matrix[2], tmp3);
        Inverse_D[3] = Div1(D_matrix[0], tmp3);

        tmpf1 = xpsd(Inverse_D[0]);
        tmpf2 = xpsd(Inverse_D[2]);

        if (tmpf1 > tmpf2)
        {
            a1_weight.real = Inverse_D[0].real;
            a1_weight.image = Inverse_D[0].image;
        }
        else
        {
            a1_weight.real = Inverse_D[2].real;
            a1_weight.image = Inverse_D[2].image;
        }
        tmpf1 = xpsd(Inverse_D[3]);
        tmpf2 = xpsd(Inverse_D[1]);

        if (tmpf1 > tmpf2)
        {
            a2_weight.real = Inverse_D[3].real;
            a2_weight.image = Inverse_D[3].image;
        }
        else
        {
            a2_weight.real = Inverse_D[1].real;
            a2_weight.image = Inverse_D[1].image;
        }

        w_demix_t[i][0] = Multi1(a1_weight, D_matrix[0]);
        w_demix_t[i][1] = Multi1(a1_weight, D_matrix[1]);
        w_demix_t[i][2] = Multi1(a2_weight, D_matrix[2]);
        w_demix_t[i][3] = Multi1(a2_weight, D_matrix[3]);
    }
}
float power_sum1[100]={0.0f};
float power_sum2[100]={0.0f};
bool Detected_asr_Valid(float** bss_out,int asr_thr)
{
  float power1 = 0.0f;
  float power2 = 0.0f;
  float power1_mean = 0.0f;
  float power2_mean = 0.0f;
  float power_diff = 0.0f;
  float power_A,power_B;
  int result_A,result_B;
  static int pow_index=0;
  bool asr_valid = true;
  int result_ch0 = ciss_get(CI_SS_CMD_SCORE_CHA0);
  int result_ch1 = ciss_get(CI_SS_CMD_SCORE_CHA1);
  status_t wkup_cmd_state = ciss_get(CI_SS_CMD_STATE_FOR_BSS);

  for (int i = 0;i < FFTSize;i++)
  {
    float tmpf1 = bss_out[0][i] * bss_out[0][i];
    float tmpf2 = bss_out[1][i] * bss_out[1][i];
    power1 += tmpf1;
    power2 += tmpf2;
  }
  power_sum1[pow_index] = power1;
  power_sum2[pow_index] = power2;
  pow_index++;
  if(pow_index==100)
  {
    pow_index=0;
  }

  if(wkup_cmd_state == 1 && (result_ch0 == 0 || result_ch1 ==0))
  {
    
    for (int i = 0; i < 100; i++)
    {
        power1_mean += power_sum1[i];
        power2_mean += power_sum2[i];           
    }

    if(result_ch0 >0 && result_ch1==0)
    {
        result_A = result_ch0;
        result_B = result_ch1;
        power_A = power1_mean;
        power_B = power2_mean;
    }
    else if (result_ch0 ==0 && result_ch1 > 0)
    {
        result_A = result_ch1;
        result_B = result_ch0;
        power_A = power2_mean;
        power_B = power1_mean;
    } 
    power_diff = power_A / power_B;
    if(result_A <= asr_thr)
    {
      if( power_diff < 2.0f)
      {
        asr_valid = false;
      }
    }
    else if (result_A > asr_thr && result_A <= (asr_thr+5))
    {
      if( power_diff < 1.2f)
      {
        asr_valid = false;
      }
    }
    else if (result_A > (asr_thr+10) && result_A <= (asr_thr+15))
    {
      if( power_diff < 0.5f)
      {
        asr_valid = false;
      }
    }
    if (result_A > 0 && result_B == 0)
    {
      if(power_diff < 0.35f)
      {
        asr_valid = false;
      }
    }
  }
  ciss_set(CI_SS_CMD_STATE_VALID,asr_valid);
  ciss_set(CI_SS_CMD_STATE_FOR_BSS,0);
  return asr_valid;
}

void Channel_Choice_Dualasr(IvaParer* handle)
{
    IvaParer* pIvaParer = (IvaParer*)handle;

    float prob1_mean = 0.0f;
    float prob2_mean = 0.0f;
    float power1_mean = 0.0f;
    float power2_mean = 0.0f;
    float delta = 0.0f;
    float delta1 = 0.0f;  

    int frame_length1 = pIvaParer->frame_length1;
    int frame_length2 = pIvaParer->frame_length2;
    int sum_frame  = frame_length1 - frame_length2;   
    int frame_count = pIvaParer->frame_count;
    //int channel_flag;

    int frame_cmd = frame_count + sum_frame;
    for (int i = frame_count; i < frame_cmd; i++)
    {
        int index = i % frame_length1;
        prob1_mean += pIvaParer->Prob1[index];
        prob2_mean += pIvaParer->Prob2[index];
        power1_mean += pIvaParer->Power_sum1[index];
        power2_mean += pIvaParer->Power_sum2[index];           
    }

    prob1_mean = prob1_mean / sum_frame;
    prob2_mean = prob2_mean / sum_frame;
    power1_mean = power1_mean / sum_frame;
    power2_mean = power2_mean / sum_frame;
    
    delta = power2_mean / power1_mean;
    delta1 = prob2_mean - prob1_mean;
    
    //mprintf("###delta = %d\n",(int)(delta*1000));
    //mprintf("&&&delta1 = %d\n",(int)(delta1*1000)); 
      if(delta > 4.0f)
      {
         pIvaParer->channel_flag = 1;
         pIvaParer->ch_choice = 1;
        
      }
      else if ((1.5f < delta && delta < 4.0f) && delta1 > 0.0f)  
      {
          pIvaParer->channel_flag = 1;
          pIvaParer->ch_choice = 1;
      }
      else if (delta1 > 0.15f)
      {
          pIvaParer->channel_flag = 1; 
          pIvaParer->ch_choice = 1;
      }
      else if (delta < 0.25f)
      {
          pIvaParer->channel_flag =0;
          pIvaParer->ch_choice = 1;

      }
      else if ((0.25f < delta  && delta < 0.67f) && ( delta1 < 0.0f))   
      {
          pIvaParer->channel_flag =0;
          pIvaParer->ch_choice = 1;
      }
      else if(delta1 < -0.15f)
      {
          pIvaParer->channel_flag =0;
          pIvaParer->ch_choice = 1;
      }
      else
      {
          pIvaParer->ch_choice = 0;
      }            
} 

void Target_Choice_Dualasr(IvaParer* handle, float **fftdata_out)
{
    IvaParer* pIvaParer = (IvaParer*)handle;
    status_t wkup_status_ssp = ciss_get(CI_SS_WAKING_UP_STATE_FOR_SSP);
    status_t wkup_status_sys = ciss_get(CI_SS_WAKING_UP_STATE);
    now_wakeup_state = wkup_status_sys;
    short wkup_result_ch0 = ciss_get(CI_SS_CMD_SCORE_CHA0);
    short wkup_result_ch1 = ciss_get(CI_SS_CMD_SCORE_CHA1);
    float prob1;
    float prob2;
    float power1 = 0.0f;
    float power2 = 0.0f;
    bool wkup_valid;
    int freq_low = pIvaParer->freq_low;
    int frame_count = pIvaParer->frame_count;
    int frame_length = pIvaParer->frame_length1;
    int wkup_thr = pIvaParer->wkup_result_thr;

    prob1 = ci_denoise_for_bf_deal(denoise_bf_l,pIvaParer->bss_out[0],NULL);
    prob2 = ci_denoise_for_bf_deal(denoise_bf_r,pIvaParer->bss_out[1],NULL);
       
    pIvaParer->Prob1[frame_count] = prob1;
    pIvaParer->Prob2[frame_count] = prob2;
    
    for (int i = freq_low;i < 128;i++)
    {
      float tmpf1 = pIvaParer->bss_out[0][i] * pIvaParer->bss_out[0][i];
      float tmpf2 = pIvaParer->bss_out[1][i] * pIvaParer->bss_out[1][i];
      power1 += tmpf1;
      power2 += tmpf2;
    }
    pIvaParer->Power_sum1[frame_count] = power1;
    pIvaParer->Power_sum2[frame_count] = power2;
    if(1 == g_bf_config->bf_asr_valid_mode)
    {
      wkup_valid = Detected_asr_Valid(pIvaParer->bss_out,wkup_thr);
    }
    else if (0 == g_bf_config->bf_asr_valid_mode)
    {
      wkup_valid = true;
      ciss_set(CI_SS_CMD_STATE_VALID,1);
      ciss_set(CI_SS_CMD_STATE_FOR_BSS,0);
    }
    
    if ((CI_SS_WAKEUPED == wkup_status_ssp && (wkup_valid == true))) // && (CI_SS_CMD_IS_WAKEUP == wkup_cmd_state) )
    {
      if(pre_wakeup_state != now_wakeup_state)
      {
        
        // mprintf("wkup_ch0 = %d\n",wkup_result_ch0);
        // mprintf("wkup_ch1 = %d\n",wkup_result_ch1);
        int diff_channel = abs(wkup_result_ch0 - wkup_result_ch1);
        if ((wkup_result_ch0 - wkup_result_ch1) >= 20 )
        {
          pIvaParer->channel_flag = 0;   //target == channel0
          pIvaParer->ch_choice = 1;     //dualmic_out
        }
        else if ((wkup_result_ch1 - wkup_result_ch0) >= 20)
        {
          pIvaParer->channel_flag = 1;   //target == channel1
          pIvaParer->ch_choice = 1;     //dualmic_out

        }
        else if ((diff_channel > 5) && (diff_channel < 20))
        {
          Channel_Choice_Dualasr(pIvaParer);
        }
        else
        {   
          pIvaParer->ch_choice = 0;   //orgmic_out
        }
      }
    }
    pre_wakeup_state = now_wakeup_state;

    if ((CI_SS_WAKEUPED == wkup_status_sys))
    {
      if(1 == pIvaParer->ch_choice)
      {
        if (1 == pIvaParer->channel_flag)
        {
          for (int i =0;i < FFTSize;i++)
          {
            fftdata_out[0][i] = pIvaParer->bss_out[1][i] *32767.0f;
            fftdata_out[1][i] = pIvaParer->bss_out[0][i] *32767.0f;
          }
        }

        if (0 == pIvaParer->channel_flag)
        {
            for (int j = 0; j < FFTSize; j++)
            {
              fftdata_out[0][j] = pIvaParer->bss_out[0][j] *32767.0f;
              fftdata_out[1][j] = pIvaParer->bss_out[1][j]*32767.0f ;
            }
        }

      }
      
    }
    else if (CI_SS_NO_WAKEUP == wkup_status_sys)
    {
        for (int i = 0; i < MICRO_NUM; i++)
        {
          for (int j = 0; j < FFTSize; j++)
          {
             fftdata_out[i][j] = pIvaParer->bss_out[i][j]*32767.0f ;
          }
        }
    }
        
    pIvaParer->frame_count++;
    pIvaParer->frame_count = pIvaParer->frame_count % frame_length;  
 
}


int ci_bf_deal(void* handle, float* leftbuff, float* rightbuff, float** fftdata_out)
{
     IvaParer* pIvaParer = (IvaParer*)handle;
     int result_thr = pIvaParer->wkup_result_thr;

     BlindSourceSeparation(pIvaParer,leftbuff,rightbuff);

     if(1 == g_bf_config->bf_deepse_mode)
     {
       if(1 == g_bf_config->bf_asr_valid_mode)
       {
          bool cmd_valid = Detected_asr_Valid(pIvaParer->bss_out,result_thr);
       }
       else if(0 == g_bf_config->bf_asr_valid_mode)
       {
         ciss_set(CI_SS_CMD_STATE_VALID,1);
         ciss_set(CI_SS_CMD_STATE_FOR_BSS,0);
       }

        for (int i = 0; i < MICRO_NUM; i++)
        {
          for (int j = 0; j < FFTSize; j++)
          {
            fftdata_out[i][j] = pIvaParer->bss_out[i][j] *32767.0f;
          }
        }
     }
     else if(0 == g_bf_config->bf_deepse_mode)
     {
       Target_Choice_Dualasr(pIvaParer,fftdata_out);
     }

    return 0;
}
// int channel_choice =0;
// void set_bf_process_state(int ch_choice)
// {
//     channel_choice = ch_choice;
// }
// int get_bf_process_state(void)
// {
//   return channel_choice;
// }




