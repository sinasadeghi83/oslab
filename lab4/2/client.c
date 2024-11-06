#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#define PORT 8088

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    // sets all memory cells to zero
    memset(&serv_addr, '0', sizeof(serv_addr));
    // sets port and address family
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    // connects to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    char clientName[16];
    printf("Enter client name: ");
    fgets(clientName, sizeof(clientName), stdin);
    send(sock, clientName, strlen(clientName), 0);
    printf("Welcome %s!\n", clientName);
    char user_in[1024];
    while (gets(user_in, sizeof(user_in)))
    {
        send(sock, user_in, strlen(user_in), 0);
        char buffer[1024] = {0};
        valread = read(sock, buffer, 1024);
        if (valread < 0)
        {
            perror("read");
            return -1;
        }
        printf("%s", buffer);
        if(strcmp(user_in, "quit") == 0){
            exit(0);
        }
    }
    return 0;
}