#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#define S_PORT 9002
typedef struct sockaddr_in socketAddress;

void printError(const char* msg)
{
    perror(msg);
}

int main(int argc, char const *argv[])
{
    //creation of client socket
    int server_socket = -1;
    server_socket = socket(AF_INET,SOCK_STREAM,0);
    if(server_socket < 0) printError("Server Socket not created");
    else printf("Welcome socket created successfully\n");

    socketAddress server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(S_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    int bind_status = -1;
    bind_status = bind(server_socket,(struct sockaddr* )&server_addr,sizeof(server_addr));
    if(bind_status < 0) printError("Welcome socket bind failure");
    else printf("Welcome socket bind success\n");

    listen(server_socket,5);

    int connection_socket;
    socklen_t client_add_size;
    socketAddress client_address;
    if(connection_socket = accept(server_socket,(struct sockaddr* )&client_address,&client_add_size) < 0) printError("Request not accepted");
    else printf("Connection Socket created successfully\n");

    printf("Waiting for client request\n");

    char client_request[1024] = {0};
    int io_buffer = -1;
    // if(io_buffer = recv(connection_socket,client_request,sizeof(client_request),0) < 0) printError("Error in recv");
    if((io_buffer = read(connection_socket,client_request,1024)) < 0) printError("Error in recv");
    else printf("Server received request : %s\n",client_request);

    int UDP_PORT;
    char UDP_PORT_str[1024];
    printf("Please input port number\n");
    scanf("%d",&UDP_PORT);

    sprintf(UDP_PORT_str,"%d",UDP_PORT);
    if(io_buffer = write(connection_socket,UDP_PORT_str,strlen(UDP_PORT_str)) < 0) printError("Error in send");
    else printf("UDP_PORT sent by server\n");
    
    // close(connection_socket);
    return 0;
}
