##
##
## Copyright (c) 2011, Stephane Sezer
## All rights reserved.
## 
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##     * Redistributions of source code must retain the above copyright
##       notice, this list of conditions and the following disclaimer.
##     * Redistributions in binary form must reproduce the above copyright
##       notice, this list of conditions and the following disclaimer in the
##       documentation and/or other materials provided with the distribution.
##     * Neither the name of Stephane Sezer nor the names of its contributors
##       may be used to endorse or promote products derived from this software
##       without specific prior written permission.
## 
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL Stephane Sezer BE LIABLE FOR ANY DIRECT,
## INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
## LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
## ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##

include ../config.mk

TARGET		= dplbck

SRCS		= dplbck.c			\
		  commands/backup/backup.c	\
		  commands/help/help.c		\
		  commands/list/list.c		\
		  commands/restore/restore.c	\
		  commands/stats/stats.c	\
		  storage/dpl/dpl.c		\
		  storage/file/file.c		\
		  storage/storage.c		\
		  utils/buffer.c		\
		  utils/messages.c		\

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
