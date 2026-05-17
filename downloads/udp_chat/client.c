#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int main(){
    printf("CLIENT: WAKING UP THE CLIENT\n");
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0){
        perror("CLIENT: FAILED TO CREATE SOCKET");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t server_len = sizeof(server_addr);
    struct sockaddr *server = (struct sockaddr*)&server_addr;

    while(1){
        char buffer[1024];
        printf("CLIENT: ");
        fgets(buffer, sizeof(buffer), stdin);
        sendto(client_socket, buffer, strlen(buffer), 0, server, server_len);

        char reply[1024];
        int bytes = recvfrom(client_socket, reply, sizeof(reply)-1, 0, server, &server_len);
        reply[bytes] = '\0';

        printf("SERVER: %s", reply);
    }


    close(client_socket);
    return 0;
}