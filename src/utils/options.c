#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"

static const struct option possible_options[] =
{
  {
    .name = "profile-dir",
    .has_arg = required_argument,
    .flag = NULL,
    .val = 'd',
  },
  {
    .name = "profile",
    .has_arg = required_argument,
    .flag = NULL,
    .val = 'p',
  },
  {
    .name = "verbose",
    .has_arg = no_argument,
    .flag = NULL,
    .val = 'v',
  },
  {
    .name = "interactive",
    .has_arg = no_argument,
    .flag = NULL,
    .val = 'i',
  },
  { NULL, 0, NULL, 0, },
};

const char *options[128] = { 0 };

int options_init(int argc, char *argv[])
{
  int flag;

  while ((flag = getopt_long(argc, argv, "d:p:vi", possible_options, NULL)) != -1)
  {
    switch (flag)
    {
      case '?':
        exit(EXIT_FAILURE);
      case 'd':
      case'p':
        options[flag] = optarg;
        break;
      case 'v':
      case 'i':
        /* We will use this value as a boolean. */
        options[flag] = (void *) 0x42;
        break;
    };
  }

  return optind;
}
