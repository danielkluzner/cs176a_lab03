CC = gcc
ARGS = -Wall

all: PingClient

PingClient: client.c
	$(CC) $(ARGS) -o PingClient client.c

clean:
	rm -f *.o *~ PingClient
