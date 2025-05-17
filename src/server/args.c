#include "args.h"
#include "string.h"

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define BASE_10 10
#define MIN_CONN 1
#define MAX_CONN INT_MAX

// macro used to inform compiler to add a call to the specified function
// before ebery return statement to free the declared string as doing it
// manually would result in too much boilerplate and less readable code
#define deferred_free_str __attribute__((cleanup(defer_free_str)))
static void defer_free_str(char **thing) { free(*thing); }

static bool is_valid_prefix(const char *prefix);

SPAResult server_parse_args(const int argc, char *argv[], ServerConfig *out) {
  opterr = 0; // disable default error message

  // setup temp variables with invalid data so it can be detected later if
  // something is wrong
  char *threads deferred_free_str = nullptr;
  char *file_prefix deferred_free_str = nullptr;
  char *max_connections deferred_free_str = nullptr;

  const struct option options[] = {
      {"threads", required_argument, nullptr, 't'},
      {"prefix", required_argument, nullptr, 'p'},
      {"connections", required_argument, nullptr, 'c'},
      {"help", no_argument, nullptr, 'h'},
      // don't remove
      {nullptr, 0, nullptr, 0},
  };

  int option;
  while ((option = getopt_long(argc, argv, "t:c:p:h", options, nullptr)) !=
         -1) {
    switch (option) {
    case 't':
      if (threads != nullptr)
        return MULTIPLE_THREADS;
      threads = strdup(optarg);

      // if the next token is not a flag the args are not valid
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_THREADS;

      break;

    case 'c':
      if (max_connections != nullptr)
        return MULTIPLE_MAX_CONNECTIONS;
      max_connections = strdup(optarg);

      // if the next token is not a flag the args are not valid
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_MAX_CONNECTIONS;

      break;

    case 'p':
      if (file_prefix != nullptr)
        return MULTIPLE_FILE_PREFIX;
      file_prefix = strdup(optarg);

      // if the next token is not a flag the args are not valid
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_FILE_PREFIX;

      break;

    case 'h':
      return ONLY_HELP;
      break;

    // the function returns '?' when an unknown flag is passed
    case '?':
    default:
      return UNKNOWN_FLAG;
    }
  }

  // now I can check if the args are all set
  if (threads == nullptr)
    return MISSING_THREADS;

  char *end_threads;
  long _threads = strtol(threads, &end_threads, BASE_10);
  if (errno != 0 || *end_threads != '\0' || _threads < 1 || _threads > INT_MAX)
    return MALFORMED_THREADS;

  if (file_prefix == nullptr)
    return MISSING_FILE_PREFIX;
  if (!is_valid_prefix(file_prefix))
    return MALFORMED_FILE_PREFIX;

  if (max_connections == nullptr)
    return MISSING_MAX_CONNECTIONS;

  char *end_conn;
  long _conn = strtol(max_connections, &end_conn, BASE_10);
  if (errno != 0 || *end_conn != '\0' || _conn < MIN_CONN || _conn > MAX_CONN)
    return MALFORMED_MAX_CONNECTIONS;

  // if here then the values can be safely casted and saved
  out->threads = _threads;
  out->file_prefix = strdup(file_prefix);
  out->max_connections = _conn;

  return OK;
}

void server_print_usage(const char *program_name) {
  fprintf(stderr,
          "Usage:\n"
          "  %s -t <threads> -p <file prefix> -c <max connections> [-h]\n"
          "\n"
          "Options:\n"
          "  -t <threads>          number of threads to use for encryption\n"
          "  -p <file prefix>      prefix for all received files\n"
          "  -c <max connections>  max concurrent connections\n"
          "  -h                    show this help (ignores everything else)\n",
          program_name);
}

char *server_pa_result_to_string(const SPAResult result) {
  switch (result) {
  case OK:
    return "success";
  case UNKNOWN_FLAG:
    return "unknown flag";
  case WRONG_USAGE:
    return "args could not be parsed";
  case ONLY_HELP:
    return ""; // the actual message is printed by `print_usage`

  case MISSING_THREADS:
    return "must specify the number of threads";
  case MALFORMED_THREADS:
    return "number of threads is invalid";
  case MULTIPLE_THREADS:
    return "must specify the number of threads only once";

  case MISSING_FILE_PREFIX:
    return "must specify file prefix";
  case MALFORMED_FILE_PREFIX:
    return "file prefix is invalid";
  case MULTIPLE_FILE_PREFIX:
    return "must specify file prefix only once";

  case MISSING_MAX_CONNECTIONS:
    return "must specify max number of connections";
  case MALFORMED_MAX_CONNECTIONS:
    return "max number of connections is invalid";
  case MULTIPLE_MAX_CONNECTIONS:
    return "must specify max number of connections only once";
  }

  // effectively unreachable but makes compiler happy
  return "unknown error";
}

bool is_valid_prefix(const char *prefix) {
  // empty prefix
  if (strlen(prefix) == 0)
    return true;

  // first char cannot be a number
  {
    char first = prefix[0];
    if (!(isalpha(first)) && first != '_')
      return false;
  }

  for (size_t i = 0; i < strlen(prefix); i++) {
    if (!isalnum(prefix[i]) && prefix[0] != '_')
      return false;
  }

  return true;
}
