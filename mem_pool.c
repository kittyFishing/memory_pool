#include "mem_pool.h"



/* 描述： 创建内存池函数
*  参数：
*    @arg1:block_size, 要创建的内存池内存块大小byte
*    @arg2:block_count,  要创建的内存池内存块数量
*  返回值：成功返回内存池指针，失败返回NULL
*/
memPool *create_mem_pool(size_t block_size, size_t block_count)
{
    size_t i;

    assert(block_size > 0);
    assert(block_count > 0);

    memPool *pool = (memPool*)malloc(sizeof(memPool));
    if (!pool) {
        printf("[%s,%d]error, malloc failed\n", __func__, __LINE__);
        return NULL;
    }
    pool->pool = malloc(block_count * block_size);
    if (!pool->pool) {
        printf("[%s,%d]error, malloc failed\n", __func__, __LINE__);
        goto err;
    }

    pool->block_count = block_count;
    pool->block_size = block_size;
    pool->free_count = pool->block_count;
    pool->free_list = NULL;
    pool->shut_down = 0;
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    for (i = 0; i < block_count; i++) {
        memBlock *block = (memBlock*)((char*)pool->pool + i * block_size);
        block->next = pool->free_list;
        pool->free_list = block;
    }

    return pool;
err:
    if (pool) free(pool);
    if (pool->pool) free(pool->pool);

    return NULL;

}


/* 描述： 从内存池中申请内存块函数
*  参数：
*    @arg1:pool,目标内存池
*  返回值：成功返回内存块指针，失败返回NULL
*/
void *mem_pool_alloc(memPool *pool)
{
    memBlock *block;

    assert(pool != NULL);
    if(pool->shut_down)//池已经被关闭
    {
        printf("[%s,%d] Info, mem pool shutdown.\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    pthread_mutex_lock(&pool->mutex);

    /* 无空闲内存块，则等待 */
    while (pool->free_count == 0) {
        pthread_cond_wait(&pool->cond, &pool->mutex);
    }
    block = (memBlock*)pool->free_list;
    pool->free_list = block->next;
    pool->free_count--;

    pthread_mutex_unlock(&pool->mutex); 
    memset((void*)block, 0, pool->block_size);

    return block;
}


/* 描述：释放内存块回到内存池
*  参数：
*    @arg1:pool,目标内存池
*    @arg2:pblock,要释放的内存块指针
*  返回值：无
*/
void mem_pool_free(memPool *pool, void *pblock)
{
    memBlock *block;

    assert(pool != NULL);
    assert(pblock != NULL);
    if(pool->shut_down)//池已经被关闭
    {
        printf("[%s,%d] Info, mem pool shutdown.\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    pthread_mutex_lock(&pool->mutex);

    block = (memBlock*)pblock;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->free_count++;
    pthread_cond_signal(&pool->cond);

    pthread_mutex_unlock(&pool->mutex); 
}


/* 描述：销毁内存池
*  参数：
*    @arg1:pool,目标内存池
*  返回值：无
*/
void destroy_mem_pool(memPool *pool)
{
    pool->shut_down = 1;

    /* 唤醒所有阻塞线程 */
    pthread_cond_broadcast(&pool->cond);
    /* 销毁资源 */
    free(pool->pool);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    free(pool);
}
