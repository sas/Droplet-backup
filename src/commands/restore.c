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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <storage/storage.h>
#include <utils/messages.h>
#include <utils/path.h>

#include "restore.h"

struct object
{
  char *type;
  char *hash;
  unsigned int uid;
  unsigned int gid;
  unsigned int perm;
  char *name;
};

static void unhash_file(storage_t storage, const char *hash, const char *path)
{
  (void) storage;

  printf("unhash_file(%s, %s);\n", hash, path);
}

static void unhash_tree(storage_t storage, const char *hash, const char *path)
{
  (void) storage;

  printf("unhash_tree(%s, %s);\n", hash, path);
}

static void unhash_dispatch(storage_t storage, char *elem, const char *path)
{
  struct object obj;
  char *tmp_elem;
  char *new_path;

  /* XXX: We should have a more explicit error message here. */
#define MALFORMED() do { warnx("malformed line"); return; } while (0)
  if ((obj.type = strsep(&elem, " ")) == NULL)
    MALFORMED();

  if ((obj.hash = strsep(&elem, " ")) == NULL)
    MALFORMED();

  tmp_elem = elem;
  obj.uid = strtol(elem, &elem, 10);
  if (tmp_elem == elem)
    MALFORMED();

  tmp_elem = elem;
  obj.gid = strtol(elem, &elem, 10);
  if (tmp_elem == elem)
    MALFORMED();

  tmp_elem = elem;
  obj.perm = strtol(elem, &elem, 8);
  if (tmp_elem == elem)
    MALFORMED();

  ++elem; // Skip the space before the name
  if ((obj.name = strsep(&elem, "\n")) == NULL)
    MALFORMED();
#undef MALFORMED

  new_path = path_concat(path, obj.name);

  if (strcmp(obj.type, "file") == 0)
    unhash_file(storage, obj.hash, new_path);
  else if (strcmp(obj.type, "tree") == 0)
    unhash_tree(storage, obj.hash, new_path);

  free(new_path);
}

int cmd_restore(int argc, char *argv[])
{
  storage_t storage;
  FILE *backup;
  char *download_path;
  char buf[4096];

  /* XXX: No option parsing for the moment. */
  if (argc != 3)
    usage_die();

  if ((storage = storage_new(argv[1], 0)) == NULL)
    errx(EXIT_FAILURE, "unable to open storage: %s", argv[1]);

  download_path = path_concat("backups", argv[2]);
  if ((backup = storage_retrieve_file(storage, download_path)) == NULL)
    errx(EXIT_FAILURE, "unable to retrieve the backup description file");
  free(download_path);

  while (fgets(buf, 4096, backup) != NULL)
    unhash_dispatch(storage, buf, ".");

  fclose(backup);
  storage_delete(storage);

  return EXIT_SUCCESS;
}
