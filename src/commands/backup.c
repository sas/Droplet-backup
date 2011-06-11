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

/*
** Make a backup.
** Each element of the hash tree is backuped with the corresponding hash_
** function.
*/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <commands/help.h>
#include <storage/storage.h>
#include <utils/buffer.h>
#include <utils/diefuncs.h>
#include <utils/digest.h>
#include <utils/logger.h>
#include <utils/options.h>
#include <utils/path.h>
#include <utils/rollsum.h>

#include "backup.h"

struct hash_state
{
  storage_t storage;
  char *path;
  dev_t devid;
};

static void hash_dispatch(struct hash_state *hs, FILE *backup);

static const char *hash_blob(struct hash_state *hs, struct buffer *buf)
{
  const char *res;
  char *upload_path;

  res = digest_buffer(buf);
  upload_path = path_concat("objects", res);
  if (!storage_store_buffer(hs->storage, upload_path, buf))
    logger(LOG_ERROR, "unable to store: %s", hs->path);
  free(upload_path);

  return res;
}

static const char *hash_file(struct hash_state *hs, FILE *file)
{
  const char *res;
  FILE *tmp;
  struct buffer *buf;
  struct rollsum rs;
  char file_buf[4096];
  int file_buf_cnt;
  int file_buf_idx;
  char *upload_path;

  logger(LOG_VERBOSE, "%s\n", hs->path);

  tmp = etmpfile();
  buf = buffer_new(ROLLSUM_MAXSIZE);
  rollsum_init(&rs);

  while ((file_buf_cnt = fread(file_buf, 1, 4096, file)) > 0)
  {
    file_buf_idx = 0;

    while (file_buf_idx < file_buf_cnt)
    {
      buf->data[buf->used++] = file_buf[file_buf_idx];

      if (rollsum_roll(&rs, file_buf[file_buf_idx]))
      {
        res = hash_blob(hs, buf);
        fprintf(tmp, "%s\n", res);
        buf->used = 0;
        rollsum_init(&rs);
      }

      ++file_buf_idx;
    }
  }

  if (ferror(file))
    logger(LOG_ERROR, "unable to backup: %s", hs->path);

  /* Upload the last block of data. */
  res = hash_blob(hs, buf);
  fprintf(tmp, "%s\n", res);

  if ((res = digest_file(tmp)) == NULL)
    logger(LOG_ERROR, "unable to backup: %s", hs->path);
  upload_path = path_concat("objects", res);
  if (!storage_store_file(hs->storage, upload_path, tmp))
    logger(LOG_ERROR, "unable to store: %s", hs->path);
  free(upload_path);

  buffer_delete(buf);
  fclose(tmp);

  return res;
}

static const char *hash_tree(struct hash_state *hs, DIR *dir)
{
  const char *res;
  FILE *tmp;
  struct dirent *ent;
  char *upload_path;

  logger(LOG_VERBOSE, "%s\n", hs->path);

  tmp = etmpfile();

  while ((ent = readdir(dir)) != NULL)
  {
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
      continue;

    char *new_path = path_concat(hs->path, ent->d_name);
    char *old_path = hs->path;
    hs->path = new_path;

    hash_dispatch(hs, tmp);

    free(new_path);
    hs->path = old_path;
  }

  if ((res = digest_file(tmp)) == NULL)
    logger(LOG_ERROR, "unable to backup: %s", hs->path);
  upload_path = path_concat("objects", res);
  if (!storage_store_file(hs->storage, upload_path, tmp))
    logger(LOG_ERROR, "unable to store: %s", hs->path);
  free(upload_path);

  fclose(tmp);

  return res;
}

static const char *hash_link(struct hash_state *hs)
{
  const char *res;
  struct buffer *tmp;
  int size;
  char *upload_path;

  logger(LOG_VERBOSE, "%s\n", hs->path);

  /*
  ** We should use PATH_MAX instead of 4096 here, but this macro is not defined
  ** on all systems so a static size does the job.
  */
  tmp = buffer_new(4096);
  if ((size = readlink(hs->path, (char *) tmp->data, tmp->size - 1)) == -1)
  {
    elogger(LOG_WARNING, "%s", hs->path);
    buffer_delete(tmp);
    return NULL;
  }

  tmp->data[size++] = '\0';
  tmp->used = size;

  res = digest_buffer(tmp);
  upload_path = path_concat("objects", res);
  if (!storage_store_buffer(hs->storage, upload_path, tmp))
    logger(LOG_ERROR, "unable to store: %s", hs->path);
  free(upload_path);

  buffer_delete(tmp);

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

static const char *mode_to_str(mode_t mode)
{
  if (S_ISREG(mode))
    return "file";
  if (S_ISDIR(mode))
    return "tree";
  if (S_ISLNK(mode))
    return "link";

  return NULL;
}

static void hash_dispatch(struct hash_state *hs, FILE *backup)
{
  struct stat buf;
  const char *hash = NULL;

  if (lstat(hs->path, &buf) == -1)
  {
    elogger(LOG_WARNING, "%s", hs->path);
    return;
  }

  /* Skip the current path if we are crossing a device and --xdev is enabled. */
  if (options_get()->xdev)
  {
    if (hs->devid == (dev_t) -1)
      hs->devid = buf.st_dev;
    if (hs->devid != buf.st_dev)
      return;
  }

  if (S_ISREG(buf.st_mode))
  {
    FILE *file;

    if ((file = fopen(hs->path, "rb")) == NULL)
    {
      elogger(LOG_WARNING, "%s", hs->path);
      return;
    }

    hash = hash_file(hs, file);

    fclose(file);
  }
  else if (S_ISDIR(buf.st_mode))
  {
    DIR *dir;

    if ((dir = opendir(hs->path)) == NULL)
    {
      elogger(LOG_WARNING, "%s", hs->path);
      return;
    }

    hash = hash_tree(hs, dir);

    closedir(dir);
  }
  else if (S_ISLNK(buf.st_mode))
  {
    hash = hash_link(hs);
  }
  else if (S_ISCHR(buf.st_mode))
  {
    logger(LOG_WARNING, "%s: is a character device, not backuping", hs->path);
    return;
  }
  else if (S_ISBLK(buf.st_mode))
  {
    logger(LOG_WARNING, "%s: is a block device, not backuping", hs->path);
    return;
  }
  else if (S_ISFIFO(buf.st_mode))
  {
    logger(LOG_WARNING, "%s: is a named pipe, not backuping", hs->path);
    return;
  }
  else if (S_ISSOCK(buf.st_mode))
  {
    logger(LOG_WARNING, "%s: is a socket, not backuping", hs->path);
    return;
  }
  else
  {
    logger(LOG_WARNING, "%s: unknown file type, not backuping", hs->path);
    return;
  }

  if (hash != NULL)
    fprintf(backup, "%s %s %u %u %04o %s\n",
        mode_to_str(buf.st_mode),
        hash,
        buf.st_uid,
        buf.st_gid,
        buf.st_mode & 07777,
        const_basename(hs->path)
        );
}

int cmd_backup(int argc, char *argv[])
{
  storage_t storage;
  FILE *backup;
  char *upload_path;
  const char *backup_name;
  char backup_date[20]; /* Exactly the required size for "%Y.%m.%d-%H.%M.%S". */

  if (argc < 3)
  {
    int help_argc = 2;
    char *help_argv[] = { "help_err", "backup", NULL };
    return cmd_help(help_argc, help_argv);
  }

  if ((storage = storage_new(argv[1], 1)) == NULL)
    logger(LOG_ERROR, "unable to open storage: %s", argv[1]);

  backup = etmpfile();

  for (int i = 2; i < argc; ++i)
  {
    struct hash_state hs;

    hs.storage = storage;
    hs.path = path_rm_trailing_slashes(argv[i]);
    hs.devid = (dev_t) -1;

    hash_dispatch(&hs, backup);
  }

  backup_name = options_get()->name;

  if (backup_name == NULL)
  {
    time_t cur_time;
    struct tm *t;

    time(&cur_time);
    t = gmtime(&cur_time);
    if (strftime(backup_date, sizeof (backup_date), "%Y.%m.%d-%H.%M.%S", t) == 0)
      logger(LOG_ERROR, "strftime()");

    backup_name = backup_date;
  }

  upload_path = path_concat("backups", backup_name);
  if (!storage_store_file(storage, upload_path, backup))
    logger(LOG_ERROR, "unable to store the backup description file");
  free(upload_path);

  fclose(backup);
  storage_delete(storage);

  return EXIT_SUCCESS;
}
