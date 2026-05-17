#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>


int main(){
    printf("SERVER: WAKING UP\n");
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0){
        perror("SERVER: FAILED TO CREATE SOCKET");
        exit(1);
    }

    struct sockaddr_in server_addr;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("SERVER: FAILED TO BIND SOCKET TO THE ADDRESS");
        exit(1);
    }

    listen(server_socket, 5);
    printf("SERVER: LISTENING FOR CONNECTIONS ON PORT 8080\n");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket < 0){
        perror("SERVER: FAILED TO CONNECT TO CLIENT");
        exit(1);
    }

    printf("SERVER: CONNECTED TO CLIENT\n");

    while(1){
        char buffer[1024];
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("CLIENT: %s\n", buffer);

        char reply[1024];
        printf("SERVER:");
        scanf("%s", reply);
        if (strcmp(reply, "QUIT") == 0){
            close(server_socket);
            close(client_socket);
            break;
        }else{
            send(client_socket, reply, sizeof(reply), 0);
        }
    }

    return 0;
}