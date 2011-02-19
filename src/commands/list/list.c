#include <stdio.h>

#include "list.h"

int cmd_list(int argc, char *argv[])
{
  printf("executing command:");

  for (int i = 0; i < argc; ++i)
    printf(" %s", argv[i]);
  printf("\n");

  return 0;
}
