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

#include <utils/diefuncs.h>

#include "list.h"

struct list_node
{
  void              *elem;
  struct list_node  *next;
  struct list_node  *prev;
};

struct list
{
  struct list_node  *head;
  unsigned int       size;
};

struct list *list_new(void)
{
  struct list *res;

  res = emalloc(sizeof (struct list));

  res->head = NULL;
  res->size = 0;

  return res;
}

void list_delete(struct list *l)
{
  struct list_node *next;

  for (unsigned int i = 0; i < l->size; ++i)
  {
    next = l->head->next;
    free(l->head);
    l->head = next;
  }

  free(l);
}

unsigned int list_size(struct list *l)
{
  return l->size;
}

static unsigned int list_push(struct list *l, void *elem, bool front)
{
  struct list_node *new;

  new = emalloc(sizeof (struct list_node));

  new->elem = elem;

  if (l->size == 0)
  {
    new->next = new->prev = new;
    l->head = new;
  }
  else
  {
    new->next = l->head;
    new->prev = l->head->prev;
    l->head->prev->next = new;
    l->head->prev = new;
    if (front)
      l->head = new;
  }

  l->size += 1;

  return l->size;
}

unsigned int list_push_front(struct list *l, void *elem)
{
  return list_push(l, elem, true);
}

unsigned int list_push_back(struct list *l, void *elem)
{
  return list_push(l, elem, false);
}

static void *list_pop(struct list *l, bool front)
{
  void *res;
  struct list_node *old;

  if (l->size == 0)
    return NULL;

  if (front)
    old = l->head;
  else
    old = l->head->prev;

  res = old->elem;

  old->next->prev = old->prev;
  old->prev->next = old->next;

  if (front)
    l->head = old->next;

  l->size -= 1;
  free(old);

  return res;
}

void *list_pop_front(struct list *l)
{
  return list_pop(l, true);
}

void *list_pop_back(struct list *l)
{
  return list_pop(l, false);
}

void list_foreach(struct list *l, void (*fun)(void *, void *), void *data)
{
  struct list_node *current = l->head;

  for (unsigned int i = 0; i < l->size; ++i)
  {
    fun(current->elem, data);
    current = current->next;
  }
}
