CFLAGS=-Wall -g -O2 -std=gnu99 -I./src/inc -L./src/lib

all: Server Client

clean:
rm run test

Server: Server.c
gcc $(CFLAGS) -o run src/server.c

Client: Client.c
gcc $(CFLAGS) -o test src/client.c