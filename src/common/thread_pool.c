#include "thread_pool.h"

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct ThreadPool {
  size_t max_threads;
  sem_t limiter;
  pthread_mutex_t join_lock;
  size_t active_threads;
  pthread_cond_t join_cond;
};

typedef struct {
  ThreadPool *pool;
  thread_func_t func;
  void *arg;
} TPoolWork;

static void *task_wrapper(void *raw);

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

bool thread_pool_do(ThreadPool *pool, thread_func_t fn, void *arg) {
  sem_wait(&pool->limiter);

  TPoolWork *work = malloc(sizeof(TPoolWork));

  // if work couldn't be allocated
  if (!work) {
    fprintf(stderr, "couldn't allocate memory for the requested task\n");
    sem_post(&pool->limiter);
    return false;
  }

  work->func = fn;
  work->arg = arg;
  work->pool = pool;

  pthread_mutex_lock(&pool->join_lock);
  pool->active_threads++;
  pthread_mutex_unlock(&pool->join_lock);

  pthread_t thread;
  int res = pthread_create(&thread, NULL, task_wrapper, work);

  // res == 0 -> ok
  if (res == 0) {
    pthread_detach(thread);
    return true;
  }

  fprintf(stderr, "couldn't create thread for the requested task\n");
  pthread_mutex_lock(&pool->join_lock);
  pool->active_threads--;
  pthread_mutex_unlock(&pool->join_lock);
  sem_post(&pool->limiter);
  free(work);
  return false;
}

void thread_pool_join(ThreadPool *pool) {
  pthread_mutex_lock(&pool->join_lock);
  while (pool->active_threads > 0)
    pthread_cond_wait(&pool->join_cond, &pool->join_lock);
  pthread_mutex_unlock(&pool->join_lock);
}

static void *task_wrapper(void *raw) {
  TPoolWork *work = raw;
  ThreadPool *pool = work->pool;
  work->func(work->arg);

  pthread_mutex_lock(&pool->join_lock);

  pool->active_threads--;
  if (pool->active_threads == 0)
    pthread_cond_signal(&pool->join_cond);

  pthread_mutex_unlock(&pool->join_lock);
  sem_post(&pool->limiter);

  free(work);
  return NULL; // needed for the signature but not used
}
