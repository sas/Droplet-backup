#ifndef USAGE_H_
# define USAGE_H_

# include <stdio.h>

# define EXIT_USAGE_FAIL    1
# define EXIT_STORAGE_FAIL  2

void usage(FILE *output);
void usage_die(void);
void err(int exit_val, const char *format, ...);
void warn(const char *format, ...);

#endif /* !USAGE_H_ */
