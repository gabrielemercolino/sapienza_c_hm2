#include "client/args.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  ClientConfig config = {0};
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

  printf("Hello from client\n");
  printf("Config:\n  -k: %lu\n  -t: %lu\n  -f: '%s'\n  -a: '%s'\n  -p: %hu\n",
         config.key, config.threads, config.file_path, config.server_ip,
         config.server_port);

  return 0;
}
