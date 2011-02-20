#include <droplet.h>
#include <stdlib.h>
#include <string.h>

#include <storage/storage.h>

#include "dpl.h"

struct storage *sto_dpl_new(const char *uri)
{
  struct storage *res = NULL;
  dpl_ctx_t *ctx = NULL;
  char *wuri = NULL;

  if (dpl_init() != DPL_SUCCESS)
    goto err;

  /* XXX: We should use --profile-dir and --profile here. */
  if ((ctx = dpl_ctx_new(NULL, NULL)) == NULL)
    goto err;

  if ((res = malloc(sizeof (struct storage))) == NULL)
    goto err;

  if ((wuri = strdup(uri)) == NULL)
    goto err;
  if ((ctx->cur_bucket = strsep(&wuri, "/")) == NULL)
    goto err;

  res->store_file = NULL;
  res->get_file = NULL;
  res->local_root = NULL;
  res->remote_root = wuri;
  res->specific_data = ctx;

  return res;

err:
  if (res != NULL)
    free(res);
  if (ctx != NULL)
    dpl_ctx_free(ctx);
  return NULL;
}
