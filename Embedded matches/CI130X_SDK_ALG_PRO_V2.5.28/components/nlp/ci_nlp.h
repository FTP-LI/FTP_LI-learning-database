#ifndef __CI_NLP_H__
#define __CI_NLP_H__

#define NLP_LOG 0    //1，NLP相关打印信息 0 关闭
#define NLP_ENABLE 1 //nlp功能启用 1 启用 0 禁用
#define MAX_SIL_CFD -315
#define MIN_CNT_1 10
#define MIN_CNT_2 5
#define MIN_CNT_L 10
#define NLP_BUFFER_MAX 42          //接收到的识别结果文本最大长度
#define NLP_SAVE_STR_MAX_NUM 8     //保存的字符串数量
#define NLP_TOURN_OFF_FUNC_INDEX 3 //关机功能索引
#define NLP_TEMP_BUF_MAX_LEN 100   //NLP临时数据处理长度

typedef struct{
    char* entity;
    char* intent[4];
    uint8_t flag;
    uint8_t intent_count;
}nlp_data_t;

typedef enum
{
    NLP_STATE_OK,
    NLP_ERR_COMMON,
    NLP_ERR_PARAM_INIT,
} NlpStatus_t;

typedef struct
{
    uint8_t nlpPrintEnable;                  //nlp中间打印结果开关
    uint8_t nlpAsrOutMaxNum;                 //asr输出最大个数
    uint16_t nlpAsrCmdMaxLen;                //asr每个词最大长度
    char **pNlpAsrOutAllCmds;                //保存ASR所有输出的词
    char **pNlpAsrAviableCmds;               //asr输出中值中有效的词
    char *pNlpCmdAviableIndex;               //有效索引
    uint8_t *pNlpCmdOut;                     //每个命令词输出的次数
    int *pNlpCmdCfd;                         //每个命令词打分

    int *pNlpCmdWordsStartFrames;            //每个词的解码开始帧
    int *pNlpCmdWordsFramesLen;              //每个词的解码帧长

    uint8_t *pNlpCmdNodeConfidenceNum;       //每个词节点置信度次数
    uint16_t pNlpAviableCmdConnectStrMaxLen; //有效词拼接最大长度
    char *pNlpAviableCmdConnectStr;
    uint16_t pNlpSplitCmdMaxLen; //分割后每个词的最大长度
    char **pNlpCmdSplitStr;      //连词词后分割后有效词字符串

    char **pNlpSpecialCmdParam1;
    uint8_t pNlpSpecialCmdParam1Len;

    char **pNlpSpecialCmdParam4;
    uint8_t pNlpSpecialCmdParam4Len;

    char **pNlpSpecialCmdParam5;
    uint8_t pNlpSpecialCmdParam5Len;

    char **pNlpSpecialCmdParam5_val;

    char **pNlpSpecialCmdParam6;
    uint8_t pNlpSpecialCmdParam6Len;

    void (*nlp_result_out_func)(nlp_data_t , short , int []);
    void (*nlp_intents_sort_func)(nlp_data_t *); 
    
} NlpParamTypedef;

int getResult();
int set_intent_output(char *cmd_detail, short flag, char *entity);

int asr_result_callback_nlp(char *cmd, int cfd, int decoded_frames, short sli_cfd, short path_node_cfd, short start_frame, short frame_len);
int nlp_callback();
int asr_count_words_to_nlp(char *cmd, int cfd, int decoded_frames, short sli_cfd, short path_node_cfd, short start_frame, short frame_len);
void clean_nlp_keywords();
void nlp_param_set(NlpParamTypedef *pNlpParam);
void ci_sprintf(char *dst, const char *str1, const char *str2, int dst_len);
#endif //__CI_NLP_H__