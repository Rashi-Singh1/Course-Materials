#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#define S_PORT 9002
typedef struct sockaddr_in socketAddress;

void printError(const char* msg)
{
    perror(msg);
    // exit(0);
}

int main(int argc, char const *argv[])
{
    //creation of client socket
    int server_socket = 0;
    server_socket = socket(AF_INET,SOCK_STREAM,0);
    if(server_socket < 0) printError("Server Socket not created\n");
    else printf("Welcome socket created successfully\n");

    socketAddress server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(S_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    int bind_status = 0;
    bind_status = bind(server_socket,(struct sockaddr* )&server_addr,sizeof(server_addr));
    if(bind_status < 0) printError("Welcome socket bind failure\n");
    else printf("Welcome socket bind success\n");

    listen(server_socket,5);
    // int connection_status = connect(server_socket,(struct sockaddr* )&server_addr,sizeof(server_addr));
    // if(connection_status < 0) printf("Connection error");

    // char* request_msg = "UDP port requested";
    // send(server_socket,request_msg,strlen(request_msg),0);
    int connection_socket;
    if(connection_socket = accept(server_socket,(struct sockaddr* )&server_addr,(socklen_t* )sizeof(server_addr)) < 0) printf("Request not accepted\n");
    return 0;
}
