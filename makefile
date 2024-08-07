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

INCLUDEPATH = -I./
LIBS = -lscb -lpthread -lrt
LIBPATH = -L./

all: clean lscb

lscb:
	@echo
	@echo "=== libscb ================="
	$(CC) -o scb.o -c scb.c -lpthread -lrt $(CFLAGS)
	$(AR) rc libscb.a scb.o
	$(RANLIB) libscb.a
	-$(RM) scb.o

sample: lscb
	@echo
	@echo "=== Compiling =============="
	$(CC) -o prod prod.c $(CFLAGS) $(INCLUDEPATH) $(LIBPATH) $(LIBS)
	$(CC) -o cons cons.c $(CFLAGS) $(INCLUDEPATH) $(LIBPATH) $(LIBS)
	$(CC) -o info info.c $(CFLAGS) $(INCLUDEPATH) $(LIBPATH) $(LIBS)
	$(CC) -o destroy destroy.c $(CFLAGS) $(INCLUDEPATH) $(LIBPATH) $(LIBS)
	$(CC) -o iterator iterator.c $(CFLAGS) $(INCLUDEPATH) $(LIBPATH) $(LIBS)
	$(CC) -o producer_consumer_2 producer_consumer_2.c $(CFLAGS) $(INCLUDEPATH) $(LIBPATH) $(LIBS)

clean:
	@echo
	@echo "=== clean_data =============="
	-$(RM) prod cons info destroy iterator producer_consumer_2 libscb.a *.o core
