#include <openssl/sha.h>
#include <stdio.h>

#include <utils/buffer.h>

#include "digest.h"

static char last_digest[DIGEST_SIZE];

static void update_last_digest(const unsigned char *md)
{
  static const char *to_hex = "0123456789abcdef";

  for (unsigned int i = 0; i < SHA_DIGEST_LENGTH; ++i)
  {
    last_digest[2 * i] = to_hex[md[i] >> 4];
    last_digest[2 * i + 1] = to_hex[md[i] & 0xf];
  }
  last_digest[DIGEST_SIZE - 1] = 0;
}

const char *digest_file(FILE *file)
{
  unsigned char md[SHA_DIGEST_LENGTH];
  SHA_CTX ctx;
  char buf[4096];
  int size;

  SHA1_Init(&ctx);
  fseek(file, 0, SEEK_SET);

  while ((size = fread(buf, 1, 4096, file)) > 0)
    SHA1_Update(&ctx, buf, size);

  if (ferror(file))
    return NULL;

  SHA1_Final(md, &ctx);
  update_last_digest(md);
  return last_digest;
}

const char *digest_buffer(struct buffer *buffer)
{
  unsigned char md[SHA_DIGEST_LENGTH];

  SHA1(buffer->data, buffer->used, md);

  update_last_digest(md);
  return last_digest;
}
