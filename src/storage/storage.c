#define STORAGE_INTERNAL

#include <string.h>

#include <storage/dpl/dpl.h>
#include <storage/file/file.h>
#include <usage.h>

#include "storage.h"

storage_t storage_new(const char *uri)
{
  static const struct
  {
    const char *scheme;
    storage_t (*initializer)(const char *uri);
  } inits[] = {
    { "dpl://",   sto_dpl_new },
    { "file://",   sto_file_new },
  };

  for (unsigned int i = 0; i < sizeof (inits) / sizeof (inits[0]); ++i)
    if (strncmp(uri, inits[i].scheme, strlen(inits[i].scheme)) == 0)
      return inits[i].initializer(uri + strlen(inits[i].scheme));

  err(EXIT_UNK_STORAGE, "unknown uri scheme: %s\n", uri);
}

int storage_store(storage_t storage, const char *path, struct buffer *data)
{
  return storage->store(storage->state, path, data);
}

struct buffer *storage_retrieve(storage_t storage, const char *path)
{
  return storage->retrieve(storage->state, path);
}

const char *storage_list(storage_t storage, const char *path)
{
  return storage->list(storage->state, path);
}
