// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h>

#define PORT 9002 
#define MAX_BUFFER_SIZE 1024 
typedef struct sockaddr_in socketAddress;
typedef struct sockaddr* socketAddressPtr;

void printError(char* msg)
{
    perror(msg); 
	exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]) 
{ 
    //**************UDP port negotiation******************

	int client_TCP_socket = -1; 
	if ((client_TCP_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) printError("Client TCP socket not created");
    else printf("Client TCP socket created successfully\n");

	socketAddress server_TCP_addr; 
    memset(&server_TCP_addr, 0, sizeof(server_TCP_addr)); 
	server_TCP_addr.sin_family = AF_INET; 
	server_TCP_addr.sin_port = htons(PORT); 
	server_TCP_addr.sin_addr.s_addr = INADDR_ANY;

	if (connect(client_TCP_socket, (socketAddressPtr)&server_TCP_addr, sizeof(server_TCP_addr)) < 0) printError("Connection error");
    else printf("TCP Connection created successfully\n");

    char *request_msg = "UDP port requested"; 
	char UDP_port_str[MAX_BUFFER_SIZE] = {0};  //server response

	send(client_TCP_socket , request_msg , strlen(request_msg) , 0 ); 
	printf("UDP port request sent\n"); 

    int read_status = -1;
	read_status = read( client_TCP_socket , UDP_port_str, MAX_BUFFER_SIZE); 
	printf("Server UDP port : %s\n\n\n",UDP_port_str ); 

    int UDP_PORT = atoi(UDP_port_str);

    close(client_TCP_socket);

    //**************UDP port negotiation completed******************

    int client_UDP_socket; 

    // Creating socket file descriptor 
    if ( (client_UDP_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) printError("Client UDP Socket not created");
    else printf("Client UDP socket created successfully\n");

    //filling server information
    socketAddress server_UDP_addr; 
    memset(&server_UDP_addr, 0, sizeof(server_UDP_addr)); 
    server_UDP_addr.sin_family = AF_INET; 
    server_UDP_addr.sin_port = htons(UDP_PORT); 
    server_UDP_addr.sin_addr.s_addr = INADDR_ANY; 
      
    int server_UDP_addr_len; 
    char client_msg[MAX_BUFFER_SIZE];   
    printf("Please input msg to the server\n");
    scanf("%s",client_msg);

    sendto(client_UDP_socket, (const char *)client_msg, strlen(client_msg), MSG_CONFIRM, (socketAddressPtr) &server_UDP_addr, sizeof(server_UDP_addr)); 
    printf("Client message sent.\n"); 
          
    char server_response[MAX_BUFFER_SIZE]; 
    int server_response_len = recvfrom(client_UDP_socket, (char *)server_response, MAX_BUFFER_SIZE, MSG_WAITALL, (socketAddressPtr) &server_UDP_addr, &server_UDP_addr_len); 
    server_response[server_response_len] = '\0'; 
    printf("Server response : %s\n", server_response); 
  
    close(client_UDP_socket); 
	return 0; 
} 
