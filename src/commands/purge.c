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
#include <stdlib.h>

#include <commands/help.h>
#include <storage/storage.h>
#include <utils/strset.h>

#include "purge.h"

int cmd_purge(int argc, char *argv[])
{
  storage_t storage;
  strset_t ss;
  const char *elem;

  if (argc != 2)
  {
    int help_argc = 2;
    char *help_argv[] = { "help_err", "purge", NULL };
    return cmd_help(help_argc, help_argv);
  }

  if ((storage = storage_new(argv[1], 0)) == NULL)
    errx(EXIT_FAILURE, "unable to open storage: %s", argv[1]);

  ss = strset_new();

  elem = storage_list(storage, "objects");
  while (elem != NULL)
  {
    strset_add(ss, elem);
    elem = storage_list(storage, NULL);
  }

  strset_delete(ss);
  storage_delete(storage);

  return EXIT_SUCCESS;
}