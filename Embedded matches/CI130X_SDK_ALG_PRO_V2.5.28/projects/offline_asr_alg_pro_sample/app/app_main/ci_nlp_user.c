#include "ci_nlp_user.h"
#include "FreeRTOS.h" 
#include "status_share.h"


int nlp_param_init(NlpParamTypedef *pNlpPram)
{
    pNlpPram->pNlpAsrOutAllCmds = (char **)pvPortMalloc(sizeof(char *)*pNlpPram->nlpAsrOutMaxNum);
    for (int i = 0; i < pNlpPram->nlpAsrOutMaxNum; i++)
    {
        pNlpPram->pNlpAsrOutAllCmds[i] = (char *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(char));
        if (pNlpPram->pNlpAsrOutAllCmds[i] == NULL)
        {
            mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
            return NLP_ERR_PARAM_INIT;
        }
        memset(pNlpPram->pNlpAsrOutAllCmds[i], 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(char));
    }
    pNlpPram->pNlpAsrAviableCmds = (char **)pvPortMalloc(sizeof(char *)*pNlpPram->nlpAsrOutMaxNum);
    for (int i = 0; i < pNlpPram->nlpAsrOutMaxNum; i++)
    {
        pNlpPram->pNlpAsrAviableCmds[i] = (char *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(char));
        if (pNlpPram->pNlpAsrAviableCmds[i] == NULL)
        {
            mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
            return NLP_ERR_PARAM_INIT;
        }
        memset(pNlpPram->pNlpAsrAviableCmds[i], 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(char));
    }
    pNlpPram->pNlpCmdAviableIndex = (char *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(char));
    if (pNlpPram->pNlpCmdAviableIndex == NULL)
    {
        mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
        return NLP_ERR_PARAM_INIT;
    }
    memset(pNlpPram->pNlpCmdAviableIndex, 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(char));
    pNlpPram->pNlpCmdOut = (uint8_t *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(uint8_t));
    if (pNlpPram->pNlpCmdOut == NULL)
    {
        mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
        return NLP_ERR_PARAM_INIT;
    }
    memset(pNlpPram->pNlpCmdOut, 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(uint8_t));

    pNlpPram->pNlpCmdCfd = (int *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(int));
    if (pNlpPram->pNlpCmdCfd == NULL)
    {
        mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
        return NLP_ERR_PARAM_INIT;
    }
    memset(pNlpPram->pNlpCmdCfd, 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(int));

    pNlpPram->pNlpCmdWordsStartFrames = (int *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(int));
    if (pNlpPram->pNlpCmdWordsStartFrames == NULL)
    {
        mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
        return NLP_ERR_PARAM_INIT;
    }
    memset(pNlpPram->pNlpCmdWordsStartFrames, 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(int));

    pNlpPram->pNlpCmdWordsFramesLen = (int *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(int));
    if (pNlpPram->pNlpCmdWordsFramesLen == NULL)
    {
        mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
        return NLP_ERR_PARAM_INIT;
    }
    memset(pNlpPram->pNlpCmdWordsFramesLen, 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(int));

    pNlpPram->pNlpCmdNodeConfidenceNum = (uint8_t *)pvPortMalloc(pNlpPram->nlpAsrCmdMaxLen*sizeof(uint8_t));
    if (pNlpPram->pNlpCmdNodeConfidenceNum == NULL)
    {
        mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
        return NLP_ERR_PARAM_INIT;
    }
    memset(pNlpPram->pNlpCmdNodeConfidenceNum, 0, pNlpPram->nlpAsrCmdMaxLen * sizeof(uint8_t));
    pNlpPram->pNlpAviableCmdConnectStr = (char *)pvPortMalloc(pNlpPram->pNlpAviableCmdConnectStrMaxLen*sizeof(char));
    if (pNlpPram->pNlpAviableCmdConnectStr == NULL)
    {
        mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
        return NLP_ERR_PARAM_INIT;
    }
    memset(pNlpPram->pNlpAviableCmdConnectStr, 0, pNlpPram->pNlpAviableCmdConnectStrMaxLen*sizeof(char));
    pNlpPram->pNlpCmdSplitStr = (char **)pvPortMalloc(sizeof(char *)* ciss_get(CI_SS_INTENT_NUM));
    for (int i = 0; i < ciss_get(CI_SS_INTENT_NUM); i++)
    {
        pNlpPram->pNlpCmdSplitStr[i] = (char *)pvPortMalloc(pNlpPram->pNlpSplitCmdMaxLen*sizeof(char));
        if (pNlpPram->pNlpCmdSplitStr[i] == NULL)
        {
            mprintf("nlp param pvPortMalloc error:%d !\r\n", __LINE__);
            return NLP_ERR_PARAM_INIT;
        }
        memset(pNlpPram->pNlpCmdSplitStr[i], 0, pNlpPram->pNlpSplitCmdMaxLen * sizeof(char));
    }
    nlp_param_set(pNlpPram);
    
    return NLP_STATE_OK;
}


int nlp_module_init(void)
{

    #if (MULT_INTENT == 1)
    
    /*定义3.5待自然说关闭相关功能，
    A、第一个是产品关闭的词条，例如：空调关闭、风扇关闭等
    B、后面的是有关闭属性的功能词列表*/
    static char *gs_close_words[] = {"空调关闭", "上下扫风", "左右扫风", "扫风", "新风模式", "防直吹模式", "睡眠模式", "舒省模式", "节能模式", "智控温模式", "除菌模式", "无风感模式", "电辅热模式", "智清洁模式", "电辅热"};
   
    extern void nlp_set_close_func_addr(char **func_addr, uint8_t size);
    nlp_set_close_func_addr(gs_close_words, sizeof(gs_close_words) / sizeof(char *));
#else
    //下面变量中定义的词最大不能超过11个字，不然会报错
    /*
    第一个数组主要填写领域开相关的词条，主要用于多意图时 风扇打开xx 识别为风扇打开，打开xx ，用于消除前词，如更换领域，需将风扇换成对应领域词
    */
    //static char *pNlpSpecialCmdParamTemp0[] =  {"空调打开"};
    /*
    第二个数组用于开低档，开一下低档 开一档  开一下一档等极易误识为 开机，开低档的情况，用于消除误识的词 开机
    */
    static char *pNlpSpecialCmdParamTemp1[] =  {"打开一档", "打开一档风", "打开七档", "打开七档风", "打开最小", "打开最小风"};
 
    /*
    第四个用于 风扇关闭上下摇头 等词时， 容易误识为 风扇关闭，关闭上下摇头的情况 数组内的词为有关闭属性的功能词
    被包含的词条，放在长词条前面
    */
    static char *pNlpSpecialCmdParamTemp4[] =  {"杀菌", "负离子", "净离子", "清凉风", "上下扫风", "左右扫风", "扫风", "智能加湿", "加湿", "风随温变"};
    /*
    第五个用于相近易误识的词处理，如 打开最小音量 误识为打开最小风，打开最小声音 用于消除打开最小风， 索引值对应 下面 pNlpSpecialCmdParam5_val的索引
    */
    static char *pNlpSpecialCmdParamTemp5[] =  {"打开最小风", "打开最大风", "打开等", "打开负离子", "定时三小时"};
    static char *pNlpSpecialCmdParamTemp5Real[] =  {"打开最小声音", "打开最大声音", "打开负离子", "打开等离子风", "三小时打开"};

    /*
    暂时不做处理，如更换领域，需将风扇换成对应领域词
    */
    static char *pNlpSpecialCmdParamTemp6[] =  {"风扇打开", "打开风扇", "关闭风扇"};


    static NlpParamTypedef mNlpParam =
    {
            .nlpPrintEnable = NLP_LOG,
            .nlpAsrOutMaxNum = NLP_SAVE_STR_MAX_NUM,
            .nlpAsrCmdMaxLen = NLP_BUFFER_MAX,
            .pNlpAviableCmdConnectStrMaxLen = NLP_BUFFER_MAX*NLP_SAVE_STR_MAX_NUM,
            .pNlpSplitCmdMaxLen = NLP_BUFFER_MAX,
            .pNlpSpecialCmdParam1 = pNlpSpecialCmdParamTemp1,
            .pNlpSpecialCmdParam1Len = sizeof(pNlpSpecialCmdParamTemp1) / sizeof(char *),
            .pNlpSpecialCmdParam4 = pNlpSpecialCmdParamTemp4,
            .pNlpSpecialCmdParam4Len = sizeof(pNlpSpecialCmdParamTemp4) / sizeof(char *),
            .pNlpSpecialCmdParam5 = pNlpSpecialCmdParamTemp5,
            .pNlpSpecialCmdParam5Len = sizeof(pNlpSpecialCmdParamTemp5) / sizeof(char *),
            .pNlpSpecialCmdParam5_val = pNlpSpecialCmdParamTemp5Real,
            .pNlpSpecialCmdParam6 = pNlpSpecialCmdParamTemp6,
            .pNlpSpecialCmdParam6Len = sizeof(pNlpSpecialCmdParamTemp6) / sizeof(char *),
            .nlp_result_out_func = nlp_result,
            .nlp_intents_sort_func = nlp_intents_sort
    };
    if (nlp_param_init(&mNlpParam) != NLP_STATE_OK) //多意图参数初始化
    {
        mprintf("nlp param init error...\r\n");
        return NLP_ERR_COMMON;
    }
#endif
    return NLP_STATE_OK;
}

char nlp_buf[NLP_TEMP_BUF_MAX_LEN*2] = {0};
void nlp_result(nlp_data_t res, short is_wake_words, int cmd_words_cfd[4])
{
	if (res.intent_count)
	{
		#if NLP_ENABLE
		if (res.intent_count == 4)
		{
			sprintf(nlp_buf, "%s,%s,%s,", res.intent[0], res.intent[1], res.intent[2]); //sprintf最多拼接三个字符串
			sprintf(nlp_buf + strlen(nlp_buf), "%s", res.intent[3]);
			mprintf("nlp send result: %s %d-%d-%d-%d \n", nlp_buf, cmd_words_cfd[0], cmd_words_cfd[1], cmd_words_cfd[2], cmd_words_cfd[3]);
		}
		else if (res.intent_count == 3)
		{
			sprintf(nlp_buf, "%s,%s,%s", res.intent[0], res.intent[1], res.intent[2]);
			mprintf("nlp send result: %s %d-%d-%d \n", nlp_buf, cmd_words_cfd[0], cmd_words_cfd[1], cmd_words_cfd[2]);
		}
		else if (res.intent_count == 2)
		{
			sprintf(nlp_buf, "%s,%s", res.intent[0], res.intent[1]);
			mprintf("nlp send result: %s %d-%d \n", nlp_buf, cmd_words_cfd[0], cmd_words_cfd[1]);
		}
		else
		{
            if(is_wake_words) //1 代表唤醒词 0 代表命令词
            {
                mprintf("wake send result: %s %d \n", res.intent[0], cmd_words_cfd[0]);
            }
            else
            {
                mprintf("nlp send result: %s %d \n", res.intent[0], cmd_words_cfd[0]);
            }
		}
		#endif
	}
}
extern cmd_handle_t cmd_info_find_command_by_string(const char *cmd_string);
extern uint32_t cmd_info_get_semantic_id(cmd_handle_t cmd_handle);

char nlp_intent_temp_buf[NLP_BUFFER_MAX] = {0};

void nlp_intents_sort(nlp_data_t *res)
{
    uint32_t turn_on_semantic_id =  0x0101;
    uint32_t turn_off_semantic_id = 0x0100;
    uint32_t gs_semantic_id = 0;
    
    uint8_t powe_on_idx = 0xFF;
    uint8_t powe_off_idx = 0xFF;
    if (res->intent_count)
    {
        for(int i=0; i < res->intent_count; i++)
        {
            if(res->intent[i] != '\0')
            {
                cmd_handle_t cmd_handle_semantic = cmd_info_find_command_by_string(res->intent[i]);
                gs_semantic_id = cmd_info_get_semantic_id(cmd_handle_semantic);
                //ci_loginfo(LOG_USER, "gs_semantic_id=0x%04X \n", gs_semantic_id);
                if(gs_semantic_id == turn_on_semantic_id)
                {
                    powe_on_idx = i;
                }
                if(gs_semantic_id == turn_off_semantic_id)
                {
                    powe_off_idx = i;
                }
            }
        }
        //开机关机提到最前面
        if(powe_on_idx == 0xFF && powe_off_idx == 0xFF)
        {
            //不调顺序
        }
        else if(powe_on_idx != 0xFF && powe_off_idx == 0xFF)//只有开机调顺序
        {
            if(powe_on_idx == 1)
            {
                strncpy(nlp_intent_temp_buf, res->intent[0], NLP_BUFFER_MAX);
                strncpy(res->intent[0], res->intent[1], NLP_BUFFER_MAX);
                strncpy(res->intent[1], nlp_intent_temp_buf, NLP_BUFFER_MAX);
            }
            else if(powe_on_idx == 2)
            {
                strncpy(nlp_intent_temp_buf, res->intent[1], NLP_BUFFER_MAX);
                strncpy(res->intent[1], res->intent[2], NLP_BUFFER_MAX);
                strncpy(res->intent[2], nlp_intent_temp_buf, NLP_BUFFER_MAX);

                strncpy(nlp_intent_temp_buf, res->intent[0], NLP_BUFFER_MAX);
                strncpy(res->intent[0], res->intent[1], NLP_BUFFER_MAX);
                strncpy(res->intent[1], nlp_intent_temp_buf, NLP_BUFFER_MAX);
            }
        }
        else if(powe_on_idx == 0xFF && powe_off_idx != 0xFF)//只有关机,把关机后面的指令删除
        {
            if(powe_off_idx == 0)
            {
                memset(res->intent[1], 0, NLP_BUFFER_MAX);
                memset(res->intent[2], 0, NLP_BUFFER_MAX);
            }
            else if(powe_off_idx == 1)
            {
                memset(res->intent[2], 0, NLP_BUFFER_MAX);
            }
        }
        else if(powe_on_idx != 0xFF && powe_off_idx != 0xFF)//保持开关机顺序
        {
            if(powe_on_idx == 0 && powe_off_idx == 1)//关机后面的指令删除
            {
                memset(res->intent[2], 0, NLP_BUFFER_MAX);
            }
            else if((powe_on_idx == 0 && powe_off_idx == 2)
                || (powe_on_idx == 1 && powe_off_idx == 0)
                || (powe_on_idx == 1 && powe_off_idx == 2)
                || (powe_on_idx == 2 && powe_off_idx == 1))//不调整
            {
                //保持不变
            }
            else if(powe_on_idx == 2 && powe_off_idx == 0)//开机放到最后
            {
                strncpy(nlp_intent_temp_buf, res->intent[1], NLP_BUFFER_MAX);
                strncpy(res->intent[1], res->intent[2], NLP_BUFFER_MAX);
                strncpy(res->intent[2], nlp_intent_temp_buf, NLP_BUFFER_MAX);
            }
        }
    }
}