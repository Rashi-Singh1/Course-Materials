.PHONY=all clean
.DEFAULT=all
all: a.out

a.out: client.c server.c
	gcc client.c -w -g -o out/client
	gcc server.c -w -g -lpthread -o out/server 

clean: 
	-rm -f out/*.o 