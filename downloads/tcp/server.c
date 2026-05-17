#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> // used for close, read, write POSIX sys calls
#include<string.h>
#include<sys/socket.h> // contains socket(), bind, listen, accept, send, recv()
#include<arpa/inet.h> // uses htons()

// WORKFLOW
/*
1)CREATE A SOCKET AND SERVER ADDRESS
2)SET PORT, FAMILY, IP FOR THE SERVER ADDRESS
3)BIND THAT SOCKET WITH THAT ADDRESS
4)LISTEN FOR INCOMMING CONNECTIONS
5)CREATE CLIENT SOCKET AND CLIENT ADDR
6)ACCEPT INCOMMING CONNECTIONS
7)RECEIVE INCOMMING MESSAGE FROM CLIENT TO A SERVER
8)SEND REPLY BACK TO THE CLIENT
9)STOP
*/

int main(){
    printf("SERVER: WAKING UP\n");
    // CREATE A SOCKET WITH IPV4 AND TCP

    // server_socket stores socket file descriptor
    // In linux sockets are treated as files
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0 ){
        perror("SERVER: FAILED TO CREATE A SOCKET");
        exit(1);
    }

    // CREATE A SERVER ADDRESS
    struct sockaddr_in server_addr; // stores information about sockets network
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // host to network short
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //BIND THE SERVER SOCKET WITH SERVER ADDRESS
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        // Type casting is needed bind expects sockaddr*
        perror("SERVER: FAILED TO BIND SOCKET WITH ADDRESS");
        exit(1);
    }

    // LISTEN FOR INCOMMING CONNECTIONS
    listen(server_socket, 5);// max 5 clients waiting in connection queue
    printf("SERVER: LISTENING ON PORT 8080\n");

    //ACCEPT INCOMMING CONNECTIONS
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket < 0){
        perror("SERVER: FAILED TO CONNECT TO CLIENT");
        exit(1);
    }
    printf("SERVER: CONNECTED TO CLIENT\n");
    
    //RECIEVE MESSAGE FROM CLIENT
    char buffer[1024];
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("CLIENT: %s", buffer);

    //SEND MESSAGE TO CLIENT
    char reply[] = "MESSAGE SUCCESSFULLY RECIEVED BY SERVER";
    send(client_socket, reply, strlen(reply), 0);

    //CLOSE THE SOCKETS
    close(client_socket);
    close(server_socket);


    return 0;
}