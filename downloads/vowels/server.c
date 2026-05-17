#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int checkVowel(char c){
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U'){
        return 1;
    }
    return 0;
}

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

    char buffer[1024];
    int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
    buffer[bytes] = '\0';

    int vowels = 0;

    //printf("CLIENT: %s\n", buffer);
    for (int i = 0; i < strlen(buffer); i++){
        vowels += checkVowel(buffer[i]);
    }

    char reply[126];
    sprintf(reply, "Number of vowels is %d", vowels);
    printf("\nSERVER: %s\n", reply);

    send(client_socket, reply, strlen(reply), 0);


    return 0;
}