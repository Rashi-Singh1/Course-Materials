.PHONY=all
.DEFAULT=all
all: a.out

a.out: client.c server.c non-blocking-server.c threaded_server.c
	gcc client.c -g -o out/client
	gcc server.c -g -lpthread -o out/server 
	gcc non-blocking-server.c -o out/non-blocking-server 
	gcc threaded_server.c -lpthread -o out/threaded_server 

clean: 
	-rm -f out/*.o 