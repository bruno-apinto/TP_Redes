all: client server

client: src/client.c src/FLIP.c
	gcc -Wall -Iinclude src/client.c src/FLIP.c -o bin/client

server: src/server.c src/FLIP.c
	gcc -Wall -Iinclude src/server.c src/FLIP.c -o bin/server

clean:
	rm -f bin/client bin/server
