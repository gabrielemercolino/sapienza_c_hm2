#include "server/args.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *argv_flat;
  SPAResult expected;
} TestCase;

const TestCase cases[] = {
    {
        .argv_flat = "program",
        .expected = MISSING_THREADS,
    },
    {
        .argv_flat = "program -x idk",
        .expected = UNKNOWN_FLAG,
    },
    {
        .argv_flat = "program -t xyz",
        .expected = MALFORMED_THREADS,
    },
    {
        .argv_flat = "program -t -1",
        .expected = MALFORMED_THREADS,
    },
    {
        .argv_flat = "program -t 1 -p 123",
        .expected = MALFORMED_FILE_PREFIX,
    },
    {
        .argv_flat = "program -t 1 -p abc",
        .expected = MISSING_MAX_CONNECTIONS,
    },
    {
        .argv_flat = "program -t 1 -p abc -c abc",
        .expected = MALFORMED_MAX_CONNECTIONS,
    },
    {
        .argv_flat = "program -t 1 -p abc -c 2",
        .expected = OK,
    },
};

char **split(const char *str, int *tokens_size);

int main() {
  size_t test_number = sizeof(cases) / sizeof(*cases);
  for (size_t i = 0; i < test_number; i++) {
    // reset globals

    optind = 0;
    errno = 0;

    const TestCase test_case = cases[i];
    int argc_case;
    char **argv_case = split(test_case.argv_flat, &argc_case);
    ServerConfig __c;

    const SPAResult result = server_parse_args(argc_case, argv_case, &__c);

    if (result != test_case.expected) {
      printf("Test case %zu, wrong result\n\texpected: %s\n\tgot: %s\n", i,
             server_pa_result_to_string(test_case.expected),
             server_pa_result_to_string(result));
      printf("argv:\n");
      for (int j = 0; j < argc_case; j++) {
        printf("\t%s\n", argv_case[j]);
      }
      return 1;
    }

    for (int j = 0; j < argc_case; j++)
      free(argv_case[j]);
    free(argv_case);
  }

  return 0;
}

char **split(const char *str, int *tokens_size) {
  char **tokens = nullptr;
  *tokens_size = 0;

  char *copy = strdup(str);
  const char *token = strtok(copy, " ");

  while (token != nullptr) {
    tokens = realloc(tokens, sizeof(char *) * (*tokens_size + 1));
    tokens[*tokens_size] = strdup(token);
    *tokens_size += 1;
    token = strtok(nullptr, " ");
  }

  free(copy);
  return tokens;
}
