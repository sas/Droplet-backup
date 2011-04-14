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

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include <storage/storage.h>
#include <utils/messages.h>
#include <utils/path.h>

#include "restore.h"

int cmd_restore(int argc, char *argv[])
{
  storage_t storage;
  FILE *backup;
  char *download_path;
  char buf[4096];
  int size;

  /* XXX: No option parsing for the moment. */
  if (argc != 3)
    usage_die();

  if ((storage = storage_new(argv[1], 0)) == NULL)
    errx(EXIT_FAILURE, "unable to open storage: %s", argv[1]);

  download_path = path_concat("backups", argv[2]);
  backup = storage_retrieve_file(storage, download_path);
  free(download_path);

  while ((size = fread(buf, 1, 4096, backup)) > 0)
    fwrite(buf, 1, size, stdout);

  fclose(backup);
  storage_delete(storage);

  return EXIT_SUCCESS;
}
