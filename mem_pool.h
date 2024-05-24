#ifndef __MEMPOOL_H
#define __MEMPOOL_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include "usr_def_types.h"
#include "logtool.h"

/* 内存块结构 */
typedef struct memory_block {
    struct memory_block *next;
}memBlock;

/* 内存池结构 */
typedef struct memory_pool {
    memBlock            *free_list;     /* 空闲内存块链表 */
    size_t              block_count;    /* 内存池的内存块总数 */
    size_t              block_size;     /* 内存池的内存块的大小 */
    size_t              free_count;     /* 空闲的内存块数量 */
    void                *pool;          /* 内存池内存首地址指针 */
    pthread_mutex_t     mutex;          /* 互斥锁 */
    pthread_cond_t      cond;           /* 内存池条件变量*/
    int                 shut_down;      /* 内存池子情况 */
}memPool;


memPool *create_mem_pool(size_t block_size, size_t block_count);

/* 描述： 从内存池中申请内存块函数
*  参数：
*    @arg1:pool,目标内存池
*  返回值：成功返回内存块指针，失败返回NULL
*/
void *mem_pool_alloc(memPool *pool);


/* 描述：释放内存块回到内存池
*  参数：
*    @arg1:pool,目标内存池
*    @arg2:pblock,要释放的内存块指针
*  返回值：无
*/
void mem_pool_free(memPool *pool, void *pblock);


/* 描述：销毁内存池
*  参数：
*    @arg1:pool,目标内存池
*  返回值：无
*/
void destroy_mem_pool(memPool *pool);

#endif
