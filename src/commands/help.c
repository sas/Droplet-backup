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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.h"

extern char *__progname; /* From crt0.o. */

static void main_help(FILE *output)
{
  static const char *message[] =
  {
    "",
    "    Commmands:",
    "        backup:    Make a backup.",
    "        restore:   Restore a backup.",
    "        delete:    Delete a backup.",
    "        purge:     Purge a backup directory from unused blocks.",
    "        list:      List all available backups in a backup directory.",
    "        help:      Display detailed help about each command.",
    "",
  };

  fprintf(output, "usage: %s <command> [ options ] <args>\n", __progname);
  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    fprintf(output, "%s\n", message[i]);
}


int cmd_help(int argc, char *argv[])
{
  if (argc == 1)
  {
    main_help(stdout);
    return EXIT_SUCCESS;
  }

  char buf[42]; /* This should be enough! */
  snprintf(buf, sizeof (buf), "man %s-%s", __progname, argv[1]);
  system(buf);

  return EXIT_SUCCESS;
}

int cmd_help_err(int argc, char *argv[])
{
  if (argc == 1)
  {
    main_help(stderr);
    return EXIT_FAILURE;
  }

  static const struct
  {
    const char *command;
    const char *usage;
  } ut[] =
  {
    { "backup",   "[ options ] <storage> <elements...>" },
    { "restore",  "[ options ] <storage> <backup>" },
    { "purge",    "[ options ] <storage>" },
    { "list",     "[ options ] <storage>" },
    { "delete",   "[ options ] <storage> <backup>" },
  };

  for (unsigned int i = 0; i < sizeof (ut) / sizeof (ut[0]); ++i)
    if (strcmp(argv[1], ut[i].command) == 0)
      fprintf(stderr, "usage: %s %s %s\n", __progname, ut[i].command, ut[i].usage);

  return EXIT_FAILURE;
}
