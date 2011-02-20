include ../config.mk

TARGET		= dplbck

SRCS		= dplbck.c			\
		  usage.c			\
		  commands/backup/backup.c	\
		  commands/help/help.c		\
		  commands/list/list.c		\
		  commands/restore/restore.c	\
		  commands/stats/stats.c	\
		  objects/hash_file.c		\
		  storage/dpl/dpl.c		\
		  storage/storage.c		\

OBJS		= $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	echo "LD	$@"
	$(CC) $^ -o $@ $(LDFLAGS)

.c.o:
	echo "CC	$^"
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	echo "RM	OBJS"
	rm -f $(OBJS)

distclean: clean
	echo "RM	$(TARGET)"
	rm -f $(TARGET)
