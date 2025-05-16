#include "args.h"
#include "string.h"

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define BASE_10 10
#define MIN_PORT 0
#define MAX_PORT 65535

// macro used to inform compiler to add a call to the specified function
// before ebery return statement to free the declared string as doing it
// manually would result in too much boilerplate and less readable code
#define deferred_free_str __attribute__((cleanup(defer_free_str)))
void defer_free_str(char **thing) { free(*thing); }

PAResult parse_args(const int argc, char *argv[], ClientConfig *out) {
  opterr = 0; // disable default error message

  // setup temp variables with invalid data so it can be detected later if
  // something is wrong
  char *file_name deferred_free_str = nullptr;
  char *key deferred_free_str = nullptr;
  char *threads deferred_free_str = nullptr;
  char *server_ip deferred_free_str = nullptr;
  char *server_port deferred_free_str = nullptr;

  const struct option options[] = {
      {"file", required_argument, nullptr, 'f'},
      {"key", required_argument, nullptr, 'k'},
      {"threads", required_argument, nullptr, 't'},
      {"ip", required_argument, nullptr, 'a'},
      {"port", required_argument, nullptr, 'p'},
      {"help", no_argument, nullptr, 'h'},
      // don't remove
      {nullptr, 0, nullptr, 0},
  };

  int option;
  while ((option = getopt_long(argc, argv, "f:k:t:a:p:h", options, nullptr)) !=
         -1) {
    switch (option) {
    case 'f':
      if (file_name != nullptr)
        return MULTIPLE_FILES;
      file_name = strdup(optarg);

      // if the next token is not a flag the args are not valid
      // e.g. "program --file input1 invalid ..."
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_FILES;

      break;

    case 'k':
      if (key != nullptr)
        return MULTIPLE_KEYS;
      key = strdup(optarg);

      // if the next token is not a flag the args are not valid
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_KEYS;

      break;

    case 't':
      if (threads != nullptr)
        return MULTIPLE_THREADS;
      threads = strdup(optarg);

      // if the next token is not a flag the args are not valid
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_THREADS;

      break;

    case 'a':
      if (server_ip != nullptr)
        return MULTIPLE_SERVER_IP;
      server_ip = strdup(optarg);

      // if the next token is not a flag the args are not valid
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_SERVER_IP;

      break;
    case 'p':
      if (server_port != nullptr)
        return MULTIPLE_SERVER_PORT;
      server_port = strdup(optarg);

      // if the next token is not a flag the args are not valid
      if (optind < argc && argv[optind][0] != '-')
        return MULTIPLE_SERVER_PORT;

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
  if (file_name == nullptr)
    return MISSING_FILE;

  // check if file is valid
  FILE *f = fopen(file_name, "r");
  if (!f)
    return FILE_NOT_FOUND;
  fclose(f);

  if (key == nullptr)
    return MISSING_KEY;

  if (threads == nullptr)
    return MISSING_THREADS;

  if (server_ip == nullptr)
    return MISSING_SERVER_IP;

  if (server_port == nullptr)
    return MISSING_SERVER_PORT;

  // now convert from strings to valid types
  char *end_key;
  long _key = strtol(key, &end_key, BASE_10);
  if (errno != 0 || *end_key != '\0' || _key < 1 || _key > INT_MAX)
    return MALFORMED_KEY;

  char *end_threads;
  long _threads = strtol(threads, &end_threads, BASE_10);
  if (errno != 0 || *end_threads != '\0' || _threads < 1 || _threads > INT_MAX)
    return MALFORMED_THREADS;

  char *end_server_port;
  long _server_port = strtol(server_port, &end_server_port, BASE_10);
  if (errno != 0 || *end_server_port != '\0' || _server_port < MIN_PORT ||
      _server_port > MAX_PORT)
    return MALFORMED_SERVER_PORT;

  // if here then the values can be safely casted and saved
  out->file_path = file_name;
  out->key = _key;
  out->threads = _threads;
  out->server_ip = server_ip;
  out->server_port = _server_port;

  return OK;
}

void print_usage(const char *program_name) {
  fprintf(stderr,
          "Usage:\n"
          "  %s -f <input> -k <key> -t <threads> -a <server ip> -p <server "
          "port> [-h]\n"
          "\n"
          "Options:\n"
          "  -f <input>       input file\n"
          "  -k <key>         the encryption key to use\n"
          "  -t <threads>     number of threads to use for encryption\n"
          "  -a <server ip>   server ip\n"
          "  -P <server port> server port\n"
          "  -h               show this help (ignores everything else)\n",
          program_name);
}

char *pa_result_to_string(const PAResult result) {
  switch (result) {
  case OK:
    return "success";
  case UNKNOWN_FLAG:
    return "unknown flag";
  case WRONG_USAGE:
    return "args could not be parsed";
  case ONLY_HELP:
    return ""; // the actual message is printed by `print_usage`

  case MISSING_FILE:
    return "must specify a file";
  case FILE_NOT_FOUND:
    return "file could not be found";
  case MULTIPLE_FILES:
    return "must specify only 1 file";

  case MISSING_KEY:
    return "must specify the key to use";
  case MALFORMED_KEY:
    return "key is invalid";
  case MULTIPLE_KEYS:
    return "must specify only 1 key to use";

  case MISSING_THREADS:
    return "must specify the number of threads";
  case MALFORMED_THREADS:
    return "number of threads is invalid";
  case MULTIPLE_THREADS:
    return "must specify the number of threads only once";

  case MISSING_SERVER_IP:
    return "must specify the server IP";
  case MULTIPLE_SERVER_IP:
    return "must specify the server IP only once";

  case MISSING_SERVER_PORT:
    return "must specify the server port";
  case MALFORMED_SERVER_PORT:
    return "server port is invalid";
  case MULTIPLE_SERVER_PORT:
    return "must specify the server port only once";
  }

  // effectively unreachable but makes compiler happy
  return "unknown error";
}
