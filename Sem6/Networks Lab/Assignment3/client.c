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
    int client_socket = 0;
    client_socket = socket(AF_INET,SOCK_STREAM,0);
    if(client_socket < 0) printError("Client Socket not created\n");
    else printf("Client socket created successfully\n");

    socketAddress server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(S_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(client_socket,(struct sockaddr* )&server_addr,sizeof(server_addr));
    if(connection_status < 0) printError("Connection error\n");
    else printf("TCP Connection created successfully\n");

    char* request_msg = "UDP port requested";
    send(client_socket,request_msg,strlen(request_msg),0);

    // close(client_socket);
    return 0;
}
