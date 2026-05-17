#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

int main(){

    printf("CLIENT: STARTING\n");

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(client_socket < 0){
        perror("CLIENT: SOCKET CREATION FAILED");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
        perror("CLIENT: CONNECTION FAILED");
        exit(1);
    }

    printf("CLIENT: CONNECTED TO SERVER\n");

    /* Receive welcome message */

    char buffer[4096];

    int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
    buffer[bytes] = '\0';

    printf("%s\n",buffer);

    while(1){

        char command[1024];

        printf("ftp> ");

        fgets(command,sizeof(command),stdin);

        command[strcspn(command,"\n")] = 0;

        send(client_socket, command, strlen(command), 0);

        if(strncmp(command,"QUIT",4) == 0){
            printf("CLIENT: DISCONNECTED\n");
            break;
        }

        bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
        buffer[bytes] = '\0';

        printf("%s\n",buffer);
    }

    close(client_socket);

    return 0;
}
