#ifndef OPTIONS_H_
# define OPTIONS_H_

# include <stdbool.h>

struct options
{
  const char  *change_dir;
  bool         interactive;
  const char  *name;
  bool         verbose;
  const char  *profile;
  const char  *profile_dir;
};

int              options_init(int argc, char *argv[]);
struct options  *options_get(void);

#endif /* !OPTIONS_H_ */
