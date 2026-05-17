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

    int count;
    printf("SERVER: Enter the count of numbers:");
    scanf("%d", &count);
    send(client_socket, &count, sizeof(count), 0);

    printf("SERVER: ENTER THE NUMBERS\n");
    for(int i = 0; i < count; i++){
        int num = 0;
        printf("CLIENT:");
        scanf("%d", &num);

        send(client_socket, &num, sizeof(num), 0);
    }

    char result[100];
    int bytes2 = recv(client_socket, result, sizeof(result), 0);
    result[bytes2] = '\0';
    printf("SERVER: %s", result);

    close(client_socket);
    return 0;
}