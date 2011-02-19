#include <stdio.h>

#include "backup.h"

int cmd_backup(int argc, char *argv[])
{
  printf("executing command:");

  for (int i = 0; i < argc; ++i)
    printf(" %s", argv[i]);
  printf("\n");

  return 0;
}
