#include "args.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  ServerConfig config = {0};
  PAResult result = server_parse_args(argc, argv, &config);

  if (result == ONLY_HELP) {
    server_print_usage(argv[0]);
    return 0;
  }

  if (result != OK) {
    fprintf(stderr, "%s\n", server_pa_result_to_string(result));
    server_print_usage(argv[0]);
    return 1;
  }

  printf("Hello form server\n");
  printf("Config:\n  -t: %lu\n  -p: '%s'\n  -c: %d\n", config.threads,
         config.file_prefix, config.max_connections);

  return 0;
}
