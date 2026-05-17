#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int checkPrime(int num){
    for (int i = 2; i < num/2; i++){
        if (num % i == 0){
            return 0;
        }
    }
    return 1;
}

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

    char prime[] = "IT IS A PRIME NUMBER...\n";
    char noprime[] = "IT IS NOT A PRIME NUMBER...\n";

    while(1){
        int num = 0;
        recvfrom(server_socket, &num, sizeof(num), 0, client, &client_len);
        printf("CLIENT: %d\n", num);

        if(checkPrime(num)){
            printf("SERVER: %s", prime);
            sendto(server_socket, prime, strlen(prime), 0, client, client_len);
        }else{
            printf("SERVER: %s", noprime);
            sendto(server_socket, noprime, strlen(noprime), 0, client, client_len);
        }


    }
    close(server_socket);

    return 0;
}