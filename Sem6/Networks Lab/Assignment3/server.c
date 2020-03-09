#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    int welcomeSocket;
    welcomeSocket = socket(AF_INET,SOCK_STREAM,0);
    
    return 0;
}
