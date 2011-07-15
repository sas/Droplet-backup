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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <utils/diefuncs.h>

#include "strset.h"

#define STRSET_INITIAL_SIZE 5

struct strset
{
  char **strs;
  unsigned long int size;
  unsigned long int used;
};

strset_t strset_new(void)
{
  strset_t res = emalloc(sizeof (struct strset));

  res->strs = emalloc(STRSET_INITIAL_SIZE * sizeof (res->strs[0]));
  res->size = STRSET_INITIAL_SIZE;
  res->used = 0;

  return res;
}

bool strset_contains(strset_t ss, const char *str)
{
  for (unsigned int i = 0; i < ss->used; ++i)
    if (strcmp(str, ss->strs[i]) == 0)
      return true;

  return false;
}

bool strset_add(strset_t ss, const char *str)
{
  for (unsigned int i = 0; i < ss->used; ++i)
    if (strcmp(str, ss->strs[i]) == 0)
      return false;

  if (ss->used == ss->size)
  {
    ss->strs = realloc(ss->strs, 2 * ss->size * sizeof (ss->strs[0]));
    ss->size *= 2;
  }

  ss->strs[ss->used++] = estrdup(str);

  return true;
}

bool strset_del(strset_t ss, const char *str)
{
  for (unsigned int i = 0; i < ss->used; ++i)
  {
    if (strcmp(str, ss->strs[i]) == 0)
    {
      free(ss->strs[i]);
      ss->strs[i] = ss->strs[ss->used - 1];
      --ss->used;

      return true;
    }
  }

  return false;
}

void strset_foreach(strset_t ss, void (*fun)(const char *, void *), void *data)
{
  for (unsigned int i = 0; i < ss->used; ++i)
    fun(ss->strs[i], data);
}

static int strset_sort_cb(const void *s1, const void *s2)
{
  const char *ss1 = *((char**) s1);
  const char *ss2 = *((char**) s2);
  return strcmp(ss1, ss2);
}

void strset_sort(strset_t ss)
{
  qsort(ss->strs, ss->used, sizeof (char *), strset_sort_cb);
}

void strset_print_cb(const char *s, void *data)
{
  (void) data;

  printf("%s\n", s);
}

void strset_print(strset_t ss)
{
  strset_foreach(ss, strset_print_cb, NULL);
}

void strset_delete(strset_t ss)
{
  for (unsigned int i = 0; i < ss->used; ++i)
    free(ss->strs[i]);
  free(ss->strs);
  free(ss);
}
