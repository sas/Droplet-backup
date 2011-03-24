#ifndef USAGE_H_
# define USAGE_H_

# include <stdio.h>

# define EXIT_BAD_USAGE    1
# define EXIT_UNK_COMMAND  2
# define EXIT_UNK_STORAGE  3

void usage(FILE *output);
void usage_die(void);
void err(int exit_val, const char *format, ...);
void warn(const char *format, ...);

#endif /* !USAGE_H_ */
