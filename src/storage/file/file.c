#define STORAGE_INTERNAL

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <storage/storage.h>

#include "file.h"

struct file_storage_state
{
  char *remote_root;
};

struct storage *sto_file_new(const char *uri)
{
  struct storage *res = NULL;
  struct file_storage_state *state = NULL;
  char *wuri = NULL;

  if ((res = malloc(sizeof (struct storage))) == NULL)
    goto err;
  if ((state = malloc(sizeof (struct file_storage_state))) == NULL)
    goto err;

  if ((wuri = strdup(uri)) == NULL)
    goto err;

  res->store = NULL;
  res->retrieve = NULL;
  res->list = NULL;
  state->remote_root = wuri;
  res->state = state;

  return res;

err:
  if (res != NULL)
    free(res);
  if (state != NULL)
    free(state);
  if (wuri != NULL)
    free(wuri);
  return NULL;
}
