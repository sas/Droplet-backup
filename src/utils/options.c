#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"

static const struct option possible_options[] =
{
  {
    .name = "profile",
    .has_arg = required_argument,
    .flag = NULL,
    .val = 'p',
  },
  {
    .name = "profile-dir",
    .has_arg = required_argument,
    .flag = NULL,
    .val = 'd',
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

  while ((flag = getopt_long(argc, argv, "p:d:vi", possible_options, NULL)) != -1)
  {
    switch (flag)
    {
      case 'p':
      case 'd':
        options[flag] = optarg;
        break;
      case 'v':
      case 'i':
        /* This value is used as a simple boolean. */
        options[flag] = (void *) 0x42;
        break;
      case '?':
        exit(EXIT_FAILURE);
    };
  }

  return optind;
}
