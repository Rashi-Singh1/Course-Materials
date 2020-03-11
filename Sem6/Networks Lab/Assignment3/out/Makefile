.PHONY=all
.DEFAULT=all
all: a.out

a.out: ../client.c ../server.c
	gcc ../client.c -o client
	gcc ../server.c -lpthread -o server