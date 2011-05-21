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
** Purge a backup directory from unused blocks.
** Every element of the hash tree is marked as being used with the corresponding
** purge_ function.
*/

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include <commands/help.h>
#include <storage/storage.h>
#include <utils/options.h>
#include <utils/path.h>
#include <utils/strset.h>

#include "purge.h"

static void purge_dispatch(storage_t storage, strset_t objects, char *elem_str);

static void purge_file(storage_t storage, strset_t objects, char *hash)
{
  char *download_path;
  FILE *descr;
  char buf[4096];
  char *blob_hash;

  download_path = path_concat("objects", hash);
  if ((descr = storage_retrieve_file(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve: %s", hash);
  free(download_path);

  while ((blob_hash = fgets(buf, 4096, descr)) != NULL)
  {
    size_t size;

    size = strlen(blob_hash);
    if (blob_hash[size - 1] != '\n')
      errx(EXIT_FAILURE, "invalid object: %s", hash);
    blob_hash[size - 1] = '\0';

    strset_del(objects, blob_hash);
  }

  if (ferror(descr))
    errx(EXIT_FAILURE, "unable to purge: %s", hash);

  fclose(descr);
}

static void purge_tree(storage_t storage, strset_t objects, char *hash)
{
  char *download_path;
  FILE *descr;
  char buf[4096];

  download_path = path_concat("objects", hash);
  if ((descr = storage_retrieve_file(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve: %s", hash);
  free(download_path);

  while (fgets(buf, 4096, descr) != NULL)
    purge_dispatch(storage, objects, buf);

  if (ferror(descr))
    errx(EXIT_FAILURE, "unable to purge: %s", hash);

  fclose(descr);
}

static void purge_dispatch(storage_t storage, strset_t objects, char *elem_str)
{
  char *type;
  char *hash;

  if ((type = strsep(&elem_str, " ")) == NULL)
    errx(EXIT_FAILURE, "invalid description file: %s", elem_str);

  if ((hash = strsep(&elem_str, " ")) == NULL)
    errx(EXIT_FAILURE, "invalid description file: %s", elem_str);

  strset_del(objects, hash);

  if (strcmp(type, "file") == 0)
    purge_file(storage, objects, hash);
  else if (strcmp(type, "tree") == 0)
    purge_tree(storage, objects, hash);
  else if (strcmp(type, "link") == 0)
    return; /* No need to recurse. */
  else
    errx(EXIT_FAILURE, "invalid description file: %s", elem_str);
}

struct mark_backup_args
{
  storage_t storage;
  strset_t objects;
};

static void mark_backup(const char *str, void *data)
{
  struct mark_backup_args *args = data;
  char *download_path;
  FILE *backup;
  char buf[4096];

  download_path = path_concat("backups", str);
  if ((backup = storage_retrieve_file(args->storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "%s: unable to retrieve the description file", str);
  free(download_path);

  while (fgets(buf, 4096, backup) != NULL)
    purge_dispatch(args->storage, args->objects, buf);

  if (ferror(backup))
    errx(EXIT_FAILURE, "unable to restore the backup");

  fclose(backup);
}

static void delete_object(const char *str, void *data)
{
  storage_t storage = (storage_t) data;
  char *unlink_path;

  logger(LOG_VERBOSE, "%s\n", str);

  unlink_path = path_concat("objects", str);
  if (!storage_unlink(storage, unlink_path))
    errx(EXIT_FAILURE, "%s: unable to delete object", str);
  free(unlink_path);
}

int cmd_purge(int argc, char *argv[])
{
  storage_t storage;
  strset_t backups;
  strset_t objects;
  const char *elem;
  struct mark_backup_args args;

  if (argc != 2)
  {
    int help_argc = 2;
    char *help_argv[] = { "help_err", "purge", NULL };
    return cmd_help(help_argc, help_argv);
  }

  if ((storage = storage_new(argv[1], 0)) == NULL)
    errx(EXIT_FAILURE, "unable to open storage: %s", argv[1]);

  backups = strset_new();
  objects = strset_new();

  /* Get the list of backups. */
  elem = storage_list(storage, "backups");
  while (elem != NULL)
  {
    strset_add(backups, elem);
    elem = storage_list(storage, NULL);
  }

  /* Get the list of objects. */
  elem = storage_list(storage, "objects");
  while (elem != NULL)
  {
    strset_add(objects, elem);
    elem = storage_list(storage, NULL);
  }

  /*
  ** For each backup, call mark_backup, which will mark all the objects of a
  ** backup as used (it will actually remove them from the `objects` strset.
  */
  args.objects = objects;
  args.storage = storage;
  strset_foreach(backups, mark_backup, &args);

  /* Delete every object remaining in the `objects` strset. */
  strset_foreach(objects, delete_object, (void *) storage);

  strset_delete(backups);
  strset_delete(objects);
  storage_delete(storage);

  return EXIT_SUCCESS;
}
