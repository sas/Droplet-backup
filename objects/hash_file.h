#ifndef HASH_FILE_H_
# define HASH_FILE_H_

# include <openssl/sha.h>

int hash_file(const char *filename, unsigned char hash[SHA_DIGEST_LENGTH]);

#endif /* !HASH_FILE_H_ */
