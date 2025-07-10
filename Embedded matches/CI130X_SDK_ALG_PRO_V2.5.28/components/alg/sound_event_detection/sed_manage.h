

#ifndef __SED_MANAGE_H
#define __SED_MANAGE_H

#include <stdint.h>
#include <stdbool.h>
typedef enum
{
    SED_CMD_VAD_START = 0,
    SED_CMD_CMPT,
    SED_CMD_VAD_END,
}sed_cmd_type_t;


typedef struct
{
    sed_cmd_type_t cmd;
    uint32_t cmpt_addr;
}sed_manage_info_t;

typedef struct
{
    bool alg_enable;
    //初始化参数
    float threshold;       //设置判断阈值
    int mode;             //设置声音检测类型
    int times;            //设置音频检测次数
}sed_config_t;

void send_vad_start_msg_to_sed_manage(void);
void send_cmpt_msg_to_sed_manage(uint32_t addr);
void send_vad_end_msg_to_sed_manage(void);

void sed_manage_task(void *p);


#endif


