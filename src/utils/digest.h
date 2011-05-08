#ifndef DIGEST_H_
# define DIGEST_H_

# include <openssl/sha.h>
# include <stdio.h>

# include <utils/buffer.h>

/* Outputs of digest_file() and digest_buffer() are already string-formatted. */
# define DIGEST_SIZE  (2 * SHA_DIGEST_LENGTH + 1)

/*
** These function return a static buffer containing the requested hash.
** These buffers shall not be free'd by the user and should be considered
** invalid after any subsequent call to a digest_file() or digest_buffer().
**
** digest_buffer() can't fail. digest_file() can fail and return NULL if an I/O
** error occurs.
*/
const char *digest_file(FILE *file);
const char *digest_buffer(struct buffer *buffer);

#endif /* !DIGEST_H_ */
