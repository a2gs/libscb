# Andre Augusto Giannotti Scota (https://sites.google.com/view/a2gs/)
# C flags:
CFLAGS_OPTIMIZATION = -g
#CFLAGS_OPTIMIZATION = -O3
CFLAGS_VERSION = -std=c11
CFLAGS_WARNINGS = -Wall -Wextra -Wno-unused-parameter -Wno-unused-but-set-parameter
CFLAGS_DEFINES = -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE=1 -D_DEFAULT_SOURCE=1
CFLAGS = $(CFLAGS_OPTIMIZATION) $(CFLAGS_VERSION) $(CFLAGS_WARNINGS) $(CFLAGS_DEFINES)

# System shell utilities
CC = gcc
RM = rm -fr
CP = cp
AR = ar
RANLIB = ranlib
CPPCHECK = cppcheck

SRCPATH=./src
INCPATH=./inc
BINPATH=./bin

LIBCOMPPATH = -L$(BINPATH)
INCCOMPPATH = -I$(INCPATH)

LIBS = -lpthread -lrt

all: clean lscb

lscb:
	@echo
	@echo "=== Compiling libscb ================="
	$(CC) -o $(BINPATH)/scb.o -c $(SRCPATH)/scb.c $(INCCOMPPATH) $(LIBS) $(CFLAGS)
	$(AR) rc $(BINPATH)/libscb.a $(BINPATH)/scb.o
	$(RANLIB) $(BINPATH)/libscb.a
	-$(RM) $(BINPATH)/scb.o

sample: lscb
	@echo
	@echo "=== Compiling sample =============="
	$(CC) -o $(BINPATH)/prod $(SRCPATH)/prod.c $(CFLAGS) $(INCCOMPPATH) $(LIBCOMPPATH) $(LIBS) -lscb
	$(CC) -o $(BINPATH)/cons $(SRCPATH)/cons.c $(CFLAGS) $(INCCOMPPATH) $(LIBCOMPPATH) $(LIBS) -lscb
	$(CC) -o $(BINPATH)/info $(SRCPATH)/info.c $(CFLAGS) $(INCCOMPPATH) $(LIBCOMPPATH) $(LIBS) -lscb
	$(CC) -o $(BINPATH)/destroy $(SRCPATH)/destroy.c $(CFLAGS) $(INCCOMPPATH) $(LIBCOMPPATH) $(LIBS) -lscb
	$(CC) -o $(BINPATH)/iterator $(SRCPATH)/iterator.c $(CFLAGS) $(INCCOMPPATH) $(LIBCOMPPATH) $(LIBS) -lscb
	$(CC) -o $(BINPATH)/producer_consumer_2 $(SRCPATH)/producer_consumer_2.c $(CFLAGS) $(INCCOMPPATH) $(LIBCOMPPATH) $(LIBS) -lscb

clean:
	@echo
	@echo "=== clean_data =============="
	-$(RM) $(BINPATH)/*
