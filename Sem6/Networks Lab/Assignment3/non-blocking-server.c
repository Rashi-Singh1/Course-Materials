//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#define MAX_CLIENTS 30
#define TRUE 1 
#define FALSE 0 
#define PORT 9002 
#define MAX_BUFFER_SIZE 1024
typedef struct sockaddr_in socketAddress;
typedef struct sockaddr* socketAddressPtr;

void printError(char* msg)
{
    perror(msg); 
	exit(EXIT_FAILURE);
}
	
int main(int argc , char *argv[]) 
{ 
    // Creating socket file descriptor 
	int server_TCP_socket; 
	if ((server_TCP_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) printError("Server Socket not created");
    else printf("Welcome socket created successfully\n");

    socketAddress server_TCP_addr; 	
    memset(&server_TCP_addr, 0, sizeof(server_TCP_addr)); 
	server_TCP_addr.sin_family = AF_INET; 
	server_TCP_addr.sin_addr.s_addr = INADDR_ANY; 
	server_TCP_addr.sin_port = htons( PORT ); 
	int server_TCP_addr_len = sizeof(server_TCP_addr);  
	
	// Forcefully attaching socket to the port PORT
	if (bind(server_TCP_socket, (socketAddressPtr)&server_TCP_addr, sizeof(server_TCP_addr))<0) printError("Welcome socket bind failure");
    else printf("Welcome socket bind success\n");

    //maximum no of pending connections
	if (listen(server_TCP_socket, 3) < 0) printError("Listen");

	char client_request[MAX_BUFFER_SIZE];  
		
	//set of socket descriptors 
	fd_set client_socket_bitmap; 
		
	//a message 
	char *message = "Hi there"; 
	
	//accept the incoming connection 
    int current_client , connection_socket[MAX_CLIENTS] , 
		 activity , read_status; 
	int max_socket_bit; 

	//initialise all client_socket[] to 0 so not checked 
	for (int i = 0; i < MAX_CLIENTS; i++) connection_socket[i] = 0;

	puts("Waiting for connections ..."); 
		
	while(TRUE) 
	{ 
		//clear the socket set 
		FD_ZERO(&client_socket_bitmap); 
	
		//add server welcome socket to set 
		FD_SET(server_TCP_socket, &client_socket_bitmap); 
		max_socket_bit = server_TCP_socket; 
			
		//add child sockets to set 
		for (int i = 0 ; i < MAX_CLIENTS ; i++) 
		{ 
            current_client = connection_socket[i];
			
			//if valid socket descriptor then add to read list 
			if(current_client > 0) FD_SET( current_client , &client_socket_bitmap); 
				
			//highest file descriptor number, need it for the select function 
			if(current_client > max_socket_bit) max_socket_bit = current_client; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_socket_bit + 1 , &client_socket_bitmap , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) printError("Select error");
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(server_TCP_socket, &client_socket_bitmap)) 
		{ 
			if ((current_client = accept(server_TCP_socket, 
					(struct sockaddr *)&server_TCP_addr, (socklen_t*)&server_TCP_addr_len))<0) printError("Accept");
            else printf("Connection Socket created successfully\n");
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , current_client , inet_ntoa(server_TCP_addr.sin_addr) , ntohs 
				(server_TCP_addr.sin_port)); 
		
			// //send new connection greeting message 
			// if( send(current_client, message, strlen(message), 0) != strlen(message) ) printError("Server welcome/post-connection msg not sent");
			// else puts("Welcome message sent successfully"); 
				
			//add new socket to array of sockets 
			for (int i = 0; i < MAX_CLIENTS; i++) 
			{ 
				//if position is empty 
				if( connection_socket[i] == 0 ) 
				{ 
					connection_socket[i] = current_client; 
					printf("Adding to list of sockets as %d\n" , i); 
					break; 
				} 
			} 
		} 
			
		//else its some IO operation on some other socket 
		for (int i = 0; i < MAX_CLIENTS; i++) 
		{ 
			current_client = connection_socket[i]; 
				
			if (FD_ISSET( current_client , &client_socket_bitmap)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message 
				if ((read_status = read( current_client , client_request, MAX_BUFFER_SIZE)) == 0) 
				{ 
					//Somebody disconnected , get his details and print 
					getpeername(current_client , (socketAddressPtr)&server_TCP_addr , (socklen_t*)&server_TCP_addr_len); 
					printf("Host disconnected , ip %s , port %d \n" , 
						inet_ntoa(server_TCP_addr.sin_addr) , ntohs(server_TCP_addr.sin_port)); 
						
					//Close the socket and mark as 0 in list for reuse 
					close( current_client ); 
					connection_socket[i] = 0; 
				} 
					
				else
				{ 
                    printf("Client request : %s\n",client_request ); 

                    printf("Please input port number\n");
                    int UDP_PORT;
                    scanf("%d",&UDP_PORT);

                    char UDP_PORT_str[MAX_BUFFER_SIZE];
                    sprintf(UDP_PORT_str,"%d",UDP_PORT);
                    
                    send(current_client , UDP_PORT_str , strlen(UDP_PORT_str) , 0 ); 
                    printf("UDP PORT sent\n\n\n"); 
				} 
			} 
		} 
	} 
	  //**************UDP port negotiation completed******************

    // int server_UDP_socket; 

    // // Creating socket file descriptor 
    // if ( (server_UDP_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
    //     perror("socket creation failed"); 
    //     exit(EXIT_FAILURE); 
    // } 
      
    // socketAddress server_UDP_addr;       
    // memset(&server_UDP_addr, 0, sizeof(server_UDP_addr)); 
      
    // // Filling server information 
    // server_UDP_addr.sin_family = AF_INET; // IPv4 
    // server_UDP_addr.sin_port = htons(UDP_PORT); 
    // server_UDP_addr.sin_addr.s_addr = INADDR_ANY; 
      
    // // Bind the socket with the server address 
    // if ( bind(server_UDP_socket, (const socketAddressPtr)&server_UDP_addr, sizeof(server_UDP_addr)) < 0 ) printError("Server UDP socket bind failure");
    // else printf("Server UDP socket bind success\n");
      
    // socketAddress client_UDP_addr;
    // memset(&client_UDP_addr, 0, sizeof(client_UDP_addr)); 

    // int client_UDP_addr_len = sizeof(client_UDP_addr);  
  
    // char client_msg[MAX_BUFFER_SIZE]; 
    // int client_msg_len;   

    // client_msg_len = recvfrom(server_UDP_socket, (char *)client_msg, MAX_BUFFER_SIZE, MSG_WAITALL, (socketAddressPtr) &client_UDP_addr, &client_UDP_addr_len); 
    // client_msg[client_msg_len] = '\0'; 
    // printf("Client UDP msg : %s\n", client_msg); 

    // char server_reply[MAX_BUFFER_SIZE];
    // printf("Please input response to the client\n");
    // scanf("%s",server_reply);

    // sendto(server_UDP_socket, (const char *)server_reply, strlen(server_reply), MSG_CONFIRM, (socketAddressPtr) &client_UDP_addr, client_UDP_addr_len); 
    // printf("Server reply sent.\n");
    // close(server_UDP_socket);
	return 0; 
} 
