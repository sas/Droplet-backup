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

extern char *__progname; // From crt0.o

static void help(FILE *output)
{
  static const char *message[] =
  {
    "",
    "    Commmands:",
    "        backup:    Make a backup.",
    "        restore:   Restore a backup.",
    "        list:      List all available backups in a backup directory.",
    "        help:      Display detailed help about each command.",
    "",
  };

  fprintf(output, "\nusage: %s <command> [ options ] <args>\n", __progname);
  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    fprintf(output, "%s\n", message[i]);
}

static void help_backup(void)
{
  static const char *message[] =
  {
    "",
    "Make a backup.",
    "",
    "    Options:",
    "        --profile, -p:",
    "            Profile to use with the `dpl` type storage.",
    "        --profile-dir, -d:",
    "            Directory in which the `dpl` profile should be searched.",
    "        --verbose, -v:",
    "            Verbose mode. A line is displayed for each file backuped.",
    "        --name, -n:",
    "            Give a name to the backup, to use with the `restore` command.",
    "",
    "There are currently two schemes of URIs supported:",
    "    dpl://   to backup to an S3 compatible cloud storage",
    "    file://  to backup to a local dirctory",
    "",
  };

  printf("\nusage: %s backup [ options ] <storage> <elements...>\n", __progname);
  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    printf("%s\n", message[i]);
}

static void help_restore(void)
{
  static const char *message[] =
  {
    "",
    "Restore a backup.",
    "",
    "    Options:",
    "        --profile, -p:",
    "            Profile to use with the `dpl` type storage.",
    "        --profile-dir, -d:",
    "            Directory in which the `dpl` profile should be searched.",
    "        --verbose, -v:",
    "            Verbose mode. A line is displayed for each file restored.",
    "        --interactive, -i:",
    "            Interractively ask for a backup name, with a list of available",
    "            backups if no backup name is specified on the command line.",
    "",
  };

  printf("\nusage: %s restore [ options ] <storage> <backups>\n", __progname);
  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    printf("%s\n", message[i]);
}

static void help_list(void)
{
  static const char *message[] =
  {
    "",
    "List available backups.",
    "",
  };

  printf("\nusage: %s list [ options ] <storage>\n", __progname);
  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    printf("%s\n", message[i]);
}

static void help_help(void)
{
  static const char *message[] =
  {
    "",
    "lolwut?",
    "",
  };

  printf("\nusage: %s help <topic>\n", __progname);
  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    printf("%s\n", message[i]);
}

int cmd_help(int argc, char *argv[])
{
  if (argc == 0)
  {
    help(stderr);
    return EXIT_FAILURE;
  }

  if (argc == 1)
  {
    help(stdout);
    return EXIT_SUCCESS;
  }

  if (strcmp(argv[1], "backup") == 0)
    help_backup();
  else if (strcmp(argv[1], "restore") == 0)
    help_restore();
  else if (strcmp(argv[1], "list") == 0)
    help_list();
  else if (strcmp(argv[1], "help") == 0)
    help_help();
  else
    return cmd_help(0, NULL);

  return EXIT_SUCCESS;
}
