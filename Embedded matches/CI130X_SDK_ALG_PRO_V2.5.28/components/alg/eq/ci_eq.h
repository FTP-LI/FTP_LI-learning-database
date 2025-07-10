
#ifndef CI_EQ_H
#define CI_EQ_H

typedef struct
{
    float Gain[9];            //各频段增益
    bool eq_gain_from_uart;    //算法参数是否来自于uart传输
}eq_config_t;

#ifdef __cplusplus
extern "C" {
#endif
  
    /**
     * @brief  获取算法版本信息
     * @return int 版本id, 整数型，例如返回10000, 表示版本号为 1.0.0;
     */
    int ci_eq_version( void );

    /**
     * @brief  初始化当前算法模块
     * @return void*      若返回空NULL，则表示模块创建失败，否则表示创建成功;
     */
    void* ci_eq_create(void* module_config);
	
    /**
     * @brief  eq算法处理函数
     * @param handle 模块句柄，参数为来自ci_eq_create()的返回值; 
     * @return int   返回0表示成功，否则表示失败;
     */
    int ci_eq_deal(void *handle,short *pcm_in,short *pcm_out);

#ifdef __cplusplus
}
#endif    

#endif