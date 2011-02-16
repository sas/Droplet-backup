#include <stdio.h>

#include "stats.h"

int cmd_stats(int argc, char *argv[])
{
  printf("executing command:");

  for (int i = 0; i < argc; ++i)
    printf(" %s", argv[i]);
  printf("\n");

  return 0;
}
