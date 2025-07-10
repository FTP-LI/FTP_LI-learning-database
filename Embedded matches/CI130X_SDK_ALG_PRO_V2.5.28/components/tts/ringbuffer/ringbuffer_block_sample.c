#include "string.h"
#include "ci_log.h"
#include "ringbuffer_block.h"
#include "ringbuffer_block.h"
#include "romlib_runtime.h"
#define  RINGBUFFER_BLOCK_TEST
#ifdef RINGBUFFER_BLOCK_TEST



stbufferblock_t *pTestRingBlockBuffer = NULL;
static unsigned char bStartTest = 0;
static void ringbufferblock_test1(void *p)
{
    int index = 0;
    stbufferinfo* bufferinfo = NULL;

    while (bStartTest)
    {
        if (ringbufferblock_available(pTestRingBlockBuffer) >= 1)
        {
            bufferinfo = ringbufferblock_get_freeblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            MASK_ROM_LIB_FUNC->newlibcfunc.memcpy_p(bufferinfo->base, &index, sizeof(index));

            ringbufferblock_pushblock(pTestRingBlockBuffer);
            index++;
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    vTaskDelete(NULL);
}


static void ringbufferblock_test2(void *p)
{
    int* pindex = NULL;
    stbufferinfo* bufferinfo = NULL;
    int i32check_ret = 0;

    while (bStartTest)
    {
        if (ringbufferblock_size(pTestRingBlockBuffer) >= 1)
        {
            bufferinfo = ringbufferblock_get_pushblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            pindex = (int *)bufferinfo->base;

            // mprintf("%d ", *pindex);
            if (i32check_ret != *pindex)
            {
                mprintf("Error %s  %d  ret:%d  need:%d\n", __func__, __LINE__, *pindex, i32check_ret);
                bStartTest = 0;
            }

            if (0 == i32check_ret%10000)
            {
                mprintf("test cnt:%d\n", i32check_ret);
            }

            i32check_ret++;

            ringbufferblock_freeblock(pTestRingBlockBuffer);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    vTaskDelete(NULL);
}


static void ringbufferblock_test3(void *p)
{
    int index = 0;
    stbufferinfo* bufferinfo1 = NULL;
    stbufferinfo* bufferinfo2 = NULL;

    while (bStartTest)
    {
        if (ringbufferblock_available(pTestRingBlockBuffer) >= 2)
        {
            bufferinfo1 = ringbufferblock_get_freeblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo1)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            MASK_ROM_LIB_FUNC->newlibcfunc.memcpy_p(bufferinfo1->base, &index, sizeof(index));
            index++;

            bufferinfo2 = ringbufferblock_get_freeblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo2)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            MASK_ROM_LIB_FUNC->newlibcfunc.memcpy_p(bufferinfo2->base, &index, sizeof(index));
            index++;

            ringbufferblock_pushblock(pTestRingBlockBuffer);
            ringbufferblock_pushblock(pTestRingBlockBuffer);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    vTaskDelete(NULL);
}



static void ringbufferblock_test4(void *p)
{
    int index = 0;
    stbufferinfo* bufferinfo1 = NULL;
    stbufferinfo* bufferinfo2 = NULL;
    stbufferinfo* bufferinfo3 = NULL;

    while (bStartTest)
    {
        if (ringbufferblock_available(pTestRingBlockBuffer) >= 2)
        {
            bufferinfo1 = ringbufferblock_get_freeblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo1)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            MASK_ROM_LIB_FUNC->newlibcfunc.memcpy_p(bufferinfo1->base, &index, sizeof(index));
            index++;

            bufferinfo2 = ringbufferblock_get_freeblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo2)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            MASK_ROM_LIB_FUNC->newlibcfunc.memcpy_p(bufferinfo2->base, &index, sizeof(index));
            index++;

            bufferinfo3 = ringbufferblock_get_freeblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo3)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            MASK_ROM_LIB_FUNC->newlibcfunc.memcpy_p(bufferinfo3->base, &index, sizeof(index));
            index++;

            ringbufferblock_pushblock(pTestRingBlockBuffer);
            ringbufferblock_pushblock(pTestRingBlockBuffer);
            ringbufferblock_pushblock(pTestRingBlockBuffer);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    vTaskDelete(NULL);
}



static void ringbufferblock_test5(void *p)
{
    int* pindex = NULL;
    stbufferinfo* bufferinfo1 = NULL;
    stbufferinfo* bufferinfo2 = NULL;
    int i32check_ret = 0;

    while (bStartTest)
    {
        if (ringbufferblock_size(pTestRingBlockBuffer) >= 2)
        {
            bufferinfo1 = ringbufferblock_get_pushblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo1)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            bufferinfo2 = ringbufferblock_get_pushblock(pTestRingBlockBuffer);
            if (NULL == bufferinfo2)
            {
                mprintf("error %s  %d\n", __func__, __LINE__);
            }

            pindex = (int *)bufferinfo1->base;
            // mprintf("%d ", *pindex);
            if (i32check_ret != *pindex)
            {
                mprintf("Error %s  %d  ret:%d  need:%d\n", __func__, __LINE__, *pindex, i32check_ret);
                bStartTest = 0;
            }

            if (0 == i32check_ret%10000)
            {
                mprintf("test cnt:%d\n", i32check_ret);
            }
            i32check_ret++;


            pindex = (int *)bufferinfo2->base;
            // mprintf("%d ", *pindex);
            if (i32check_ret != *pindex)
            {
                mprintf("Error %s  %d  ret:%d  need:%d\n", __func__, __LINE__, *pindex, i32check_ret);
                bStartTest = 0;
            }

            if (0 == i32check_ret%10000)
            {
                mprintf("test cnt:%d\n", i32check_ret);
            }
            i32check_ret++;

            ringbufferblock_freeblock(pTestRingBlockBuffer);
            ringbufferblock_freeblock(pTestRingBlockBuffer);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }

    vTaskDelete(NULL);
}



void ringbuffer_block_sample(void)
{
    pTestRingBlockBuffer = ringbufeferblock_init(BUFFER_TYPE_HW, 4, 5);

    // test 1
    mprintf("Start Test Case1\n");
    bStartTest = 1;
    xTaskCreate(ringbufferblock_test1, "ringbufferblock_test1", 256, NULL, 4, NULL);
    xTaskCreate(ringbufferblock_test2, "ringbufferblock_test2", 256, NULL, 4, NULL);
    vTaskDelay(pdMS_TO_TICKS(10*60*1000));


    bStartTest = 0;
    mprintf("Start Test Case2\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    bStartTest = 1;
    xTaskCreate(ringbufferblock_test3, "ringbufferblock_test3", 256, NULL, 4, NULL);
    xTaskCreate(ringbufferblock_test2, "ringbufferblock_test2", 256, NULL, 4, NULL);
    vTaskDelay(pdMS_TO_TICKS(10*60*1000));


    bStartTest = 0;
    mprintf("Start Test Case3\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
    bStartTest = 1;
    xTaskCreate(ringbufferblock_test4, "ringbufferblock_test4", 256, NULL, 4, NULL);
    xTaskCreate(ringbufferblock_test5, "ringbufferblock_test5", 256, NULL, 4, NULL);
    vTaskDelay(pdMS_TO_TICKS(10*60*1000));


    bStartTest = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
    ringbufeferblock_free(pTestRingBlockBuffer);
    mprintf("Test end\n");
}

#endif