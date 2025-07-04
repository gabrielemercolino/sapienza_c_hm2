#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t threads;
  char *file_prefix;
  uint16_t max_connections;
} ServerConfig;

typedef enum {
  OK,
  UNKNOWN_FLAG,
  WRONG_USAGE,
  ONLY_HELP,

  MISSING_THREADS,
  MALFORMED_THREADS,
  MULTIPLE_THREADS,

  MISSING_FILE_PREFIX,
  MALFORMED_FILE_PREFIX,
  MULTIPLE_FILE_PREFIX,

  MISSING_MAX_CONNECTIONS,
  MALFORMED_MAX_CONNECTIONS,
  MULTIPLE_MAX_CONNECTIONS,
} SPAResult;

SPAResult server_parse_args(const int argc, char *argv[], ServerConfig *out);
char *server_pa_result_to_string(SPAResult result);
void server_print_usage(const char *program_name);
