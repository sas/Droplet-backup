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
#define OPT_LOGFILE       'l'

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
    .has_arg = optional_argument,
    .flag = NULL,
    .val = OPT_VERBOSE,
  },
  {
    .name = "log-file",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_LOGFILE,
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
        if (optarg == NULL)
        {
          options.verbose = LOG_VERBOSE;
        }
        else
        {
          if (strcmp(optarg, "debug") == 0)
            options.verbose = LOG_DEBUG;
          else if (strcmp(optarg, "verbose") == 0)
            options.verbose = LOG_VERBOSE;
          else if (strcmp(optarg, "info") == 0)
            options.verbose = LOG_INFO;
          else if (strcmp(optarg, "warning") == 0)
            options.verbose = LOG_WARNING;
          else if (strcmp(optarg, "error") == 0)
            options.verbose = LOG_ERROR;
          else
            logger(LOG_ERROR, "%s: invalid argument", optarg);
        }
        break;
      case OPT_LOGFILE:
        options.log_file = optarg;
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
