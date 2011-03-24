#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "usage.h"

void usage(FILE *output)
{
  char *message[] = {
    "usage: dplbck [ options ] <command> [ command_args ]",
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
    "    dplbck -v backup /etc dpl://my_backups/backup_folder/",
    "        makes a backup of /etc in the bucket `my_backups', in the folder",
    "        `backup_folder' and enables verbose mode, so all transferts are",
    "        logged on stderr.",
    "    dplbck -p bck restore dpl://my_save/websrv/htdocs/ /srv/www/htdocs/",
    "        uses the profile `bck' to restore the latest backup present in",
    "        the folder `/websrv/htdocs' in the bucket `my_save' in the",
    "        folder `/srv/www/htdocs/'.",
  };

  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    fprintf(output, "%s\n", message[i]);
}

void usage_die(void)
{
  usage(stderr);
  exit(EXIT_BAD_USAGE);
}

void err(int exit_val, const char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
  exit(exit_val);
}

void warn(const char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
}
