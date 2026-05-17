#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int main(){
    printf("SERVER: STARTING THE SERVER\n");
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_socket < 0){
        perror("SERVER: FAILED TO CREATE SOCKET");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("SERVER: FAILED TO BIND SERVER TO ADDRESS");
        exit(1);

    }

    printf("SERVER: LISTENING ON PORT 8080\n");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct sockaddr *client = (struct sockaddr*)&client_addr;

    while(1){
        char buffer[1024];
        int bytes = recvfrom(server_socket, buffer, sizeof(buffer)-1, 0, client, &client_len);
        buffer[bytes] = '\0';
        printf("CLIENT: %s\n", buffer);

        char reply[1024];
        printf("SERVER:");
        fgets(reply, sizeof(reply), stdin);
        sendto(server_socket, reply, strlen(reply), 0, client, client_len);
    }

    close(server_socket);
    
    return 0;
    }