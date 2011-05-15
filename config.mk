# customize below to fit your system
DPL_PKG_CONFIG	= /usr/local/lib/pkgconfig/

# compiler and flags
CC		= gcc
CFLAGS		= -Wall -Wextra -Werror -g
LDFLAGS		=
MAKEFLAGS	= --silent

# required flags
CFLAGS		+= -I. -std=c99 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64

# openssl
CFLAGS		+= `pkg-config --cflags openssl`
LDFLAGS		+= `pkg-config --libs openssl`

# libdroplets
CFLAGS		+= `PKG_CONFIG_PATH=$(DPL_PKG_CONFIG) pkg-config --cflags droplet-1.0`
LDFLAGS		+= `PKG_CONFIG_PATH=$(DPL_PKG_CONFIG) pkg-config --libs droplet-1.0`

# libedit
CFLAGS		+= `pkg-config --cflags libedit`
LDFLAGS		+= `pkg-config --libs libedit`
