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

#ifndef DIEFUNCS_H_
# define DIEFUNCS_H_

# include <err.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

/*
** These functions are used to remove a small amount of error handling from the
** core parts of the code. They are meant to be used where an error means the
** termination of the program (e.g.: when malloc() fails, there is nothing more
** to do than to die).
*/

static inline void *emalloc(size_t size)
{
  void *res = malloc(size);

  if (res == NULL)
    err(EXIT_FAILURE, "malloc()");

  return res;
}

static inline char *estrdup(const char *s)
{
  char *res = strdup(s);

  if (res == NULL)
    err(EXIT_FAILURE, "strdup()");

  return res;
}

static inline FILE *etmpfile(void)
{
  FILE *res = tmpfile();

  if (res == NULL)
    err(EXIT_FAILURE, "tmpfile()");

  return res;
}

#endif /* !DIEFUNCS_H_ */
