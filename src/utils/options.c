#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "options.h"

/* General options. */
#define OPT_CHANGEDIR     'c'
#define OPT_INTERACTIVE   'i'
#define OPT_NAME          'n'

/* Logging options. */
#define OPT_VERBOSE       'v'

/* Droplet related options. */
#define OPT_PROFILEDIR    'd'
#define OPT_PROFILE       'p'

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
    .name = "profile-dir",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_PROFILEDIR,
  },
  {
    .name = "profile",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_PROFILE,
  },

  { NULL, 0, NULL, 0, },
};

static struct options options;

int options_init(int argc, char *argv[])
{
  int flag;

  memset(&options, 0, sizeof (struct options));

  while ((flag = getopt_long(argc, argv, "c:in:vd:p:", possible_options, NULL)) != -1)
  {
    switch (flag)
    {
      case OPT_CHANGEDIR:
        options.change_dir = optarg;
        break;
      case OPT_INTERACTIVE:
        options.interactive = true;
        break;
      case OPT_NAME:
        options.name = optarg;
        break;

      case OPT_VERBOSE:
        options.verbose = true;
        break;

      case OPT_PROFILEDIR:
        options.profile_dir = optarg;
        break;
      case OPT_PROFILE:
        options.profile = optarg;
        break;

      case '?':
        exit(EXIT_FAILURE);
    };
  }

  return optind;
}

struct options *options_get(void)
{
  return &options;
}
