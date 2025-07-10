/**
 * @file voice_print_recognition.h
 * @brief interface declearation for voice print recognition module
 * @version 1.0
 * @date 2023-03-08
 * 
 * @copyright Copyright (c) 2023 Chipintelli Technology Co., Ltd.
 * 
 */

#ifndef __VOICE_PRINT_WMAN_RECOGNITION_H__
#define __VOICE_PRINT_WMAN_RECOGNITION_H__
/******男女声音检测*******/
#if USE_WMAN_VPR
typedef enum
{
    vpr_reg_successed,          // 注册成功
    vpr_reg_failed,             // 注册失败
    vpr_rec_successed,          // 录入成功(注册一个模板可能需要录入多次)。
    vpr_rec_failed,             // 录入成功(注册一个模板可能需要录入多次)。
}vpr_callback_rst_t;

typedef enum
{
    VPR_REG_MAN = 10012,   // 识别到男声
    VPR_REG_WOMAN,         // 识别到女声
}vpr_reg_rst_t;

typedef void (*vpr_callback_t)(vpr_callback_rst_t rst, int rec_index);

/**
 * @brief 初始化声纹模块
 * @para callback_func 函数指针，用于传入事件回调函数。事件回调函数用于对外通知声纹识别相关事件，比如"模板注册成功"、"模板注册失败"、"声纹匹配成功"。
 * @return 0:初始化成功; 非0:错误号。 
 */
extern int vpr_init(vpr_callback_t callback_func);

/**
 * @brief 开始注册声纹模板
 * @return 0:启动声纹注册成功; -1:启动声纹注册失败。
 */
extern int vpr_start_regist();

/**
 * @brief 停止注册声纹模板
 * @return 0:没有错误; 非0:错误号。
 */
extern int vpr_stop_regist();

/**
 * @brief 删除指定的声纹模板
 * @return 0:没有错误; -1:指定的模板号非有效模板(未注册或者已被删除)。
 */
extern int vpr_delete(int index);

/**
 * @brief 删除所有声纹模板
 * @return 0:没有错误; 非0:错误号。
 */
extern int vpr_clear();

/**
 * @brief 执行一次声纹识别
 * @return 0:没有错误; 非0:错误号。
 */
extern int vpr_run_one_recognition();

#endif
#endif

