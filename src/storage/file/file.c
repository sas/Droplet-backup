#define STORAGE_INTERNAL

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <storage/storage.h>
#include <usage.h>

#include "file.h"

struct file_storage_state
{
  char *remote_root;
  DIR *last_list;
};

static int sto_file_store(void *state, const char *path, struct buffer *data)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 1];
  int fd = -1;
  int size, full_size;

  strcpy(full_path, s->remote_root);
  strcat(full_path, "/");
  strcat(full_path, path);

  if ((fd = open(full_path, O_WRONLY | O_EXCL | O_CREAT, 0666)) == -1)
  {
    /*
    ** If file already exists, this means that the data is already stored, so
    ** this is not an actual failure.
    */
    if (errno == EEXIST)
      return 1;
    else
      goto err;
  }

  full_size = 0;
  while ((size = write(fd, data->data + full_size, data->size - full_size)) > 0)
    full_size += size;

  if (size == -1)
    goto err;

  close(fd);

  return 1;

err:
  perror(full_path);
  if (fd != -1)
    close(fd);
  return 0;
}

static struct buffer *sto_file_retrieve(void *state, const char *path)
{
  struct file_storage_state *s = state;
  char full_path[strlen(s->remote_root) + strlen(path) + 2];
  struct buffer *res = NULL;
  int size, full_size;
  int fd = -1;
  struct stat buf;

  strcpy(full_path, s->remote_root);
  strcat(full_path, "/");
  strcat(full_path, path);

  if ((fd = open(full_path, O_RDONLY)) == -1)
    goto err;

  if (fstat(fd, &buf) == -1)
    goto err;

  if ((res = malloc(sizeof (struct buffer) + buf.st_size)) == NULL)
    goto err;

  full_size = 0;
  while ((size = read(fd, res->data + full_size, res->size - full_size)) > 0)
    full_size += size;

  if (size == -1)
    goto err;

  close(fd);

  return res;

err:
  perror(full_path);
  if (fd != -1)
    close(fd);
  if (res != NULL)
    free(NULL);
  return NULL;
}

static const char *sto_file_list(void *state, const char *path)
{
  struct file_storage_state *s = state;
  struct dirent *dir;

  if (path != NULL)
  {
    char full_path[strlen(s->remote_root) + strlen(path) + 1];

    strcpy(full_path, s->remote_root);
    strcat(full_path, "/");
    strcat(full_path, path);

    s->last_list = opendir(full_path);
  }

  if (s->last_list == NULL)
    return NULL;

  if ((dir = readdir(s->last_list)) != NULL)
    return dir->d_name;
  else
    return NULL;
}

struct storage *sto_file_new(const char *uri)
{
  struct storage *res = NULL;
  struct file_storage_state *state = NULL;
  char *wuri = NULL;

  if ((res = malloc(sizeof (struct storage))) == NULL)
    goto err;
  if ((state = malloc(sizeof (struct file_storage_state))) == NULL)
    goto err;

  if ((wuri = strdup(uri)) == NULL)
    goto err;

  res->store = sto_file_store;
  res->retrieve = sto_file_retrieve;
  res->list = sto_file_list;
  state->remote_root = wuri;
  state->last_list = NULL;
  res->state = state;

  /*
  ** Ensure we have access to the directory where we want to backup, and create
  ** required subdirectories (e.g.: `backups`, `objects`) if they do not exist
  ** yet.
  ** XXX: Some parts of this code are a little stupid.
  */
  {
    char path[strlen(state->remote_root) + strlen("/backups") + 1];

    strcpy(path, state->remote_root);
    if (mkdir(path, 0777) == -1)
      if (errno != EEXIST)
      {
        fprintf(stderr, "fail : %s\n", path);
        goto err;
      }

    strcat(path, "/backups");
    if (mkdir(path, 0777) == -1)
      if (errno != EEXIST)
      {
        fprintf(stderr, "fail : %s\n", path);
        goto err;
      }

    strcpy(path, state->remote_root);
    strcat(path, "/objects"); // strlen("/backups") == strlen("/objects")
    if (mkdir(path, 0777) == -1)
      if (errno != EEXIST)
      {
        fprintf(stderr, "fail : %s\n", path);
        goto err;
      }
  }

  return res;

err:
  if (res != NULL)
    free(res);
  if (state != NULL)
    free(state);
  if (wuri != NULL)
    free(wuri);
  return NULL;
}
