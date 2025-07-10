#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#ifndef __RINGBUFFER_BLOCK_H
#define __RINGBUFFER_BLOCK_H



typedef enum
{
    BLOCK_HEAD = 0,     // 头
    BLOCK_TAIL,         // 尾
}eHeadType;

typedef enum
{
    BLOCK_TYPE_FREE = 0,    // 空闲buffer，可以被分配
    BLOCK_TYPE_MALLOC,      // 已分配的buffer，但是还没有写入数据
    BLOCK_TYPE_PUSH,        // 已写入数据，还没有被获取
    BLOCK_TYPE_PULL,        // 已被获取，还没有释放
}eBlockType;

typedef enum
{
    BUFFER_TYPE_SW = 0,     // 软件计算用pvPortMalloc
    BUFFER_TYPE_HW,         // 硬件计算用malloc
}eBufferType;

typedef struct __stblockinfo_t
{
    uint32_t u32free_id;
    uint32_t u32mall_id;
    uint32_t u32push_id;
    uint32_t u32pull_id;
}stblockinfo;

typedef struct __stbufferinfo_t
{
    uint8_t * base;
    uint32_t size;
}stbufferinfo;

typedef struct __bufferblock_t
{
    stbufferinfo bufferinfo;
    eBlockType block_type; //先不用
}stblock_t;

typedef struct __ringbufferblockhandle_t
{
    uint32_t u32block_cnt;
    eBufferType buffer_type;
    stblock_t *blockbufferlist;
    SemaphoreHandle_t block_mutex;
    stblockinfo block_info;
}stbufferblock_t;


//空余空间，能被修改的空间
uint32_t ringbufferblock_available(const stbufferblock_t *rbb);

//计算malloc和push的块数
uint32_t ringbufferblock_malloc_push_size(const stbufferblock_t *rbb);

// 已经写了内存的空间，可以被用于其它任务读取
uint32_t ringbufferblock_size(const stbufferblock_t *rbb);


uint32_t ringbufferblock_getpullblock_size(const stbufferblock_t *rbb);

float ringbufeferblock_print_free(stbufferblock_t *rbb);

// 获取空闲空间，获取可以被赋值, 这个动作可以获取空闲地址 ， 获取后可以被 ringbufferblock_pushblock 调用
stbufferinfo* ringbufferblock_get_freeblock(stbufferblock_t *rbb);

// 内容已修改，可以被获取参与计算， 这个动作可以获取内存地址 ， 获取后可以被 ringbufferblock_freeblock 调用
stbufferinfo* ringbufferblock_get_pushblock(stbufferblock_t *rbb);

// 用于已经被写了值，但是还没有推送到ringbuffer, 这个动作可以推送到ringbuffer,获取后可以被 ringbufferblock_get_pushblock 调用
void ringbufferblock_pushblock(stbufferblock_t *rbb);

// 内容已经被读取，但是还没有释放内存，这部分可以释放内存，释放后就可以被 ringbufferblock_get_freeblock 调用
void ringbufferblock_freeblock(stbufferblock_t *rbb);

// 相关打印信息
void ringbufeferblock_print_blockinfo(stbufferblock_t *rbb);

// 清空buffer
void ringbufeferblock_clear(stbufferblock_t* rbb);

// 释放块状循环缓冲区
void ringbufeferblock_free(stbufferblock_t* rbb);

// 初始化块状循环缓冲区
stbufferblock_t* ringbufeferblock_init(eBufferType buffer_type, uint32_t block_size, uint32_t block_cnt);
#endif