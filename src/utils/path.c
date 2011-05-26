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

#include "path.h"

/*
** This function takes a path, and a directory to go into and returns the
** resulting path.
** The caller must free the result.
*/
char *path_concat(const char *path, const char *elem)
{
  unsigned int path_len = strlen(path);
  unsigned int elem_len = strlen(elem);
  char *res;

  res = emalloc(path_len + elem_len + 2);

  strcpy(res, path);
  /* Remove superfluous '/'. */
  while (path_len > 0 && res[path_len - 1] == '/')
    --path_len;
  if (path_len > 0)
    res[path_len++] = '/';
  strcpy(res + path_len, elem);

  return res;
}

/*
** This function removes trailing slashes from a path. Of course, if the path is
** '/', it does not remove anything.
** This function returns the address of the modified string (which is `path').
*/
char *path_rm_trailing_slashes(char *path)
{
  int idx;

  idx = strlen(path) - 1;

  while (idx > 0 && path[idx] == '/')
    path[idx--] = '\0';

  return path;
}

/*
** This functions compares two paths for equality. The actual processing uses
** the realpath() function to get a connonical path out of the two paths passed
** as arguments.
*/
bool path_equal(const char *path1, const char *path2)
{
  bool res;
  char *realpath1, *realpath2;

  realpath1 = realpath(path1, NULL);
  realpath2 = realpath(path2, NULL);

  if (realpath1 == NULL || realpath2 == NULL)
    return false;

  res = (strcmp(realpath1, realpath2) == 0);

  free(realpath1);
  free(realpath2);

  return res;
}
