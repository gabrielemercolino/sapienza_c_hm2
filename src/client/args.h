#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  const char *file_path;
  uint64_t key;
  uint64_t threads;
  const char *server_ip;
  uint16_t server_port;
} ClientConfig;

typedef enum {
  OK,
  UNKNOWN_FLAG,
  WRONG_USAGE,
  ONLY_HELP,

  MISSING_FILE,
  FILE_NOT_FOUND,
  MULTIPLE_FILES,

  MISSING_KEY,
  MALFORMED_KEY,
  MULTIPLE_KEYS,

  MISSING_THREADS, // TODO: maybe default to 2 or like 1 thread for each 64
                   // bit chunk
  MALFORMED_THREADS,
  MULTIPLE_THREADS,

  MISSING_SERVER_IP, // TODO: maybe default to localhost
  MULTIPLE_SERVER_IP,

  MISSING_SERVER_PORT, // TODO: maybe the port can default to 8080?
  MALFORMED_SERVER_PORT,
  MULTIPLE_SERVER_PORT,

} PAResult;

PAResult parse_args(const int argc, char *argv[], ClientConfig *out);
char *pa_result_to_string(PAResult result);
void print_usage(const char *program_name);
