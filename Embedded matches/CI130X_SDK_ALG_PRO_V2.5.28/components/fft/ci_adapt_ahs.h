/**
 * @file ci_adapt_ahs.h
 * @brief
 * @version V1.0.0
 * @date 2023.05.27
 *
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 *
 */
#ifndef CI_ADAPT_AHS_H
#define CI_ADAPT_AHS_H

#include <stdbool.h>

 /**
  * @addtogroup ahs
  * @{
  */

typedef struct
{
    float adapt_step;   // 啸叫抑制算法步长

}ahs_config_t;

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief       获得当前版本号
     *
     * @return int  版本号id，例如10000表示1.00.00;
     */
    int ci_adapt_ahs_version(void);

    /**
     * @brief                       创建模块
     * @return void*                NULL:失败;其它:成功.
     */
    void* ci_ahs_create();

    /**
     * @brief           释放模块
     *
     * @param handle    模块句柄
     * @return          void
     */
    void* ci_adapt_ahs_destroy(void* handle);


    /**
     * @brief                   ahs处理模块
     * @param handle            模块句柄;
     * @param mic_psd           mic信号的psd输入
     * @param input             mic信号的时域输入;
     * @param output            ahs处理信号的时域输出;
     * @return int              0: ahs算法处理模块正常运行；其它：ahs算法处理过程出现异常.
     */
    int ci_adapt_ahs_deal(void *handle, float *mic_psd, short *input, short *output);

#ifdef __cplusplus
}
#endif

/** @} */


#endif /* CI_ADAPT_AHS_H */
