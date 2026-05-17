#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>


// WORKFLOW
/*
1) CREATE A CLIENT SOCKET USING SOCKET()
2) CREATE THE SERVER SOCKET ADDRESS
3) CONNECT THE CLIENT SOCKET WITH THE SERVER ADDRESS USING CONNECT()
4) SEND MESSAGE TO THE SERVER
5) RECIEVE MESSAGE FROM THE SERVER
6) CLOSE THE SOCKETS
*/

int main(){
    printf("CLIENT: STARTING\n");
    
    // CREATE THE CLIENT SOCKET
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0){
        perror("CLIENT: FAILED TO CREATE SCOKET");
    }

    // CREATE THE SERVER SOCKET ADDRESS
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // CONNECT THE CLIENT SOCKET WITH SERVER SOCKET ADDRESS
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("CLIENT: FAILED TO CONNECT TO THE SERVER");
    }

    printf("CLIENT: CONNECTED TO THE SERVER\n");
    char message[] = "Welcome to socket programming";

    // SEND MESSAGE TO SERVER
    send(client_socket, message, strlen(message), 0);

    // RECIEVE MESSAGE FROM SERVER
    char buffer[1024];
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("SERVER: %s\n", buffer);

    close(client_socket);

    return 0;

}