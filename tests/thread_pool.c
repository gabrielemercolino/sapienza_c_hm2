#include "common/thread_pool.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define FROM 0
#define TO 100

int counter = 0;
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;

void task(int i);
int sum(const int from, const int to);

void task_wrapper(void *i_ptr) {
  int i = *(int *)i_ptr;
  free(i_ptr);
  pthread_mutex_lock(&counter_lock);
  task(i);
  pthread_mutex_unlock(&counter_lock);
}

int main() {
  int expected = sum(FROM, TO);
  ThreadPool *tp = create_thread_pool(5);

  for (int i = FROM; i < TO; i++) {
    int *arg = malloc(sizeof(int));
    *arg = i;
    thread_pool_do(tp, task_wrapper, arg);
  }

  thread_pool_join(tp);

  printf("result: %d\nexpected: %d\n", counter, expected);

  thread_pool_free(tp);

  if (counter != expected)
    return 1;

  return 0;
}

void task(int i) { counter += i; }
int sum(const int from, const int to) {
  int n = 0;

  for (int i = from; i < to; i++) {
    n += i;
  }

  return n;
}
