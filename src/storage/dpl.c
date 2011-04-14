/*
**
** Copyright (c) 2011, Stephane Sezer
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of Stephane Sezer nor the names of its contributors
**       may be used to endorse or promote products derived from this software
**       without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL Stephane Sezer BE LIABLE FOR ANY DIRECT,
** INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*/

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
  (void) create_dirs;
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

  res->store_file = NULL;
  res->store_buffer = NULL;
  res->retrieve_file = NULL;
  res->retrieve_buffer = NULL;
  res->list = NULL;
  res->delete = NULL;
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
