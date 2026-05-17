#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>


int main(){
    printf("CLIENT: STARTING\n");
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0){
        perror("CLIENT: FAILED TO CREATE A SOCKET");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("CLIENT: FAILED TO CONNECT TO SERVER");
        exit(1);
    }

    printf("CLIENT: CONNECTED TO SERVER AT PORT 8080\n");
    while(1){
        char message[1024];
        printf("CLIENT:");
        scanf("%s", message);

        send(client_socket, message, sizeof(message), 0);
        
        char reply[1024];
        recv(client_socket, reply, sizeof(reply), 0);
        if (strcmp(reply, "QUIT") == 0){
            close(client_socket);
            break;
        }else{
            printf("SERVER: %s\n", reply);
        }
    }

    return 0;
}