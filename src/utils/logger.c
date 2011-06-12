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

#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

extern char *__progname; /* From crt0.o. */

static enum log_level log_level = LOG_INFO;

void logger_init(enum log_level level, const char *output)
{
  if (level != 0)
    log_level = level;

  if (output != NULL)
    if (freopen(output, "w", stderr) == NULL)
      elogger(LOG_ERROR, "%s", output);
}

void logger(enum log_level level, const char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  vlogger(level, format, ap);
  va_end(ap);
}

void elogger(enum log_level level, const char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  velogger(level, format, ap);
  va_end(ap);
}

void vlogger(enum log_level level, const char *format, va_list ap)
{
  if (level >= log_level)
  {
    fprintf(stderr, "%s: ", __progname);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
  }

  if (level == LOG_ERROR)
    exit(EXIT_FAILURE);
}

void velogger(enum log_level level, const char *format, va_list ap)
{
  if (level >= log_level)
  {
    fprintf(stderr, "%s: ", __progname);
    vfprintf(stderr, format, ap);
    fprintf(stderr, ": %s\n", strerror(errno));
  }

  if (level == LOG_ERROR)
    exit(EXIT_FAILURE);
}
