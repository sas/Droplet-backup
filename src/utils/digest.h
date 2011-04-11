#ifndef DIGEST_H_
# define DIGEST_H_

# include <openssl/sha.h>
# include <stdio.h>

# include <utils/buffer.h>

/* Output of digest_file() and digest_buffer() are already string-formatted. */
# define DIGEST_SIZE  (2 * SHA_DIGEST_LENGTH + 1)

/*
** These function return a static buffer containing the requested hash, or NULL
** if an error occured.
** These buffers shall not be free'd by the user. And should be considered
** invalid after any subsequent call to a digest_file() or digest_buffer().
*/
const char *digest_file(FILE *file);
const char *digest_buffer(struct buffer *buffer);

#endif /* !DIGEST_H_ */
