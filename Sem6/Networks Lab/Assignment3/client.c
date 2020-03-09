#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define S_PORT 3601
typedef struct sockaddr_in socketAddress;

int main(int argc, char const *argv[])
{
    //creation of client socket
    int client_socket = 0;
    client_socket = socket(AF_INET,SOCK_STREAM,0);
    if(client_socket < 0) printf("Client Socket not created\n");

    socketAddress server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(S_PORT);

    int connection_status = connect(client_socket,(struct sockaddr* )&server_addr,sizeof(server_addr));
    if(connection_status < 0) printf("Connection error");

    char* request_msg = "UDP port requested";
    send(client_socket,request_msg,strlen(request_msg),0);

    return 0;
}
