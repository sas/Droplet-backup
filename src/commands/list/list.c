#include <stdio.h>

#include <storage/storage.h>
#include <usage.h>

#include "list.h"

int cmd_list(int argc, char *argv[])
{
  if (argc != 2)
    usage_die();

  struct storage *storage = storage_new(argv[1]);

  return 0;
}
