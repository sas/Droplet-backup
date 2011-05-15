#ifndef OPTIONS_H_
# define OPTIONS_H_

/* General options. */
# define OPT_CHANGEDIR    'c'
# define OPT_INTERACTIVE  'i'
# define OPT_NAME         'n'

/* Logging options. */
# define OPT_VERBOSE      'v'

/* Droplet related options. */
# define OPT_PROFILE      'p'
# define OPT_PROFILEDIR   'd'

extern const char *options[128];

int options_init(int argc, char *argv[]);

#endif /* !OPTIONS_H_ */
