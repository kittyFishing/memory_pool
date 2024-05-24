# CMAKE 最低版本要求
cmake_minimum_required(VERSION 3.10)
# 项目信息
project(mempool_demo)

#设置option选项
option(WITH_MEM_POOL_TEST "build with mem pool test" ON)

# 编译选项定义
enable_language(C CXX ASM)

# 指定头文件目录
include_directories(${PROJECT_SOURCE_DIR})
endif()

# 指定源文件/目录
set(SOURCE_DIR 
    ${PROJECT_SOURCE_DIR}mem_pool.c
)

# 宏定义
add_definitions(
    -DARCH_CPU_64BIT
    # -DDAEMON
)

# 链接库

#指定生成目标文件

if(WITH_MEM_POOL_TEST)
    set(TEST_MEM_POOL_SRC 
        ${PROJECT_SOURCE_DIR}/mem_pool.c
        ${PROJECT_SOURCE_DIR}/mem_pool_test.c
    )
    add_executable(mem_pool_demo  ${TEST_MEM_POOL_SRC})
    target_link_libraries(mem_pool_demo libpthread.so libc.so)
endif(WITH_MEM_POOL_TEST)
