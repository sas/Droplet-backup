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

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <utils/diefuncs.h>

#include "options.h"

/* General options. */
#define OPT_CHANGEDIR     'C'
#define OPT_INTERACTIVE   'i'
#define OPT_NAME          'n'
#define OPT_FORCE         'f'

/* Logging options. */
#define OPT_VERBOSE       'v'
#define OPT_LOGFILE       'o'

/* Hierarchy traversal options. */
#define OPT_XDEV          'x'
#define OPT_DRYRUN        'r'
#define OPT_EXCLUDE       'e'
#define OPT_EXCLUDEFILE   'E'

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
  {
    .name = "force",
    .has_arg = no_argument,
    .flag = NULL,
    .val = OPT_FORCE,
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

  /* Hierarchy traversal options. */
  {
    .name = "xdev",
    .has_arg = no_argument,
    .flag = NULL,
    .val = OPT_XDEV,
  },
  {
    .name = "dry-run",
    .has_arg = no_argument,
    .flag = NULL,
    .val = OPT_DRYRUN,
  },
  {
    .name = "exclude",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_EXCLUDE,
  },
  {
    .name = "exclude-file",
    .has_arg = required_argument,
    .flag = NULL,
    .val = OPT_EXCLUDEFILE,
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

static void add_exclude_file(const char *path)
{
  FILE *file;
  char line[4096]; /* On most systems, PATH_MAX is 4096. */

  if ((file = fopen(path, "r")) == NULL)
  {
    fprintf(stderr, "%s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while (fgets(line, sizeof (line), file) != NULL)
  {
    line[strlen(line) - 1] = 0;
    list_push_back(options.exclude_list, estrdup(line));
  }

  fclose(file);
}

int options_init(int argc, char *argv[])
{
  int flag;

  memset(&options, 0, sizeof (struct options));
  options.exclude_list = list_new();

  while ((flag = getopt_long(argc, argv, "C:in:fv::o:xre:E:d:p:", possible_options, NULL)) != -1)
  {
    switch (flag)
    {
      /* General options. */
      case OPT_CHANGEDIR:
        options.change_dir = optarg;
        break;
      case OPT_INTERACTIVE:
        options.interactive = true;
        break;
      case OPT_NAME:
        options.name = optarg;
        break;
      case OPT_FORCE:
        options.force = true;
        break;

      /* Logging options. */
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

      /* Hierarchy traversal options. */
      case OPT_XDEV:
        options.xdev = true;
        break;
      case OPT_DRYRUN:
        options.dry_run = true;
        break;
      case OPT_EXCLUDE:
        list_push_back(options.exclude_list, estrdup(optarg));
        break;
      case OPT_EXCLUDEFILE:
        add_exclude_file(optarg);
        break;

      /* Droplet related options. */
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

void free_list_elems_cb(void *arg, void *data)
{
  (void) data;
  free(arg);
}

void options_end(void)
{
  list_foreach(options.exclude_list, free_list_elems_cb, NULL);
  list_delete(options.exclude_list);
}
