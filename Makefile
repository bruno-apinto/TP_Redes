all:
	gcc -Wall FLIP.c Server/server.c -o server
	gcc -Wall FLIP.c Client/client.c -o client

server: 
	gcc -Wall FLIP.c Server/server.c -o server

client:
	gcc -Wall FLIP.c Client/client.c -o client
