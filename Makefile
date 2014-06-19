CC = gcc

CFLAGS= -g -O2 -std=c99

INCLUDES = -I./src/include
LFALG = -L./src/libS
LIBS =

SVR_OBJECT_FILES = src/server.o src/lib/shmsem.o
CLT_OBJECT_FILES = src/client.o

OBJS_SRV = $(SVR_OBJECT_FILES:.c=.o)
OBJS_CLT = $(CLT_OBJECT_FILES:.c=.o)

all:    clean test run

test: $(CLT_OBJECT_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) -o test $(OBJS_CLT) $(LFLAGS) $(LIBS)

run: $(SVR_OBJECT_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) -o run $(OBJS_SRV) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ test run	