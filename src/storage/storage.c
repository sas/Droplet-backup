#include <string.h>

#include <storage/dpl/dpl.h>

#include "storage.h"

struct storage *storage_new(const char *uri)
{
  static const struct
  {
    const char *scheme;
    struct storage *(*initializer)(const char *uri);
  } inits[] = {
    { "dpl://",   sto_dpl_new },
  };

  for (unsigned int i = 0; i < sizeof (inits) / sizeof (inits[0]); ++i)
    if (strncmp(uri, inits[i].scheme, strlen(inits[i].scheme)) == 0)
      return inits[i].initializer(uri + strlen(inits[i].scheme));
}
