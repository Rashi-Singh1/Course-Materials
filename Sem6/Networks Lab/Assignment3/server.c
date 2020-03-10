// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 9002
#define MAX_BUFFER_SIZE 1024 
#define MAX_CLIENTS 30
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

	// Creating socket file descriptor 
	int server_TCP_socket; 
	if ((server_TCP_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) printError("Server Socket not created");
    else printf("Welcome socket created successfully\n");

    socketAddress server_TCP_addr; 	
    memset(&server_TCP_addr, 0, sizeof(server_TCP_addr)); 
	server_TCP_addr.sin_family = AF_INET; 
	server_TCP_addr.sin_addr.s_addr = INADDR_ANY; 
	server_TCP_addr.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port PORT
	if (bind(server_TCP_socket, (socketAddressPtr)&server_TCP_addr, sizeof(server_TCP_addr))<0) printError("Welcome socket bind failure");
    else printf("Welcome socket bind success\n");

    //maximum no of pending connections
	if (listen(server_TCP_socket, 3) < 0) printError("Listen");

	int server_TCP_addr_len = sizeof(server_TCP_addr);  
    int connection_socket;
	if ((connection_socket = accept(server_TCP_socket, (socketAddressPtr)&server_TCP_addr, (socklen_t*)&server_TCP_addr_len))<0) printError("Accept");
    else printf("Connection Socket created successfully\n");

    char client_request[MAX_BUFFER_SIZE] = {0}; 

    int read_status = -1;
	read_status = read( connection_socket , client_request, MAX_BUFFER_SIZE); 
	printf("Client request : %s\n",client_request ); 

    printf("Please input port number\n");
    int UDP_PORT;
    scanf("%d",&UDP_PORT);

    char UDP_PORT_str[MAX_BUFFER_SIZE];
    sprintf(UDP_PORT_str,"%d",UDP_PORT);
    
	send(connection_socket , UDP_PORT_str , strlen(UDP_PORT_str) , 0 ); 
	printf("UDP PORT sent\n\n\n"); 

    close(connection_socket);

    //**************UDP port negotiation completed******************

    int server_UDP_socket; 

    // Creating socket file descriptor 
    if ( (server_UDP_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    socketAddress server_UDP_addr;       
    memset(&server_UDP_addr, 0, sizeof(server_UDP_addr)); 
      
    // Filling server information 
    server_UDP_addr.sin_family = AF_INET; // IPv4 
    server_UDP_addr.sin_port = htons(UDP_PORT); 
    server_UDP_addr.sin_addr.s_addr = INADDR_ANY; 
      
    // Bind the socket with the server address 
    if ( bind(server_UDP_socket, (const socketAddressPtr)&server_UDP_addr, sizeof(server_UDP_addr)) < 0 ) printError("Server UDP socket bind failure");
    else printf("Server UDP socket bind success\n");
      
    socketAddress client_UDP_addr;
    memset(&client_UDP_addr, 0, sizeof(client_UDP_addr)); 

    int client_UDP_addr_len = sizeof(client_UDP_addr);  
  
    char client_msg[MAX_BUFFER_SIZE]; 
    int client_msg_len;   

    client_msg_len = recvfrom(server_UDP_socket, (char *)client_msg, MAX_BUFFER_SIZE, MSG_WAITALL, (socketAddressPtr) &client_UDP_addr, &client_UDP_addr_len); 
    client_msg[client_msg_len] = '\0'; 
    printf("Client UDP msg : %s\n", client_msg); 

    char server_reply[MAX_BUFFER_SIZE];
    printf("Please input response to the client\n");
    scanf("%s",server_reply);

    sendto(server_UDP_socket, (const char *)server_reply, strlen(server_reply), MSG_CONFIRM, (socketAddressPtr) &client_UDP_addr, client_UDP_addr_len); 
    printf("Server reply sent.\n");
    close(server_UDP_socket);
	return 0; 
} 
