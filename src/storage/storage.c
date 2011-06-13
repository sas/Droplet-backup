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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <storage/dpl.h>
#include <storage/file.h>
#include <utils/options.h>
#include <utils/stats.h>

#include "storage.h"

storage_t storage_new(const char *uri, bool create_dirs)
{
  static const struct
  {
    const char *scheme;
    storage_t (*initializer)(const char *uri, bool create_dirs);
  } inits[] = {
    { "dpl://",   sto_dpl_new },
    { "file://",  sto_file_new },
  };

  for (unsigned int i = 0; i < sizeof (inits) / sizeof (inits[0]); ++i)
    if (strncmp(uri, inits[i].scheme, strlen(inits[i].scheme)) == 0)
      return inits[i].initializer(uri + strlen(inits[i].scheme), create_dirs);

  return NULL;
}

bool storage_store_file(storage_t storage, const char *path, FILE *file)
{
  enum store_res ret;

  ret = storage->store_file(storage->state, path, file);

  if (ret == STORE_FAILURE)
    return false;

  stats_log_transaction();

  if (ret != STORE_EXISTS)
  {
    int fd = fileno(file);
    struct stat buf;

    if (fstat(fd, &buf) == 0)
      stats_log_tx_bytes(buf.st_size);
  };

  return true;
}

bool storage_store_buffer(storage_t storage, const char *path, struct buffer *buffer)
{
  enum store_res ret;

  ret = storage->store_buffer(storage->state, path, buffer);

  if (ret == STORE_FAILURE)
    return false;

  stats_log_transaction();

  if (ret != STORE_EXISTS)
    stats_log_tx_bytes(buffer->used);

  return true;
}

FILE *storage_retrieve_file(storage_t storage, const char *path)
{
  FILE *res;

  res = storage->retrieve_file(storage->state, path);

  if (res == NULL)
    return NULL;

  stats_log_transaction();

  int fd = fileno(res);
  struct stat buf;

  if (fstat(fd, &buf) == 0)
    stats_log_rx_bytes(buf.st_size);

  return res;
}

struct buffer *storage_retrieve_buffer(storage_t storage, const char *path)
{
  struct buffer *res;

  res = storage->retrieve_buffer(storage->state, path);

  if (res == NULL)
    return NULL;

  stats_log_transaction();
  
  stats_log_rx_bytes(res->used);

  return res;
}

const char *storage_list(storage_t storage, const char *path)
{
  stats_log_transaction();
  return storage->list(storage->state, path);
}

bool storage_unlink(storage_t storage, const char *path)
{
  stats_log_transaction();
  return storage->unlink(storage->state, path);
}

void storage_delete(storage_t storage)
{
  storage->delete(storage->state);
  free(storage);
}
