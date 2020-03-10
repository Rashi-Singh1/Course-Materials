// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 
typedef struct sockaddr_in socketAddress;

void printError(char* msg)
{
    perror(msg); 
	exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]) 
{ 
	// Creating socket file descriptor 
	int server_socket; 
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) printError("Server Socket not created");
    else printf("Welcome socket created successfully\n");

    socketAddress server_addr; 	
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	server_addr.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))<0) printError("Welcome socket bind failure");
    else printf("Welcome socket bind success\n");

	if (listen(server_socket, 3) < 0) printError("Listen");

	int server_addr_len = sizeof(server_addr);  
    int connection_socket;
	if ((connection_socket = accept(server_socket, (struct sockaddr *)&server_addr, (socklen_t*)&server_addr_len))<0) printError("Accept");
    else printf("Connection Socket created successfully\n");

    char client_request[1024] = {0}; 

    int read_status = -1;
	read_status = read( connection_socket , client_request, 1024); 
	printf("Client request : %s\n",client_request ); 

    printf("Please input port number\n");
    int UDP_PORT;
    scanf("%d",&UDP_PORT);

    char UDP_PORT_str[1024];
    sprintf(UDP_PORT_str,"%d",UDP_PORT);
    
	send(connection_socket , UDP_PORT_str , strlen(UDP_PORT_str) , 0 ); 
	printf("UDP PORT sent\n"); 
	return 0; 
} 
