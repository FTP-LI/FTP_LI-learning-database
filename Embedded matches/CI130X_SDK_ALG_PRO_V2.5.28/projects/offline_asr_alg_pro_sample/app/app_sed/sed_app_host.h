#include <stdint.h>
#include <stdbool.h>
typedef struct
{
    float threshold;       //设置判断阈值
    int times;            //设置音频检测次数,最大次数为5
}sed_param_config_t;

typedef enum
{
    SED_DETECTED_CRY = 10010,   // 检测到哭声
    SED_DETECTED_SNORE,         // 检测到鼾声
}sed_detected_rst_t;

#ifdef __cplusplus
extern "C" {
#endif
  
/**
 * @brief   根据连续n（n<=5）次音频的预测概率值计算检测结果
 * @param out_array 被检测音频的模型预测概率值，范围[0,1]; 
 * @return bool值，返回true说明检测到目标声音，返回false说明未检测到目标声音;
 */
bool sed_continuous_detect(float out_array);

/**
 * @brief  获取模型预测概率值，调用sed_continuous_detect()计算最终检测结果
 * @param data 声音事件检测模型的输出数组，data[0]为目标声音的预测概率值; 
 * @return 无返回值，可根据最终检测结果进行二次开发;
 */
void sed_rslt_cb(void* data);

    


/**
 * @brief IO 输出初始化
 */
void io_out_init(void);
void sed_set_vol_init(void);
void sed_play_welcome_prompt(void);

#ifdef __cplusplus
}
#endif  