#include <openssl/sha.h>
#include <stdio.h>

#include "hash_file.h"

#define TMP_BUF_SIZE 4096

int hash_file(const char *filename, unsigned char hash[SHA_DIGEST_LENGTH])
{
  FILE *file;
  SHA_CTX ctx;
  unsigned char buf[TMP_BUF_SIZE];
  size_t size;

  if ((file = fopen(filename, "r")) == NULL)
    return 0;

  if (SHA1_Init(&ctx) == 0)
    goto err;

  while ((size = fread(buf, 1, TMP_BUF_SIZE, file)) > 0)
    if (SHA1_Update(&ctx, buf, size) == 0)
      goto err;
  
  if (!feof(file))
    goto err;

  if (SHA1_Final(hash, &ctx) == 0)
    goto err;

  fclose(file);
  return 1;

err:
  fclose(file);
  return 0;
}
