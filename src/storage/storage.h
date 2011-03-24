#ifndef STORAGE_H_
# define STORAGE_H_

struct storage
{
  int (*store)(struct storage *this, const char *path, void *data, unsigned int len);
  /* XXX: The prototype of the retrieve() needs change. */
  int (*retrieve)(const char *path);
  const char *remote_root;
  void *aux_data;
};

struct storage *storage_new(const char *uri);

#endif /* !STORAGE_H_ */
