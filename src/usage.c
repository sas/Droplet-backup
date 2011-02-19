#include <stdio.h>

#include "usage.h"

void usage(FILE *output)
{
  char *message[] = {
    "usage: dplbck [ options ] <command> [ command_options ] [ command_args ]",
    "    options",
    "        --profile, -p:     specify the droplet profile to use",
    "        --verbose, -v:     verbose mode",
    "        --interactive, -i: interactive mode",
    "    commmand",
    "        backup:  make a backup of a folder",
    "        restore: restore a backup folder",
    "        list:    list all available backups",
    "        stats:   display stats about a backup folder",
    "        help:    display this help",
  };

  for (unsigned int i = 0; i < sizeof (message) / sizeof (message[0]); ++i)
    fprintf(output, "%s\n", message[i]);
}
