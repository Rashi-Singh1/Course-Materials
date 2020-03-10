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
    // exit(0);
}

int main(int argc, char const *argv[])
{
    //creation of client socket
    int client_socket = -1;
    client_socket = socket(AF_INET,SOCK_STREAM,0);
    if(client_socket < 0) printError("Client Socket not created");
    else printf("Client socket created successfully\n");

    socketAddress server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(S_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int connection_status = -1;
    connection_status = connect(client_socket,(struct sockaddr* )&server_addr,sizeof(server_addr));
    if(connection_status < 0) printError("Connection error");
    else printf("TCP Connection created successfully\n");

    char* request_msg = "UDP port requested";
    int send_status;
    if(send_status = write(client_socket,request_msg,strlen(request_msg)) < 0) printError("Client request send error");
    else printf("UDP port request sent\n");

    char UDP_PORT_str[1024] = {0};
    int valread;
    if(valread = read(client_socket,UDP_PORT_str,1024) < 0) printError("Server response read error");
    else printf("Received port number : %s\n",UDP_PORT_str);
    // recv(client_socket,UDP_PORT_str,sizeof(UDP_PORT_str),0);
    // printf("Received port number : %s\n",UDP_PORT_str);
    // close(client_socket);
    return 0;
}
