#include <stdio.h>

#include <storage/storage.h>

#include "list.h"

#include <droplet.h>

int cmd_list(int argc, char *argv[])
{
  if (argc != 2)
    return 0;

  struct storage *storage = storage_new(argv[1]);

  printf("%s\n", storage->remote_root);
  printf("%s\n", ((dpl_ctx_t *)storage->specific_data)->cur_bucket);

  return 0;
}
