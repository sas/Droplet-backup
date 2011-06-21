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

#include <editline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commands/help.h>
#include <commands/list.h>
#include <storage/storage.h>
#include <utils/diefuncs.h>
#include <utils/logger.h>
#include <utils/options.h>
#include <utils/path.h>

#include "delete.h"

int cmd_delete(int argc, char *argv[])
{
  storage_t storage;
  char *backup_name;
  char *unlink_path;

  if (!(argc == 3 || (argc == 2 && options_get()->interactive)))
  {
    int help_argc = 2;
    char *help_argv[] = { "help_err", "delete", NULL };
    return cmd_help(help_argc, help_argv);
  }

  if ((storage = storage_new(argv[1], 0)) == NULL)
    logger(LOG_ERROR, "unable to open storage: %s", argv[1]);

  if (argc == 3)
  {
    backup_name = estrdup(argv[2]);
  }
  else
  {
    printf("Available backups:\n");
    if (cmd_list(argc, argv) == EXIT_FAILURE)
      return EXIT_FAILURE;
    backup_name = readline("Enter the backup name to delete: ");
    /* Cleanly exit if the user did not enter any backup to delete. */
    if (backup_name == NULL || strlen(backup_name) == 0)
      return EXIT_SUCCESS;
  }

  unlink_path = path_concat("backups", backup_name);
  free(backup_name);
  if (!storage_unlink(storage, unlink_path))
    logger(LOG_ERROR, "unable to delete the backup");
  free(unlink_path);

  storage_delete(storage);

  return EXIT_SUCCESS;
}
