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
#include <utils/diefuncs.h>

#include "dpl.h"

struct dpl_storage_state
{
  char *remote_root;
  dpl_ctx_t *ctx;
  void *last_list;
  dpl_dirent_t last_dirent;
};

static int sto_dpl_store_file(void *state, const char *path, FILE *file)
{
  (void) state;
  (void) path;
  (void) file;

  return 0;
}

static int sto_dpl_store_buffer(void *state, const char *path, struct buffer *buffer)
{
  (void) state;
  (void) path;
  (void) buffer;

  return 0;
}

static FILE *sto_dpl_retrieve_file(void *state, const char *path)
{
  (void) state;
  (void) path;

  return NULL;
}

static struct buffer *sto_dpl_retrieve_buffer(void *state, const char *path)
{
  (void) state;
  (void) path;

  return NULL;
}

static const char *sto_dpl_list(void *state, const char *path)
{
  struct dpl_storage_state *s = state;

  if (path != NULL)
  {
    char full_path[strlen(s->remote_root) + strlen(path) + 2];

    strcpy(full_path, s->remote_root);
    strcat(full_path, "/");
    strcat(full_path, path);

    if (s->last_list != NULL)
      dpl_closedir(s->last_list);

    if (dpl_opendir(s->ctx, full_path, &s->last_list) == DPL_FAILURE)
      return NULL;
  }

  if (s->last_list == NULL)
    return NULL;

  if (dpl_eof(s->last_list))
    return NULL;

  if (dpl_readdir(s->last_list, &s->last_dirent) == DPL_FAILURE)
    return NULL;

  if (strcmp(s->last_dirent.name, ".") == 0 || strcmp(s->last_dirent.name, "..") == 0)
    return sto_dpl_list(state, NULL);
  else
    return s->last_dirent.name;
}

static void sto_dpl_delete(void *state)
{
  struct dpl_storage_state *s = state;

  dpl_ctx_free(s->ctx);
  free(s);
}

storage_t sto_dpl_new(const char *uri, int create_dirs)
{
  struct storage *res = NULL;
  struct dpl_storage_state *state = NULL;
  dpl_ctx_t *ctx = NULL;
  /* strlen("/backups") == strlen("/objects") == strlen("/.dplbck") */
  char path[strlen(uri) + strlen("/backups") + 1];
  dpl_status_t ret;
  dpl_dict_t *dict;

  if (dpl_init() != DPL_SUCCESS)
    goto err;

  /* XXX: We should use --profile-dir and --profile here. */
  if ((ctx = dpl_ctx_new(NULL, NULL)) == NULL)
    goto err;

  res = emalloc(sizeof (struct storage));
  state = emalloc(sizeof (struct dpl_storage_state));

  state->remote_root = estrdup(uri);
  if ((ctx->cur_bucket = strsep(&state->remote_root, "/")) == NULL)
    goto err;

  res->store_file = sto_dpl_store_file;
  res->store_buffer = sto_dpl_store_buffer;
  res->retrieve_file = sto_dpl_retrieve_file;
  res->retrieve_buffer = sto_dpl_retrieve_buffer;
  res->list = sto_dpl_list;
  res->delete = sto_dpl_delete;
  state->ctx = ctx;
  res->state = state;

  /*
  ** Ensure we have access to the directory where we want to backup, and create
  ** required subdirectories (e.g.: `backups`, `objects`) if they do not exist
  ** yet.
  */
  if (create_dirs)
  {
    strcpy(path, state->remote_root);
    ret = dpl_mkdir(ctx, path);
    if (ret != DPL_SUCCESS && ret != DPL_EEXIST)
      goto err;

    strcat(path, "/backups");
    ret = dpl_mkdir(ctx, path);
    if (ret != DPL_SUCCESS && ret != DPL_EEXIST)
      goto err;

    strcpy(path, state->remote_root);
    strcat(path, "/objects");
    ret = dpl_mkdir(ctx, path);
    if (ret != DPL_SUCCESS && ret != DPL_EEXIST)
      goto err;

    strcpy(path, state->remote_root);
    strcat(path, "/.dplbck");
    ret = dpl_mknod(ctx, path);
    if (ret != DPL_SUCCESS && ret != DPL_EEXIST)
      goto err;
  }
  else
  {
    strcpy(path, state->remote_root);
    strcat(path, "/.dplbck");
    ret = dpl_head(ctx, ctx->cur_bucket, path, NULL, NULL, &dict);
    if (ret != DPL_SUCCESS)
      goto err;
    free(dict);
  }

  return res;

err:
  if (state != NULL)
    free(state);
  if (res != NULL)
    free(res);
  if (ctx != NULL)
    dpl_ctx_free(ctx);
  return NULL;
}
