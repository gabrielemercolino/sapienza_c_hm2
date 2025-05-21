#include "thread_pool.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

struct ThreadPool {
  size_t max_threads; // maximum number of threads
  sem_t limiter;      // semaphore used to ensure the max threads limit

  size_t active_threads;     // number of active threads
  pthread_mutex_t join_lock; // needed to use `active_threads` safely
  pthread_cond_t join_cond; // signal trigghered by the last thread when closing
};

typedef struct {
  ThreadPool *pool;
  task_func_t func;
  void *arg;
} TPTask;

/**
 * @brief manages the lifetime of the task
 *
 * Calls the task function with it's argument and when it's done updates the
 * pool info
 *
 * @param task_raw the `TPTask` ptr as a `void*` for signature reasons
 * @return always `NULL` for signature reasons
 */
static void *task_wrapper(void *task_raw);

ThreadPool *create_thread_pool(size_t max_threads) {
  ThreadPool *tp = malloc(sizeof(ThreadPool));

  tp->max_threads = max_threads;
  sem_init(&tp->limiter, 0, max_threads);
  pthread_mutex_init(&tp->join_lock, NULL);
  pthread_cond_init(&tp->join_cond, NULL);
  tp->active_threads = 0;

  return tp;
}

void thread_pool_free(ThreadPool *pool) {
  // ensure everythin is settled
  thread_pool_join(pool);

  sem_destroy(&pool->limiter);
  pthread_mutex_destroy(&pool->join_lock);
  pthread_cond_destroy(&pool->join_cond);

  free(pool);
  pool = NULL;
}

TPTaskResult thread_pool_do(ThreadPool *pool, task_func_t fn, void *arg) {
  // wait for a thread to close if the maximum number of threads is reached
  sem_wait(&pool->limiter);

  TPTask *task = malloc(sizeof(TPTask));

  // if work couldn't be allocated
  if (!task) {
    // I still need to ensure that the semaphore is valid
    sem_post(&pool->limiter);
    return FAILED_CREATION;
  }

  task->func = fn;
  task->arg = arg;
  task->pool = pool;

  pthread_mutex_lock(&pool->join_lock);
  pool->active_threads++;
  pthread_mutex_unlock(&pool->join_lock);

  pthread_t thread;
  int res = pthread_create(&thread, NULL, task_wrapper, task);

  // res == 0 -> ok
  if (res == 0) {
    // ensures that the thread can close on it's own, the join machanism
    // is not required and is implemented manually
    pthread_detach(thread);
    // the semaphore in this case has to be updated only when the task is done
    // in `task_wrapper`
    return OK;
  }

  // I still have to ensure to have a valid state
  pthread_mutex_lock(&pool->join_lock);
  pool->active_threads--;
  pthread_mutex_unlock(&pool->join_lock);
  sem_post(&pool->limiter);

  free(task);
  // don't free `arg` as it's a responsibility of the caller to free the memory
  // if the task couldn't be created
  return FAILED_START;
}

TPTaskResult thread_pool_try_do(ThreadPool *pool, task_func_t fn, void *arg) {
  // try to get teh semaphore without blocking
  if (sem_trywait(&pool->limiter) != 0) {
    return POOL_BUSY;
  }

  TPTask *task = malloc(sizeof(TPTask));

  // if work couldn't be allocated
  if (!task) {
    // I still need to ensure that the semaphore is valid
    sem_post(&pool->limiter);
    return FAILED_CREATION;
  }

  task->func = fn;
  task->arg = arg;
  task->pool = pool;

  pthread_mutex_lock(&pool->join_lock);
  pool->active_threads++;
  pthread_mutex_unlock(&pool->join_lock);

  pthread_t thread;
  int res = pthread_create(&thread, NULL, task_wrapper, task);

  // res == 0 -> ok
  if (res == 0) {
    // ensures that the thread can close on it's own, the join machanism
    // is not required and is implemented manually
    pthread_detach(thread);
    // the semaphore in this case has to be updated only when the task is done
    // in `task_wrapper`
    return OK;
  }

  // I still have to ensure to have a valid state
  pthread_mutex_lock(&pool->join_lock);
  pool->active_threads--;
  pthread_mutex_unlock(&pool->join_lock);
  sem_post(&pool->limiter);

  free(task);
  // don't free `arg` as it's a responsibility of the caller to free the memory
  // if the task couldn't be created
  return FAILED_START;
}

void thread_pool_join(ThreadPool *pool) {
  pthread_mutex_lock(&pool->join_lock);
  while (pool->active_threads > 0)
    pthread_cond_wait(&pool->join_cond, &pool->join_lock);
  pthread_mutex_unlock(&pool->join_lock);
}

static void *task_wrapper(void *task_raw) {
  TPTask *task = task_raw;
  ThreadPool *pool = task->pool;

  // call the task
  task->func(task->arg);

  // now the task is done and I need to update the pool
  pthread_mutex_lock(&pool->join_lock);

  pool->active_threads--;
  if (pool->active_threads == 0)
    pthread_cond_signal(&pool->join_cond);

  pthread_mutex_unlock(&pool->join_lock);
  sem_post(&pool->limiter);

  free(task);
  return NULL; // needed for the signature but not used
}
