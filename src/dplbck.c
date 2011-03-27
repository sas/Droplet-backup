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
#include <string.h>

#include <commands/backup/backup.h>
#include <commands/help/help.h>
#include <commands/list/list.h>
#include <commands/restore/restore.h>
#include <commands/stats/stats.h>
#include <usage.h>

int main(int argc, char *argv[])
{
  struct {
    char *cmd_name;
    int (*cmd)(int, char *[]);
  } commands[] = {
    { "backup",   cmd_backup, },
    { "restore",  cmd_restore, },
    { "list",     cmd_list, },
    { "stats",    cmd_stats, },
    { "help",     cmd_help, },
  };

  --argc; ++argv;

  /* XXX: We skip the arguments for now. */
  while (argc && *argv[0] == '-')
    --argc, ++argv;

  if (argc == 0)
  {
    usage(stderr);
    return 1;
  }

  for (unsigned int i = 0; i < sizeof (commands) / sizeof (commands[0]); ++i)
    if (strcmp(*argv, commands[i].cmd_name) == 0)
      return commands[i].cmd(argc, argv);

  /* We never reach this point if there is a valid command. */
  err(EXIT_USAGE_FAIL, "unknown command: %s\n", *argv);
}
