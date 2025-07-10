
#ifndef CI_DRC_H
#define CI_DRC_H

typedef struct
{
    float attack_time;               //起始时间
    float release_time;             //释放时间
    float threshold;               //阈值dB
    float ratio;                  //压缩比率
    float knee;                  //是否使用拐点
    bool  drc_para_from_uart;   //算法参数是否来自于uart传输
}drc_config_t;

#ifdef __cplusplus
extern "C" {
#endif
  
    /**
     * @brief  获取算法版本信息
     * @return int 版本id, 整数型，例如返回10000, 表示版本号为 1.0.0;
     */
    int ci_drc_version( void );

    /**
     * @brief  初始化当前算法模块
     * @return void*      若返回空NULL，则表示模块创建失败，否则表示创建成功;
     */
    void* ci_drc_create(void* module_config);
	
    /**
     * @brief  drc算法处理函数
     * @param handle 模块句柄，参数为来自ci_drc_create()的返回值; 
     * @return int   返回0表示成功，否则表示失败;
     */
    int ci_drc_deal(void *handle,short *pcm_in,short *pcm_out);

#ifdef __cplusplus
}
#endif    

#endif