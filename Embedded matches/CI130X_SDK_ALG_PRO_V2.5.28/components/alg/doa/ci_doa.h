/**
  ******************************************************************************
  * @file    ci_doa.h
  * @version V1.0.0
  * @date    2019.07.09
  * @brief 
  ******************************************************************************
  **/
#ifndef CI_DOA_H
#define CI_DOA_H
#include "../ci_alg_malloc.h"
/**
 * @addtogroup doa
 * @{
 */

typedef struct
{  
	 bool alg_enable;
    //初始化参数
    int distance;
    int min_frebin;
    int max_frebin;
    int samplerate;
    int doa_resolut;
}doa_config_t;

#ifdef __cplusplus
extern "C" {
#endif


     /**
     * @brief  获取算法版本信息
     * 
     * @return int 版本id, 整数型，例如返回10000, 表示版本号为 1.0.0;
     */
    int ci_doa_version( void );

    /**
     * @brief  初始化当前算法模块
     * 
     * @param module_config    双麦DOA算法配置文件
     * @return void*      若返回空NULL，则表示模块创建失败，否则表示创建成功;
     */
    void* ci_doa_create( void* module_config);
    int ci_doa_deal(void *handle,float **fft_in );
    int ci_get_doa_angle(void);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CI_DOA_H */
