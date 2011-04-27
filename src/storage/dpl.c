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
#include <utils/rollsum.h>

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
  struct dpl_storage_state *s = state;
  dpl_vfile_t *vfile;
  size_t file_size;
  char file_buf[4096];
  int file_buf_size;

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (path != NULL)
  {
    char full_path[strlen(s->remote_root) + strlen(path) + 2];

    snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

    if (dpl_openwrite(s->ctx, full_path, DPL_VFILE_FLAG_CREAT | DPL_VFILE_FLAG_EXCL,
                      NULL, DPL_CANNED_ACL_AUTHENTICATED_READ, file_size,
                      &vfile) != DPL_SUCCESS)
      return 0;

    while ((file_buf_size = fread(file_buf, 1, 4096, file)) > 0)
    {
      if (dpl_write(vfile, file_buf, file_buf_size) != DPL_SUCCESS)
      {
        dpl_close(vfile);
        return 0;
      }
    }

    dpl_close(vfile);
    return 1;
  }

  return 0;
}

static int sto_dpl_store_buffer(void *state, const char *path, struct buffer *buffer)
{
  (void) state;
  (void) path;
  (void) buffer;

  return 0;
}

static int sto_dpl_retrieve(struct dpl_storage_state *s, const char *path,
                            dpl_buffer_func_t cb, void *cb_arg)
{
  if (path != NULL)
  {
    char full_path[strlen(s->remote_root) + strlen(path) + 2];
    snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

    if (dpl_openread(s->ctx, full_path, 0, NULL, cb, cb_arg, NULL) != DPL_SUCCESS)
      return 0;

    return 1;
  }

  return 0;
}

static FILE *sto_dpl_retrieve_file(void *state, const char *path)
{
  (void) state;
  (void) path;

  return NULL;
}

static dpl_status_t sto_dpl_retrieve_buffer_cb(void *arg, char *data, unsigned int len)
{
  struct buffer *buf = arg;

  if (len + buf->used > buf->size)
    return DPL_FAILURE;

  memcpy(buf->data + buf->used, data, len);
  buf->used += len;
  return DPL_SUCCESS;
}

static struct buffer *sto_dpl_retrieve_buffer(void *state, const char *path)
{
  struct buffer *buf = buffer_new(ROLLSUM_MAXSIZE);

  if (!sto_dpl_retrieve(state, path, sto_dpl_retrieve_buffer_cb, buf))
  {
    buffer_delete(buf);
    return NULL;
  }

  return buf;
}

static const char *sto_dpl_list(void *state, const char *path)
{
  struct dpl_storage_state *s = state;

  if (path != NULL)
  {
    char full_path[strlen(s->remote_root) + strlen(path) + 2];
    snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

    if (s->last_list != NULL)
      dpl_closedir(s->last_list);

    if (dpl_opendir(s->ctx, full_path, &s->last_list) != DPL_SUCCESS)
      return NULL;
  }

  if (s->last_list == NULL)
    return NULL;

  if (dpl_eof(s->last_list))
    return NULL;

  if (dpl_readdir(s->last_list, &s->last_dirent) != DPL_SUCCESS)
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
