#include "common/thread_pool.h"

#include <stdlib.h>
#include <unistd.h>

#define MAX_THREADS 5

// simulates a task that is longer enough for the pool to be full and fail to
// start a task in the for loop
void dummy_task() { sleep(1); }

void dummy_task_wrapper(void *nothing) {
  dummy_task();

  // just to remove unused warning
  free(nothing);
}

int main() {
  ThreadPool *tp = create_thread_pool(MAX_THREADS);

  for (int i = 0; i < MAX_THREADS + 1; i++) {
    switch (thread_pool_try_do(tp, dummy_task_wrapper, NULL)) {
    case POOL_BUSY:
      thread_pool_free(tp);
      return 0;

    default:
      break;
    }
  }

  thread_pool_free(tp);
  return 1;
}
