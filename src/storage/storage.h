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
  void *state;
};
# endif /* STORAGE_INTERNAL */

typedef struct storage *storage_t;

storage_t storage_new(const char *uri);
int storage_store(storage_t storage, const char *path, struct buffer *data);
struct buffer *storage_retrieve(storage_t storage, const char *path);
const char *storage_list(storage_t storage, const char *path);

#endif /* !STORAGE_H_ */
