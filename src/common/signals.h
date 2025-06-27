#pragma once

#include <signal.h>

bool block_signals(sigset_t *new_mask, sigset_t *old_mask);
bool unblock_signals(sigset_t *old_mask);
