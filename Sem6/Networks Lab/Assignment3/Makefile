.PHONY=all
.DEFAULT=all
all: a.out

a.out: client.c server.c
	gcc client.c -o out/client
	gcc server.c -o out/server 

clean: 
	-rm -f out/*.o 