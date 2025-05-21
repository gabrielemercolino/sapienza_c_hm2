#pragma once

#include <stddef.h>

/**
 * @brief opaque struct needed for a thread pool
 */
typedef struct ThreadPool ThreadPool;

/**
 * @brief function signature for a task
 *
 * The function receives a void pointer and returns nothing
 */
typedef void (*task_func_t)(void *arg);

typedef enum {
  OK,              // task created and started
  POOL_BUSY,       // thread pool has not free threads
  FAILED_CREATION, // failed to allocate memory for the task
  FAILED_START     // failed to start the task
} TPTaskResult;

/**
 * @brief creates a thread pool with the specified maximum number of threads
 *
 * Won't actually create the threads but the pool will be configured to use at
 * max that number of threads at any moment
 *
 * @param max_threads the maximum number of concurrent threads
 * @return the configured thread pool
 */
ThreadPool *create_thread_pool(size_t max_threads);

/**
 * @brief frees the memory allocated by the thread pool
 *
 * Frees only th memory of the `ThreadPool` object, not the memory that a task
 * could allocate.
 *
 * For safety reasons a call to `thread_pool_join` is made.
 *
 * After this there is no need to call `free`.
 *
 * @param pool the thread pool
 */
void thread_pool_free(ThreadPool *pool);

/**
 * @brief submits a task to the thread pool
 *
 * If the pool has all the threads occupied it waits until at leas one finishes,
 * meaning it's blocking.
 *
 * The arg has to be allocated on the heap.
 *
 * For the non blocking version use `thread_pool_try_do`.
 *
 * @param pool the thread pool
 * @param fn the task
 * @param arg the task argument
 * @return `OK` if successful or an error status
 */
TPTaskResult thread_pool_do(ThreadPool *pool, task_func_t fn, void *arg);

/**
 * @brief submits a task to the thread pool in a non-blocking way
 *
 * Same as `thread_pool_do` but exits early if the task could not be created as
 * the pool is completely used
 *
 * The non-blocking part refers to the creation of the task
 *
 * @param pool the thread pool
 * @param fn the task
 * @param arg the task argument
 * @return `OK` if successful or an error status
 */
TPTaskResult thread_pool_try_do(ThreadPool *pool, task_func_t fn, void *arg);

/**
 * @brief waits until all tasks are completed
 *
 * Waits for all the task to be completed in a blocking way.
 *
 * If the tasks are already completed returns immediately.
 *
 * @param pool the thread pool
 */
void thread_pool_join(ThreadPool *pool);
