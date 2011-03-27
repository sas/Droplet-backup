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

#ifndef STORAGE_H_
# define STORAGE_H_

struct buffer
{
  unsigned int size;
  unsigned char data[];
};

# ifdef STORAGE_INTERNAL
/*
** If we do not define STORAGE_INTERNAL, i.e.: if we are not an internal storage
** module, or the abstract storage module itself, we do not have access to
** internal types.
*/
struct storage
{
  int (*store)(void *state, const char *path, struct buffer *data);
  struct buffer *(*retrieve)(void *state, const char *path);
  const char *(*list)(void *state, const char *path);
  void (*delete)(void *state);
  void *state;
};
# endif /* STORAGE_INTERNAL */

typedef struct storage *storage_t;

storage_t storage_new(const char *uri, int create_dirs);
int storage_store(storage_t storage, const char *path, struct buffer *data);
struct buffer *storage_retrieve(storage_t storage, const char *path);
const char *storage_list(storage_t storage, const char *path);
void storage_delete(storage_t storage);

#endif /* !STORAGE_H_ */
