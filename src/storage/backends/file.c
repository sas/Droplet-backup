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

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <storage/storage.h>
#include <utils/buffer.h>
#include <utils/diefuncs.h>

#include "file.h"

struct file_storage_state
{
  const char *remote_root;
  DIR *last_list;
};

static enum store_res sto_file_store_file(void *state, const char *path, FILE *file)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];
  int fd = -1;
  char file_buf[4096];
  size_t file_buf_size;
  int size, full_size;

  snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

  fseek(file, 0, SEEK_SET);

  if ((fd = open(full_path, O_WRONLY | O_EXCL | O_CREAT, 0666)) == -1)
  {
    /*
    ** If file already exists, this means that the data is already stored, so
    ** this is not an actual failure.
    */
    if (errno == EEXIST)
      return STORE_EXISTS;
    else
      goto err;
  }

  while ((file_buf_size = fread(file_buf, 1, 4096, file)) > 0)
  {
    full_size = 0;
    while ((size = write(fd, file_buf + full_size, file_buf_size - full_size)) > 0)
      full_size += size;

    if (size == -1)
      goto err;
  }

  if (ferror(file))
    goto err;

  close(fd);

  return STORE_SUCCESS;

err:
  if (fd != -1)
    close(fd);
  return STORE_FAILURE;
}

static enum store_res sto_file_store_buffer(void *state, const char *path, struct buffer *buffer)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];
  int fd = -1;
  int size, full_size;

  snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

  if ((fd = open(full_path, O_WRONLY | O_EXCL | O_CREAT, 0666)) == -1)
  {
    /*
    ** If file already exists, this means that the data is already stored, so
    ** this is not an actual failure.
    */
    if (errno == EEXIST)
      return STORE_EXISTS;
    else
      goto err;
  }

  full_size = 0;
  while ((size = write(fd, buffer->data + full_size, buffer->used - full_size)) > 0)
    full_size += size;

  if (size == -1)
    goto err;

  close(fd);

  return STORE_SUCCESS;

err:
  if (fd != -1)
    close(fd);
  return STORE_FAILURE;
}

static FILE *sto_file_retrieve_file(void *state, const char *path)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];
  FILE *res = NULL;
  int fd = -1;
  char file_buf[4096];
  int file_buf_size;
  size_t size, full_size;

  snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

  if ((fd = open(full_path, O_RDONLY)) == -1)
    goto err;

  res = etmpfile();

  while ((file_buf_size = read(fd, file_buf, 4096)) > 0)
  {
    full_size = 0;
    while ((size = fwrite(file_buf + full_size, 1, file_buf_size - full_size, res)) > 0)
      full_size += size;

    if (ferror(res))
      goto err;
  }

  if (file_buf_size == -1)
    goto err;

  fseek(res, 0, SEEK_SET);

  close(fd);

  return res;

err:
  if (fd != -1)
    close(fd);
  if (res != NULL)
    fclose(res);
  return NULL;
}

static struct buffer *sto_file_retrieve_buffer(void *state, const char *path)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];
  struct buffer *res = NULL;
  int size;
  int fd = -1;
  struct stat buf;

  snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

  if ((fd = open(full_path, O_RDONLY)) == -1)
    goto err;

  if (fstat(fd, &buf) == -1)
    goto err;

  res = buffer_new(buf.st_size);

  res->used = 0;
  while ((size = read(fd, res->data + res->used, res->size - res->used)) > 0)
    res->used += size;

  if (size == -1)
    goto err;

  close(fd);

  return res;

err:
  if (fd != -1)
    close(fd);
  if (res != NULL)
    free(res);
  return NULL;
}

static const char *sto_file_list(void *state, const char *path)
{
  struct file_storage_state *s = state;
  struct dirent *dir;

  if (path != NULL)
  {
    char full_path[strlen(s->remote_root) + strlen(path) + 2];

    snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

    if (s->last_list != NULL)
      closedir(s->last_list);

    if ((s->last_list = opendir(full_path)) == NULL)
      return NULL;
  }

  if (s->last_list == NULL)
    return NULL;

  if ((dir = readdir(s->last_list)) != NULL)
  {
    if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
      return sto_file_list(state, NULL);
    else
      return dir->d_name;
  }
  else
    return NULL;
}

static bool sto_file_unlink(void *state, const char *path)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];

  snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

  return unlink(full_path) == 0;
}

static bool sto_file_exists(void *state, const char *path)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];

  snprintf(full_path, sizeof (full_path), "%s/%s", s->remote_root, path);

  return access(full_path, F_OK) == 0;
}

static void sto_file_delete(void *state)
{
  struct file_storage_state *s = state;

  if (s->last_list != NULL)
    closedir(s->last_list);
  free(s);
}

struct storage *sto_file_new(const char *uri, bool create_dirs)
{
  struct storage *res = NULL;
  struct file_storage_state *state = NULL;
  /* strlen("backups") == strlen("objects") == strlen(".dplbck") */
  char path[strlen(uri) + strlen("backups") + 2];
  int fd;

  res = emalloc(sizeof (struct storage));
  state = emalloc(sizeof (struct file_storage_state));

  res->delete = sto_file_delete;
  res->store_file = sto_file_store_file;
  res->store_buffer = sto_file_store_buffer;
  res->retrieve_file = sto_file_retrieve_file;
  res->retrieve_buffer = sto_file_retrieve_buffer;
  res->list = sto_file_list;
  res->unlink = sto_file_unlink;
  res->exists = sto_file_exists;
  state->remote_root = uri;
  state->last_list = NULL;
  res->state = state;

  /*
  ** Ensure we have access to the directory where we want to backup, and create
  ** required subdirectories (e.g.: `backups`, `objects`) if they do not exist
  ** yet.
  */
  if (create_dirs)
  {
    snprintf(path, sizeof (path), "%s", state->remote_root);
    if (mkdir(path, 0777) == -1 && errno != EEXIST)
      goto err;

    snprintf(path, sizeof (path), "%s/%s", state->remote_root, "backups");
    if (mkdir(path, 0777) == -1 && errno != EEXIST)
      goto err;

    snprintf(path, sizeof (path), "%s/%s", state->remote_root, "objects");
    if (mkdir(path, 0777) == -1 && errno != EEXIST)
      goto err;

    snprintf(path, sizeof (path), "%s/%s", state->remote_root, ".dplbck");
    /*
    ** We could replace this with an mknod(), but the manpage states that "The
    ** only portable use of mknod() is to create a FIFO-special file".
    */
    if ((fd = open(path, O_CREAT | O_RDONLY, 0666)) == -1)
      goto err;
    close(fd);
  }
  else
  {
    snprintf(path, sizeof (path), "%s/%s", state->remote_root, ".dplbck");
    if (access(path, F_OK) == -1)
      goto err;
  }

  return res;

err:
  if (res != NULL)
    free(res);
  if (state != NULL)
    free(state);
  return NULL;
}
