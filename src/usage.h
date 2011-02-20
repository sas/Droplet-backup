#ifndef USAGE_H_
# define USAGE_H_

# include <stdio.h>

void usage(FILE *output);
void err(int exit_val, const char *format, ...);
void warn(const char *format, ...);

#endif /* !USAGE_H_ */
