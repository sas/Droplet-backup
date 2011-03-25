#include <stdio.h>

#include <storage/storage.h>
#include <usage.h>

#include "list.h"

int cmd_list(int argc, char *argv[])
{
  if (argc != 2)
    usage_die();

  struct storage *storage = NULL;
  const char *elem;

  if ((storage = storage_new(argv[1], 0)) == NULL)
    err(EXIT_STORAGE_FAIL, "unable to open storage: %s\n", argv[1]);

  if ((elem = storage_list(storage, "backups")) != NULL)
    printf("%s\n", elem);
  while ((elem = storage_list(storage, NULL)) != NULL)
    printf("%s\n", elem);

  storage_delete(storage);

  return 0;
}
