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
** Restore a backup.
** Each element of the hash tree is restored with the corresponding unhash_
** function.
*/

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <readline/readline.h>

#include <commands/help.h>
#include <commands/list.h>
#include <storage/storage.h>
#include <utils/buffer.h>
#include <utils/diefuncs.h>
#include <utils/options.h>
#include <utils/path.h>

#include "restore.h"

struct element
{
  char *type;
  char *hash;
  unsigned int uid;
  unsigned int gid;
  unsigned int perm;
  char *name;
};

static void unhash_dispatch(storage_t storage, const char *path, char *elem);

static struct buffer *unhash_blob(storage_t storage, const char *path, const char *hash)
{
  char *download_path;
  struct buffer *res;

  download_path = path_concat("objects", hash);
  if ((res = storage_retrieve_buffer(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve file: %s", path);
  free(download_path);

  return res;
}

static void unhash_file(storage_t storage, const char *path, const struct element *elem)
{
  FILE *res;
  char *download_path;
  FILE *descr;
  char buf[4096];
  char *blob_hash;
  struct buffer *blob;

  if (options['v'])
    printf("%s\n", path);

  if ((res = fopen(path, "wb")) == NULL)
    err(EXIT_FAILURE, "unable to restore file: %s", path);

  download_path = path_concat("objects", elem->hash);
  if ((descr = storage_retrieve_file(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve: %s", path);
  free(download_path);

  while ((blob_hash = fgets(buf, 4096, descr)) != NULL)
  {
    unsigned int size, full_size;

    size = strlen(blob_hash);
    if (blob_hash[size - 1] != '\n')
      errx(EXIT_FAILURE, "invalid backup format in: %s", path);
    blob_hash[size - 1] = '\0';

    blob = unhash_blob(storage, path, blob_hash);
    full_size = 0;
    while (full_size < blob->used)
    {
      size = fwrite(blob->data + full_size, 1, blob->used - full_size, res);
      full_size += size;
    }
    buffer_delete(blob);
  }

  fclose(res);
  fclose(descr);
}

static void unhash_tree(storage_t storage, const char *path, const struct element *elem)
{
  char *download_path;
  FILE *descr;
  char buf[4096];

  if (options['v'])
    printf("%s\n", path);

  if (mkdir(path, 0700) == -1 && errno != EEXIST)
    err(EXIT_FAILURE, "unable to restore directory: %s", path);

  download_path = path_concat("objects", elem->hash);
  if ((descr = storage_retrieve_file(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve: %s", path);
  free(download_path);

  while (fgets(buf, 4096, descr) != NULL)
    unhash_dispatch(storage, path, buf);

  fclose(descr);

  if (chmod(path, elem->perm) == -1)
    warn("unable to set mode for %s", path);
  if (lchown(path, elem->uid, elem->gid) == -1)
    warn("unable to set uid/gid for %s", path);
}

static void unhash_link(storage_t storage, const char *path, const struct element *elem)
{
  char *download_path;
  struct buffer *descr;

  if (options['v'])
    printf("%s\n", path);

  download_path = path_concat("objects", elem->hash);
  if ((descr = storage_retrieve_buffer(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve: %s", path);
  free(download_path);

  if (symlink((char *) descr->data, path) == -1 && errno != EEXIST)
    err(EXIT_FAILURE, "unable to restore symlink: %s", path);

  buffer_delete(descr);
}

static void unhash_dispatch(storage_t storage, const char *path, char *elem_str)
{
#define MALFORMED() errx(EXIT_FAILURE, "invalid backup format in: %s", path)

  struct element elem;
  char *tmp_elem;
  char *new_path;

  if ((elem.type = strsep(&elem_str, " ")) == NULL)
    MALFORMED();

  if ((elem.hash = strsep(&elem_str, " ")) == NULL)
    MALFORMED();

  tmp_elem = elem_str;
  elem.uid = strtol(elem_str, &elem_str, 10);
  if (tmp_elem == elem_str)
    MALFORMED();

  tmp_elem = elem_str;
  elem.gid = strtol(elem_str, &elem_str, 10);
  if (tmp_elem == elem_str)
    MALFORMED();

  tmp_elem = elem_str;
  elem.perm = strtol(elem_str, &elem_str, 8);
  if (tmp_elem == elem_str)
    MALFORMED();

  ++elem_str; // Skip the space before the name
  if ((elem.name = strsep(&elem_str, "\n")) == NULL)
    MALFORMED();

  new_path = path_concat(path, elem.name);

  if (strcmp(elem.type, "file") == 0)
    unhash_file(storage, new_path, &elem);
  else if (strcmp(elem.type, "tree") == 0)
    unhash_tree(storage, new_path, &elem);
  else if (strcmp(elem.type, "link") == 0)
    unhash_link(storage, new_path, &elem);
  else
    MALFORMED();

  free(new_path);

#undef MALFORMED
}

int cmd_restore(int argc, char *argv[])
{
  storage_t storage;
  FILE *backup;
  char *backup_name;
  char *download_path;
  char buf[4096];

  if (!(argc == 3 || (argc == 2 && options['i'])))
    return cmd_help(0, NULL);

  if ((storage = storage_new(argv[1], 0)) == NULL)
    errx(EXIT_FAILURE, "unable to open storage: %s", argv[1]);

  if (argc == 3)
  {
    backup_name = estrdup(argv[2]);
  }
  else
  {
    printf("Available backups:\n");
    if (cmd_list(argc, argv) == EXIT_FAILURE)
      return EXIT_FAILURE;
    backup_name = readline("Enter the backup name: ");
  }

  download_path = path_concat("backups", backup_name);
  free(backup_name);
  if ((backup = storage_retrieve_file(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve the backup description file");
  free(download_path);

  while (fgets(buf, 4096, backup) != NULL)
    unhash_dispatch(storage, "", buf);

  fclose(backup);
  storage_delete(storage);

  return EXIT_SUCCESS;
}
