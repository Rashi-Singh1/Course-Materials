// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h>

//#define PORT 9002 
#define MAX_BUFFER_SIZE 1024 
typedef struct sockaddr_in socketAddress;
typedef struct sockaddr* socketAddressPtr;
typedef struct message_format Message;

struct message_format
{
    uint8_t type;
    uint16_t len;
    char msg[MAX_BUFFER_SIZE];
};

void printError(char* msg)
{
    perror(msg); 
    exit(EXIT_FAILURE);
}

void printfError(char* msg)
{
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}

int PORT;
Message message;

int main(int argc, char const *argv[]) 
{ 
    //client needs to specify the server IP and PORT
    //IP can be specified as 0 or 172.0.0.1 : both represent the local host
    if(argc!=3) printfError("Use command: ./client <Server IP> <Server Port number>");

    PORT = atoi(argv[2]);

    //**************UDP port negotiation begins******************
   
    int client_TCP_socket = -1; 
    if ((client_TCP_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) printError("Client TCP socket not created");
    else printf("Client TCP socket created successfully\n");

    socketAddress server_TCP_addr; 
    memset(&server_TCP_addr, 0, sizeof(server_TCP_addr)); 
    server_TCP_addr.sin_family = AF_INET; 
    server_TCP_addr.sin_port = htons(PORT); 
    server_TCP_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(client_TCP_socket, (socketAddressPtr)&server_TCP_addr, sizeof(server_TCP_addr)) < 0) printError("TCP connection error");
    else printf("TCP connection created successfully\n");

    char *request_msg = "UDP port requested";
    message.type=1;
    message.len=strlen(request_msg);
    strcpy(message.msg, request_msg);

    send(client_TCP_socket , &message , sizeof(message) , 0 ); 
    printf("Message type %d:\tUDP port request sent to Server \n", message.type); 

    int read_status = -1;
    read_status = read( client_TCP_socket , &message , sizeof(message));

    if(read_status==0) printError("Server did not respond");
    if(message.type!=2) printfError("Invalid message from server");
    if(message.len <= 0) printfError("Invalid server port");

    printf("Message type %d:\tReceived Server UDP port %s\n", message.type,message.msg);
    
    int UDP_PORT = atoi(message.msg);  //Extracting UDP PORT number

    close(client_TCP_socket);
    printf("------------- TCP connection closed ------------\n");

    //**************UDP port negotiation completed******************

    int client_UDP_socket; 

    // Creating socket file descriptor 
    if ( (client_UDP_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) printError("Client UDP Socket not created");
    else printf("Client UDP socket created successfully\n");

    if(UDP_PORT < 1024 ) printfError("Invalid server UDP port\n");

    //filling server information
    socketAddress server_UDP_addr; 
    memset(&server_UDP_addr, 0, sizeof(server_UDP_addr)); 
    server_UDP_addr.sin_family = AF_INET; 
    server_UDP_addr.sin_port = htons(UDP_PORT); 
    server_UDP_addr.sin_addr.s_addr = inet_addr(argv[1]); 
      
    int server_UDP_addr_len = sizeof(server_UDP_addr); 
    char client_msg[MAX_BUFFER_SIZE];   
    printf("Please input msg to the server\n");
    gets(client_msg);

    /*Sending Message type 3 to Server using UDP*/
    message.type = 3;
    message.len = strlen(client_msg);
    strcpy(message.msg, client_msg);
    sendto(client_UDP_socket, &message, sizeof(message), MSG_CONFIRM, (socketAddressPtr) &server_UDP_addr, sizeof(server_UDP_addr)); 
    printf("Message type %d:\tClient message sent successfully.\n", message.type); 
          
     
    int server_response_len = recvfrom(client_UDP_socket, &message , sizeof(message) , MSG_WAITALL, (socketAddressPtr) &server_UDP_addr, &server_UDP_addr_len); 
    message.msg[message.len]='\0';
    printf("Message type %d:\t Received response from  Server\n", message.type);
    printf("Server response : %s\n\n", message.msg); 
  
    close(client_UDP_socket);
    printf("------------- UDP socket closed ------------\n");
    return 0; 
} 
