#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "mem_pool.h"

/* 线程参数结构 */ 
typedef struct ThreadArgs {
    memPool* pool;
    int thread_id;
    int alloc_count;
    int free_count;
} ThreadArgs;

void* thread_function(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    memPool* pool = args->pool;
    int thread_id = args->thread_id;
    int alloc_count = 0;
    int free_count = 0;

    /*  模拟随机分配和释放内存块 */
    for (int i = 0; i < 100; ++i) {
        void* ptr = mem_pool_alloc(pool);
        printf("Thread %d: Allocated memory block: %p\n", thread_id, ptr);
        alloc_count++;
        usleep(rand() % 1000); // 模拟处理时间

        mem_pool_free(pool, ptr);
        printf("Thread %d: Freed memory block: %p\n", thread_id, ptr);
        free_count++;
        usleep(rand() % 1000); // 模拟处理时间
    }

    args->alloc_count = alloc_count;
    args->free_count = free_count;

    return NULL;
}

int main() {
    srand(time(NULL));
    int num_threads = 8;
    memPool* pool = create_mem_pool(1024, 1024); // 创建内存池，每块32字节，共10块

    if (!pool) {
        fprintf(stderr, "Failed to create memory pool\n");
        return 1;
    }

    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

    /* 创建并启动线程 */
    for (int i = 0; i < num_threads; ++i) {
        thread_args[i].pool = pool;
        thread_args[i].thread_id = i;
        thread_args[i].alloc_count = 0;
        thread_args[i].free_count = 0;
        pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
    }

    /* 等待所有线程完成 */
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    /* 输出分配和释放计数 */
    int total_alloc = 0;
    int total_free = 0;
    for (int i = 0; i < num_threads; ++i) {
        printf("Thread %d: alloc_count = %d, free_count = %d\n",
               thread_args[i].thread_id, thread_args[i].alloc_count, thread_args[i].free_count);
        total_alloc += thread_args[i].alloc_count;
        total_free += thread_args[i].free_count;
    }

    printf("Total alloc_count = %d, Total free_count = %d\n", total_alloc, total_free);

    /* 销毁内存池 */
    destroy_mem_pool(pool);

    return 0;
}
