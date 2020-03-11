#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define MAX_BUFFER_SIZE 1024
#define PORT 9002
typedef struct connectionDetails details;
typedef struct sockaddr_in socketAddress;
typedef struct sockaddr* socketAddressPtr;

void printError(char* msg)
{
    perror(msg); 
	exit(EXIT_FAILURE);
}

void printError_without_Exit(char* msg)
{
    perror(msg);
}

void printClientDetails(socketAddress client_TCP_addr, int connection_socket_desc)
{
   
    printf("\n\tclient IP : %s \n\tclient port : %d\n\tvia connection socket : %d\n\n",  
        inet_ntoa(client_TCP_addr.sin_addr) , 
        ntohs(client_TCP_addr.sin_port),
        connection_socket_desc
    );
}

struct connectionDetails{
    int connection_socket_desc;
    socketAddress cur_client_addr;
}; 

//the thread function
void *connection_handler(void* );
 
int main(int argc , char *argv[])
{
    int server_TCP_socket;
     
    //Create welcome socket
    server_TCP_socket = socket(AF_INET , SOCK_STREAM , 0);
    if (server_TCP_socket == -1) printError("Welcome socket not created");
    else puts("Welcome socket created successfully\n");
     
    //fill the welcome socket address
    socketAddress server_TCP_addr; 
    server_TCP_addr.sin_family = AF_INET;

    //*********TODO: change this to user input IP and PORT********
    server_TCP_addr.sin_addr.s_addr = INADDR_ANY;
    server_TCP_addr.sin_port = htons( PORT );
     
    //Bind
    if( bind(   server_TCP_socket,
                (socketAddressPtr) &server_TCP_addr , 
                sizeof(server_TCP_addr)
            ) < 0) 
        printError("Welcome socket bind failure");
    else puts("Welcome socket bind success");
     
    //Listen
    if(listen(server_TCP_socket , 3) < 0) printError("Listen error");
    else puts("Waiting for incoming connections...");

    //connection socket after TCP handshake
    int connection_socket;

    //to store the client socket details which sent the connection request
	socketAddress client_TCP_addr;
    int client_TCP_addr_len = sizeof(socketAddress);

    //new thread for each new client
	pthread_t thread_id;

    while( (connection_socket = accept(
            server_TCP_socket, 
            (socketAddressPtr)&client_TCP_addr, 
            (socklen_t*) &client_TCP_addr_len
        )) 
    )
    {
        printf("New connection ");
        printClientDetails( 
            client_TCP_addr,
            connection_socket
        ); 

        details cur_client_details;
        cur_client_details.connection_socket_desc = connection_socket;
        cur_client_details.cur_client_addr = client_TCP_addr;

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &cur_client_details) < 0)
        {
            printError_without_Exit("Could not create thread");
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
    }
     
    if (connection_socket < 0) printError("Accept failed"); 
    return 0;
}
 

/*
 * This will handle connection for each client
 * */
void *connection_handler(void* cur_client)
{
    //Get the connection socket details
    details* cur_client_details = (details*)cur_client;
    int cur_connection_socket = cur_client_details->connection_socket_desc;
    socketAddress cur_client_addr = cur_client_details->cur_client_addr;

    // int cur_TCP_connection_socket = *(int*)socket_desc;
    char client_request_msg[MAX_BUFFER_SIZE] = {0}; 

    int read_status = read( cur_connection_socket , client_request_msg, MAX_BUFFER_SIZE); 
	printf("Client Request : %s \nfrom",client_request_msg ); 
    printClientDetails(cur_client_addr,cur_connection_socket);

    printf("Please input UDP port number for\n");
    printClientDetails(cur_client_addr,cur_connection_socket);
    
    int UDP_PORT;
    scanf("%d",&UDP_PORT);

    char UDP_PORT_str[MAX_BUFFER_SIZE];
    sprintf(UDP_PORT_str,"%d",UDP_PORT);


    //**************UDP port negotiation completed******************

    int server_UDP_socket; 

    // Creating socket file descriptor 
    //******confirm from Paul whether we need already created separate threads for each UDP connection*******
    if ( (server_UDP_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        printError_without_Exit("server UDP creation failure");
        send(cur_connection_socket , "" , 0 , 0 ); 
        close(cur_connection_socket);
    }
    else{
        socketAddress server_UDP_addr;       
        memset(&server_UDP_addr, 0, sizeof(server_UDP_addr)); 
        
        // Filling server information 
        server_UDP_addr.sin_family = AF_INET; // IPv4 
        server_UDP_addr.sin_port = htons(UDP_PORT); 
        server_UDP_addr.sin_addr.s_addr = INADDR_ANY; 

        cur_client_details->connection_socket_desc = server_UDP_socket;
        
        // Bind the socket with the server address 
        if ( bind(server_UDP_socket, (const socketAddressPtr)&server_UDP_addr, sizeof(server_UDP_addr)) < 0 ) {
            printError_without_Exit("Server UDP socket bind failure");
            send(cur_connection_socket , "" , 0 , 0 ); 
            close(cur_connection_socket);
        }
        else {
            printf("Server UDP socket bind success for");
            printClientDetails(cur_client_addr,cur_connection_socket);
            
            send(cur_connection_socket , UDP_PORT_str , strlen(UDP_PORT_str) , 0 ); 
            printf("UDP PORT sent to"); 
            printClientDetails(cur_client_addr,cur_connection_socket);
            printf("\n\n");
            close(cur_connection_socket);

            socketAddress client_UDP_addr;
            memset(&client_UDP_addr, 0, sizeof(client_UDP_addr)); 

            int client_UDP_addr_len = sizeof(client_UDP_addr);  
        
            char client_msg[MAX_BUFFER_SIZE]; 
            int client_msg_len;   

            client_msg_len = recvfrom(server_UDP_socket, (char *)client_msg, MAX_BUFFER_SIZE, MSG_WAITALL, (socketAddressPtr) &client_UDP_addr, &client_UDP_addr_len); 
            client_msg[client_msg_len] = '\0'; 
            printf("UDP msg : %s\nfrom", client_msg); 
            printClientDetails(cur_client_addr,cur_connection_socket);

            char server_reply[MAX_BUFFER_SIZE];
            printf("Please input msg response for");
            printClientDetails(cur_client_addr,cur_connection_socket);

            scanf("%s",server_reply);

            sendto(server_UDP_socket, (const char *)server_reply, strlen(server_reply), MSG_CONFIRM, (socketAddressPtr) &client_UDP_addr, client_UDP_addr_len); 
            printf("Server reply sent to");
            printClientDetails(cur_client_addr,cur_connection_socket);
        }
    }

    close(server_UDP_socket);
    
    return 0;
} 