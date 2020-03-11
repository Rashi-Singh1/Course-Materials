#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define MAX_BUFFER_SIZE 1024
typedef struct sockaddr_in socketAddress;
typedef struct sockaddr* socketAddressPtr;

void printError(char* msg)
{
    perror(msg); 
	exit(EXIT_FAILURE);
}

//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) printError("Could not create socket");
    else puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 9002 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) printError("Bind error");
    else puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            printError("Could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0) printError("Accept failed"); 
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int connection_socket = *(int*)socket_desc;
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