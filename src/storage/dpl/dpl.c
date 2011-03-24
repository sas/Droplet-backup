#define STORAGE_INTERNAL

#include <droplet.h>
#include <stdlib.h>
#include <string.h>

#include <storage/storage.h>

#include "dpl.h"

struct dpl_storage_state
{
  char *remote_root;
  dpl_ctx_t *ctx;
};

storage_t sto_dpl_new(const char *uri, int create_dirs)
{
  struct storage *res = NULL;
  struct dpl_storage_state *state = NULL;
  dpl_ctx_t *ctx = NULL;
  char *wuri = NULL;

  if (dpl_init() != DPL_SUCCESS)
    goto err;

  /* XXX: We should use --profile-dir and --profile here. */
  if ((ctx = dpl_ctx_new(NULL, NULL)) == NULL)
    goto err;

  if ((res = malloc(sizeof (struct storage))) == NULL)
    goto err;
  if ((state = malloc(sizeof (struct dpl_storage_state))) == NULL)
    goto err;

  if ((wuri = strdup(uri)) == NULL)
    goto err;
  if ((ctx->cur_bucket = strsep(&wuri, "/")) == NULL)
    goto err;

  res->store = NULL;
  res->retrieve = NULL;
  res->list = NULL;
  state->remote_root = wuri;
  state->ctx = ctx;
  res->state = state;

  return res;

err:
  if (ctx != NULL)
    dpl_ctx_free(ctx);
  if (res != NULL)
    free(res);
  if (state != NULL)
    free(state);
  if (wuri != NULL)
    free(wuri);
  return NULL;
}
