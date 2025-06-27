#include "signals.h"

#include <string.h>

bool block_signals(sigset_t *new_mask, sigset_t *old_mask) {
  sigemptyset(new_mask);
  sigaddset(new_mask, SIGINT);
  sigaddset(new_mask, SIGALRM);
  sigaddset(new_mask, SIGUSR1);
  sigaddset(new_mask, SIGUSR2);
  sigaddset(new_mask, SIGTERM);

  if (sigprocmask(SIG_BLOCK, new_mask, old_mask) < 0)
    return false;

  return true;
}

bool unblock_signals(sigset_t *old_mask) {
  if (sigprocmask(SIG_SETMASK, old_mask, NULL) < 0) {
    return false;
  }

  return true;
}
