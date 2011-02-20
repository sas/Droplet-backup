#ifndef STORAGE_H_
# define STORAGE_H_

struct storage
{
  int (*store_file)(const char *src_path, const char *dst_path);
  int (*get_file)(const char *src_path, const char *dst_path);
  const char *local_root;
  const char *remote_root;
  void *specific_data;
};

struct storage *storage_new(const char *uri);

#endif /* !STORAGE_H_ */
