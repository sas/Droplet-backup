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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "messages.h"

extern char *__progname; /* From crt0.o */

void usage(FILE *output)
{
  char *message[] = {
    "    options",
    "        --profile-dir, -d: specify the droplet profile drectory to use",
    "        --profile, -p:     specify the droplet profile to use",
    "        --verbose, -v:     verbose mode",
    "        --interactive, -i: interactive mode",
    "    commmand",
    "        backup:  make a backup of a folder",
    "        restore: restore a backup folder",
    "        list:    list all available backups",
    "        stats:   display stats about a backup folder",
    "        help:    display this help",
    "i.e.:",
    "    dplbck -v backup dpl://my_backups/backup_folder/ /etc",
    "        makes a backup of /etc in the bucket `my_backups', in the folder",
    "        `backup_folder' and enables verbose mode, so all transferts are",
    "        logged on stderr.",
    "    dplbck -p bck restore dpl://my_save/websrv/htdocs/ /srv/www/htdocs/",
    "        uses the profile `bck' to restore the latest backup present in",
    "        the folder `/websrv/htdocs' in the bucket `my_save' in the",
    "        folder `/srv/www/htdocs/'.",
  };

  fprintf(output, "usage: %s [ options ] <command> [ command_args ]\n", __progname);
  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    fprintf(output, "%s\n", message[i]);
}

void usage_die(void)
{
  usage(stderr);
  exit(EXIT_FAILURE);
}
