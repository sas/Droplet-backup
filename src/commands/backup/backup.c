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

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <storage/storage.h>
#include <utils/digest.h>
#include <utils/messages.h>
#include <utils/rollsum.h>

#include "backup.h"

static void hash_dispatch(storage_t storage, FILE *backup, const char *path);

/*
** This function takes a path, and a directory to go into. (Used when entering a
** directory)
*/
static char *path_down(const char *path, const char *elem)
{
  unsigned int path_len = strlen(path);
  unsigned int elem_len = strlen(elem);
  char *res;

  if ((res = malloc(path_len + elem_len + 2)) == NULL)
    err(EXIT_FAILURE, "malloc()");

  strcpy(res, path);
  res[path_len] = '/';
  strcpy(res + path_len + 1, elem);

  return res;
}

static const char *hash_file(storage_t storage, const char *path, FILE *file)
{
  const char *res;
  FILE *tmp;
  struct buffer *buf;
  struct rollsum rs;
  char file_buf[4096];
  int file_buf_cnt;
  int file_buf_idx;
  char *upload_path;

  if ((tmp = tmpfile()) == NULL)
    err(EXIT_FAILURE, "tmpfile()");

  buf = buffer_new(ROLLSUM_MAXSIZE);
  rollsum_init(&rs);

  while ((file_buf_cnt = fread(file_buf, 1, 4096, file)) > 0)
  {
    file_buf_idx = 0;

    while (file_buf_idx < file_buf_cnt)
    {
      rollsum_roll(&rs, file_buf[file_buf_idx]);
      buf->data[buf->used++] = file_buf[file_buf_idx++];

      if (rollsum_onbound(&rs))
      {
        res = digest_buffer(buf);
        upload_path = path_down("objects", res);
        if (!storage_store_buffer(storage, upload_path, buf))
          errx(EXIT_FAILURE, "unable to store file: %s", path);
        free(upload_path);
        fprintf(tmp, "%s\n", res);

        rollsum_init(&rs);
        buf->used = 0;
      }
    }
  }

  if (file_buf_cnt == -1)
  {
    err(EXIT_FAILURE, "%s", path);
  }
  else
  {
    res = digest_buffer(buf);
    upload_path = path_down("objects", res);
    if (!storage_store_buffer(storage, upload_path, buf))
      errx(EXIT_FAILURE, "unable to store file: %s", path);
    free(upload_path);
    fprintf(tmp, "%s\n", res);
  }

  res = digest_file(tmp);
  upload_path = path_down("objects", res);
  if (!storage_store_file(storage, upload_path, tmp))
    errx(EXIT_FAILURE, "unable to store file: %s", path);
  free(upload_path);

  buffer_delete(buf);
  fclose(tmp);

  return res;
}

static const char *hash_directory(storage_t storage, const char *path, DIR *dir)
{
  const char *res;
  FILE *tmp;
  struct dirent *ent;
  char *upload_path;

  if ((tmp = tmpfile()) == NULL)
    err(EXIT_FAILURE, "tmpfile()");

  while ((ent = readdir(dir)) != NULL)
  {
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
      continue;

    char *new_path = path_down(path, ent->d_name);
    hash_dispatch(storage, tmp, new_path);
    free(new_path);
  }

  res = digest_file(tmp);
  upload_path = path_down("objects", res);
  if (!storage_store_file(storage, upload_path, tmp))
    errx(EXIT_FAILURE, "unable to store file: %s", path);
  free(upload_path);

  fclose(tmp);

  return res;
}

/*
** We could have been using libgen.h's basename() function, but the manpage says
** that "Both dirname() and basename() may modify the contents of path". So we
** better use our own function.
*/
static const char *const_basename(const char *path)
{
  const char *res = path;

  while (*path != 0)
  {
    if (*path == '/')
      res = path + 1;

    ++path;
  }

  return res;
}

static void hash_dispatch(storage_t storage, FILE *backup, const char *path)
{
  struct stat buf;
  const char *hash = NULL;

  if (lstat(path, &buf) == -1)
  {
    warn("unable to access %s", path);
    return;
  }

  if (S_ISREG(buf.st_mode))
  {
    FILE *file;

    if ((file = fopen(path, "rb")) == NULL)
    {
      warn("unable to access %s", path);
      return;
    }

    hash = hash_file(storage, path, file);

    fclose(file);
  }
  else if (S_ISDIR(buf.st_mode))
  {
    DIR *dir;

    if ((dir = opendir(path)) == NULL)
    {
      warn("unable to access %s", path);
      return;
    }

    hash = hash_directory(storage, path, dir);

    closedir(dir);
  }
  else if (S_ISLNK(buf.st_mode))
  {
    warnx("%s: backuping symbolic links is not supported yet", path);
    return;
  }
  else if (S_ISCHR(buf.st_mode))
  {
    warnx("%s: is a character device, not backuping", path);
    return;
  }
  else if (S_ISBLK(buf.st_mode))
  {
    warnx("%s: is a block device, not backuping", path);
    return;
  }
  else if (S_ISFIFO(buf.st_mode))
  {
    warnx("%s: is a named pipe, not backuping", path);
    return;
  }
  else if (S_ISSOCK(buf.st_mode))
  {
    warnx("%s: is a socket, not backuping", path);
    return;
  }
  else
  {
    warnx("unknown file type: %s, not backuping", path);
    return;
  }

  if (hash != NULL)
    fprintf(backup, "%s %u %u %04o %s\n",
        hash,
        buf.st_uid,
        buf.st_gid,
        buf.st_mode & 07777,
        const_basename(path)
    );
}

int cmd_backup(int argc, char *argv[])
{
  storage_t storage;
  FILE *backup;
  char *upload_path;
  const char *backup_hash;

  /* XXX: No options parsing for the moment. */
  if (argc < 3)
    usage_die();

  if ((storage = storage_new(argv[1], 1)) == NULL)
    errx(EXIT_FAILURE, "unable to open storage: %s", argv[1]);

  if ((backup = tmpfile()) == NULL)
    err(EXIT_FAILURE, "tmpfile()");

  for (int i = 2; i < argc; ++i)
    hash_dispatch(storage, backup, argv[i]);

  
  backup_hash = digest_file(backup);
  upload_path = path_down("backups", backup_hash);
  if (!storage_store_file(storage, upload_path, backup))
    errx(EXIT_FAILURE, "unable to upload the backup");
  free(upload_path);

  fclose(backup);
  storage_delete(storage);

  return EXIT_SUCCESS;
}
