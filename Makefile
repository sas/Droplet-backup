TARGET		= dplbck

SRCS		= dplbck.c			\
		  usage.c			\
		  commands/backup/backup.c	\
		  commands/help/help.c		\
		  commands/list/list.c		\
		  commands/restore/restore.c	\
		  commands/stats/stats.c	\
		  objects/hash_file.c		\

OBJS		= $(SRCS:.c=.o)

CFLAGS		+= -std=c99 -I. `pkg-config openssl --cflags`
LDFLAGS		+= `pkg-config openssl --libs`
MAKEFLAGS	+= --silent

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
