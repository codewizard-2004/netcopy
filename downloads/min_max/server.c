#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int main(){
    printf("SERVER: STARTING THE SERVER\n");
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("SERVER: FAILED TO BIND THE SERVER");
        exit(1);
    };
    listen(server_socket, 5);
    printf("SERVER: LISTENING AT PORT 8080\n");

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len);
    if (client_socket < 0){
        perror("SERVER: FAILED TO CONNECT WITH CLIENT");
        exit(1);
    }
    printf("SERVER: CONNECTED WITH CLIENT\n");

    char welcome[] = "Welcome to AMAL server....\n";
    send(client_socket, welcome, strlen(welcome), 0);

    int min = 99999;
    int max = -99999;

    int count = 0;
    recv(client_socket, &count, sizeof(count), 0);
    printf("SERVER: GOT COUNT OF NUMBERS AS %d\n", count);

    for(int i = 0; i < count; i++){
        int num = 0;
        recv(client_socket, &num, sizeof(num), 0);
        printf("CLIENT: %d\n", num);

        if (num > max){
            max = num;
        }
        if (num < min){
            min = num;
        }
    }
    char reply[100];
    sprintf(reply, "MIN: %d MAX: %d", min, max);

    printf("\nSERVER: %s\n", reply);
    send(client_socket, reply, strlen(reply), 0);

    close(client_socket);
    close(server_socket);

    return 0;
}