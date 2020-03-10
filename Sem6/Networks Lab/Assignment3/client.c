// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h>

#define PORT 8080 
typedef struct sockaddr_in socketAddress;

void printError(char* msg)
{
    perror(msg); 
	exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]) 
{ 
	int client_socket = -1; 
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) printError("Client Socket not created");
    else printf("Client socket created successfully\n");

	socketAddress server_addr; 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(PORT); 
	// server_addr.sin_addr.s_addr = INADDR_ANY;

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) printError("Connection error");
    else printf("TCP Connection created successfully\n");

    char *request_msg = "UDP port requested"; 
	char server_response[1024] = {0}; 

	send(client_socket , request_msg , strlen(request_msg) , 0 ); 
	printf("UDP port request sent\n"); 

    int read_status = -1;
	read_status = read( client_socket , server_response, 1024); 
	printf("Server Response : %s\n",server_response ); 
	return 0; 
} 
