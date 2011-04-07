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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <storage/storage.h>
#include <utils/messages.h>

#include "file.h"

struct file_storage_state
{
  const char *remote_root;
  DIR *last_list;
};

static int sto_file_store(void *state, const char *path, struct buffer *data)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 1];
  int fd = -1;
  int size, full_size;

  strcpy(full_path, s->remote_root);
  strcat(full_path, "/");
  strcat(full_path, path);

  if ((fd = open(full_path, O_WRONLY | O_EXCL | O_CREAT, 0666)) == -1)
  {
    /*
    ** If file already exists, this means that the data is already stored, so
    ** this is not an actual failure.
    */
    if (errno == EEXIST)
      return 1;
    else
      goto err;
  }

  full_size = 0;
  while ((size = write(fd, data->data + full_size, data->size - full_size)) > 0)
    full_size += size;

  if (size == -1)
    goto err;

  close(fd);

  return 1;

err:
  if (fd != -1)
    close(fd);
  return 0;
}

static struct buffer *sto_file_retrieve(void *state, const char *path)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];
  struct buffer *res = NULL;
  int size, full_size;
  int fd = -1;
  struct stat buf;

  strcpy(full_path, s->remote_root);
  strcat(full_path, "/");
  strcat(full_path, path);

  if ((fd = open(full_path, O_RDONLY)) == -1)
    goto err;

  if (fstat(fd, &buf) == -1)
    goto err;

  if ((res = malloc(sizeof (struct buffer) + buf.st_size)) == NULL)
    goto err;

  full_size = 0;
  while ((size = read(fd, res->data + full_size, res->size - full_size)) > 0)
    full_size += size;

  if (size == -1)
    goto err;

  close(fd);

  return res;

err:
  if (fd != -1)
    close(fd);
  if (res != NULL)
    free(NULL);
  return NULL;
}

static const char *sto_file_list(void *state, const char *path)
{
  struct file_storage_state *s = state;
  struct dirent *dir;

  if (path != NULL)
  {
    char full_path[strlen(s->remote_root) + strlen(path) + 1];

    strcpy(full_path, s->remote_root);
    strcat(full_path, "/");
    strcat(full_path, path);

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

static void sto_file_delete(void *state)
{
  struct file_storage_state *s = state;

  if (s->last_list != NULL)
    closedir(s->last_list);
  free(s);
}

struct storage *sto_file_new(const char *uri, int create_dirs)
{
  struct storage *res = NULL;
  struct file_storage_state *state = NULL;
  /* strlen("/backups") == strlen("/objects") == strlen("/.dplbck") */
  char path[strlen(uri) + strlen("/backups") + 1];
  int fd;


  if ((res = malloc(sizeof (struct storage))) == NULL)
    goto err;
  if ((state = malloc(sizeof (struct file_storage_state))) == NULL)
    goto err;

  res->store = sto_file_store;
  res->retrieve = sto_file_retrieve;
  res->list = sto_file_list;
  res->delete = sto_file_delete;
  state->remote_root = uri;
  state->last_list = NULL;
  res->state = state;

  /*
  ** Ensure we have access to the directory where we want to backup, and create
  ** required subdirectories (e.g.: `backups`, `objects`) if they do not exist
  ** yet.
  ** XXX: Some parts of this code are a little stupid.
  */
  if (create_dirs)
  {
    strcpy(path, state->remote_root);
    if (mkdir(path, 0777) == -1)
      if (errno != EEXIST)
        goto err;

    strcat(path, "/backups");
    if (mkdir(path, 0777) == -1)
      if (errno != EEXIST)
        goto err;

    strcpy(path, state->remote_root);
    strcat(path, "/objects");
    if (mkdir(path, 0777) == -1)
      if (errno != EEXIST)
        goto err;

    strcpy(path, state->remote_root);
    strcat(path, "/.dplbck");
    if ((fd = open(path, O_CREAT | O_RDONLY, 0666)) == -1)
      goto err;
    close(fd);
  }
  else
  {
    strcpy(path, state->remote_root);
    strcat(path, "/.dplbck");
    if ((fd = open(path, O_RDONLY)) == -1)
      goto err;
    close(fd);
  }

  return res;

err:
  if (res != NULL)
    free(res);
  if (state != NULL)
    free(state);
  return NULL;
}
