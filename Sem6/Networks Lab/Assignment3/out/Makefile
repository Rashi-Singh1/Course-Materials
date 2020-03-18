.PHONY=all clean
.DEFAULT=all
all: a.out

a.out: ../client.c ../server.c
	gcc ../client.c -w -g -o client
	gcc ../server.c -w -g -lpthread -o server 

clean: 
	-rm -f *.o 