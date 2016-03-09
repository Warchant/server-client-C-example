all: server client

server:
	gcc -std=c99 -o server server.c 

client:
	gcc -std=c99 -o client client.c 

clear:
	rm server client
