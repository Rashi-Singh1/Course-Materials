.PHONY=all
.DEFAULT=all
all: a.out

a.out: ../client.c ../server.c
	gcc ../client.c -g -o client
	gcc ../server.c -g -lpthread -o server