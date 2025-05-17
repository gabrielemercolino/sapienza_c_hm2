#include "args.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  ServerConfig config = {0};
  PAResult result = parse_args(argc, argv, &config);

  if (result == ONLY_HELP) {
    print_usage(argv[0]);
    return 0;
  }

  if (result != OK) {
    fprintf(stderr, "%s\n", pa_result_to_string(result));
    print_usage(argv[0]);
    return 1;
  }

  printf("Hello form server\n");
  printf("Config:\n  -t: %lu\n  -p: '%s'\n  -c: %d\n", config.threads,
         config.file_prefix, config.max_connections);

  return 0;
}
