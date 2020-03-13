#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<stdbool.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#define MAX_BUFFER_SIZE 1024
//#define PORT 9002
typedef struct connectionDetails details;
typedef struct sockaddr_in socketAddress;
typedef struct sockaddr* socketAddressPtr;
typedef struct message_format Message;
typedef enum protocol Protocol;
typedef enum con_status CON_STATUS;
typedef enum msg_status MSG_STATUS;
enum protocol {TCP, UDP};
enum con_status {ACTIVE, CLOSED};
enum msg_status {SENT, RECEIVED, NONE};

bool freePorts[300];
int basePort = 1800;

struct message_format
{
    uint8_t type;
    uint16_t len;
    char msg[MAX_BUFFER_SIZE];
};

struct connectionDetails{
    int connection_socket_desc;
    socketAddress cur_client_addr;
}; 

void printError(char* msg)
{
    perror(msg); 
    exit(EXIT_FAILURE);
}

void printError_without_Exit(char* msg)
{
    perror(msg);
}

void printfError_without_Exit(char* msg)
{
    printf("%s\n",msg);
}

void printfError(char* msg)
{
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}

void printDetails(socketAddress client_TCP_addr, int connection_socket_desc, int pro, Message message, int msg_stat, int stat)
{
    char curProtocol[5];
    char conStatus[10];

    if(pro == TCP) strcpy(curProtocol,"TCP");
    else strcpy(curProtocol,"UDP");

    if(stat == ACTIVE) strcpy(conStatus,"ACTIVE");
    else strcpy(conStatus,"CLOSED");

    char msgStatus[10] = "--------";
    char colon[5] = "::";
    if(msg_stat == SENT) strcpy(msgStatus,"SENT    ");
    else if(msg_stat == RECEIVED) strcpy(msgStatus,"RECEIVED");

    if(message.type == -1) strcpy(colon,"");

    printf("  %s   %s\t  %d\t    %d\t\t %s\t%s %d%s %s\n",  
        conStatus,
        inet_ntoa(client_TCP_addr.sin_addr) , 
        ntohs(client_TCP_addr.sin_port),
        connection_socket_desc,
        curProtocol,
        msgStatus,
        message.type,
        colon,
        message.msg
    );
    
}
 Message emptyMessage;

//the thread function
void *connection_handler(void* );
 
int main(int argc , char *argv[])
{
    //initialize ports
    int i = 0;
    for(i = 0;i < 300;i++) freePorts[i] = true;

    //intialize empty msg

    emptyMessage.type = -1;
    strcpy(emptyMessage.msg,"------");
    emptyMessage.len = 0;

    if(argc!=2)
    {
        printError("Use command ./server <server port number>");
    }
    int server_TCP_socket;
    int PORT = atoi(argv[1]);

    //Create welcome socket
    server_TCP_socket = socket(AF_INET , SOCK_STREAM , 0);
    if (server_TCP_socket == -1) printError("Welcome socket not created");
    else puts("TCP Welcome socket created successfully");
     
    //fill the welcome socket address
    socketAddress server_TCP_addr; 
    server_TCP_addr.sin_family = AF_INET;

    //Server IP is localhost and PORT is taken from command line
    server_TCP_addr.sin_addr.s_addr = INADDR_ANY;
    server_TCP_addr.sin_port = htons( PORT );
     
    //Bind
    if( bind(   server_TCP_socket,
                (socketAddressPtr) &server_TCP_addr , 
                sizeof(server_TCP_addr)
            ) < 0) 
        printError("Welcome socket bind failure");
    else puts("TCP Welcome socket bind success!");
     
    //Listen
    if(listen(server_TCP_socket , 3) < 0) printError("Listen error");
    else puts("Waiting for incoming connections...\n\n");
    puts("\nConnection Client-IP  Client-port  Socket-ID  Protocol\tMsg-Status\tMessage");  
    puts("--------------------------------------------------------------------------------------------------------------------------------");  

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
        printDetails( 
            client_TCP_addr,
            connection_socket,
            TCP,
            emptyMessage,
            NONE,
            ACTIVE
        ); 

        details cur_client_details;
        cur_client_details.connection_socket_desc = connection_socket;
        cur_client_details.cur_client_addr = client_TCP_addr;

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &cur_client_details) < 0)
        {
            printError_without_Exit("Could not create thread");
            // close(connection_socket);
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
    }
     
    if (connection_socket < 0) printError("Accept failed"); 
    return 0;
}
 
int findFreePort()
{
    int freeIndex = 0;
    for(freeIndex = 0; freeIndex < 300; freeIndex++)
    {
        if(freePorts[freeIndex]) return freeIndex + basePort;
    }
    return -1;
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

    //To store and send message of given format
    Message message; 

    int read_status = read( cur_connection_socket , &message, sizeof(message)); 
    printDetails(cur_client_addr,cur_connection_socket,TCP,message,RECEIVED,ACTIVE);

    int UDP_PORT = findFreePort();
    if(UDP_PORT == -1) printfError_without_Exit("Client limit exceeded");
    else{
        /**************Check if UDP socket creation or bind fail******************/
        freePorts[UDP_PORT - basePort] = false;

        int server_UDP_socket; 
        char UDP_PORT_str[MAX_BUFFER_SIZE];
        sprintf(UDP_PORT_str,"%d",UDP_PORT);

        // Creating socket file descriptor 
        if ( (server_UDP_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
            printError_without_Exit("server UDP creation failure");
            send(cur_connection_socket , "" , 0 , 0 ); 
            close(cur_connection_socket);
            freePorts[UDP_PORT - basePort] = true;

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
                freePorts[UDP_PORT - basePort] = true;

            }
            else {
                // printf("Server UDP socket bind success\nPORT: %d\nClient:",UDP_PORT);
                // printDetails(cur_client_addr,cur_connection_socket);

                /*UDP PORT created succesfully on server, send port number to client and close TCP connection*/
                message.type=2;
                message.len = strlen(UDP_PORT_str);
                strcpy(message.msg, UDP_PORT_str);
                send(cur_connection_socket , &message , sizeof(message) , 0 ); 


                // printf("Message type %d:\t UDP PORT number %d sent to Client:", message.type, UDP_PORT); 
                printDetails(cur_client_addr,cur_connection_socket,UDP,message,SENT,ACTIVE);
                printf("\n\n");
                close(cur_connection_socket); //CLose TCP connection
                // printf("------------- TCP connection closed ------------\n");

                socketAddress client_UDP_addr;
                memset(&client_UDP_addr, 0, sizeof(client_UDP_addr)); 

                int client_UDP_addr_len = sizeof(client_UDP_addr);  
            
                int client_msg_len;   

                client_msg_len = recvfrom(server_UDP_socket, &message, sizeof(message) , MSG_WAITALL, (socketAddressPtr) &client_UDP_addr, &client_UDP_addr_len); 
                message.msg[message.len] = '\0'; 
                // printf("Message type %d:\n UDP message received: %s\nfrom Client:", message.type, message.msg); 
                printDetails(cur_client_addr,cur_connection_socket,UDP,message,RECEIVED,ACTIVE);

                char server_reply[MAX_BUFFER_SIZE] = "Message_received_successfully\n";

                scanf("%s",server_reply);
                //send Message type 4 to client
                message.type=4;
                message.len = strlen(server_reply);
                strcpy(message.msg, server_reply);
                sendto(server_UDP_socket, &message, sizeof(message) , MSG_CONFIRM, (socketAddressPtr) &client_UDP_addr, client_UDP_addr_len); 
                // printf("Message type %d:\nServer reply sent to", message.type);
                printDetails(cur_client_addr,cur_connection_socket,UDP,message,SENT,ACTIVE);
            }
        }


        close(server_UDP_socket);
        freePorts[UDP_PORT - basePort] = true;
        // printf("------------- UDP socket closed ------------\n");
    }
    freePorts[UDP_PORT - basePort] = true;
    return 0;
} 
