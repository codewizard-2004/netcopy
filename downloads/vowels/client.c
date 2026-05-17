#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int main(){
    printf("CLIENT: STARTING THE CLIENT\n");
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("CLIENT: FAILED TO CONNECT WITH SERVER");
        exit(1);
    };
    printf("CLIENT: CONNECTED WITH THE SERVER\n");

    char buffer[1024];
    int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
    buffer[bytes] = '\0';

    printf("SERVER: %s", buffer);

    printf("CLIENT: ENTER THE STRING:\n");
    fgets(buffer, sizeof(buffer), stdin);

    send(client_socket, buffer, sizeof(buffer), 0);

    recv(client_socket, buffer, strlen(buffer)-1,0);
    printf("SERVER: %s\n", buffer);


    return 0;

}