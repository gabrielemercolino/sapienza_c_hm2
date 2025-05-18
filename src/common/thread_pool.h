#pragma once

#include <stddef.h>

typedef struct ThreadPool ThreadPool;
typedef void (*thread_func_t)(void *arg);

ThreadPool *create_thread_pool(size_t max_threads);

void thread_pool_free(ThreadPool *pool);

bool thread_pool_do(ThreadPool *pool, thread_func_t fn, void *arg);

void thread_pool_join(ThreadPool *pool);
