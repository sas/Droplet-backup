#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"

static const struct option possible_options[] =
{
  /* General options. */
  {
    .name = "change-dir",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_CHANGEDIR,
  },
  {
    .name = "interactive",
    .has_arg = no_argument,
    .flag = NULL,
    .val = OPT_INTERACTIVE,
  },
  {
    .name = "name",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_NAME,
  },

  /* Logging options. */
  {
    .name = "verbose",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_VERBOSE,
  },

  /* Droplet related options. */
  {
    .name = "profile",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_PROFILE,
  },
  {
    .name = "profile-dir",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_PROFILEDIR,
  },

  { NULL, 0, NULL, 0, },
};

const char *options[128] = { 0 };

int options_init(int argc, char *argv[])
{
  int flag;

  while ((flag = getopt_long(argc, argv, "c:in:vp:d:", possible_options, NULL)) != -1)
  {
    switch (flag)
    {
      case OPT_CHANGEDIR:
        options[flag] = optarg;
        break;
      case OPT_INTERACTIVE:
        options[flag] = (void *) 0x42; /* Boolean. */
        break;
      case OPT_NAME:
        options[flag] = optarg;
        break;

      case OPT_VERBOSE:
        options[flag] = (void *) 0x42; /* Boolean. */
        break;

      case OPT_PROFILE:
        options[flag] = optarg;
        break;
      case OPT_PROFILEDIR:
        options[flag] = optarg;
        break;

      case '?':
        exit(EXIT_FAILURE);
    };
  }

  return optind;
}
