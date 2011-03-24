#ifndef STO_FILE_H_
# define STO_FILE_H_

# include <storage/storage.h>

struct storage *sto_file_new(const char *uri, int create_dirs);

#endif /* !STO_FILE_H_ */
