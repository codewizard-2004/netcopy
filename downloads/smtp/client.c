#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

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
	}
	printf("CLIENT: CONNECTED TO SERVER\n");
	char welcome[1024];
	int bytes = recv(client_socket, welcome, sizeof(welcome) - 1, 0);
	welcome[bytes] = '\0';
	printf("SERVER: %s", welcome);

	while(1){
		char command[1024];
		printf("CLIENT:");
		fgets(command, sizeof(command), stdin);
		send(client_socket, command, strlen(command), 0);

		char reply[1024];
		int bytes = recv(client_socket, reply, sizeof(reply) - 1, 0);
		reply[bytes] = '\0';
		printf("SERVER: %s\n", reply);

		if (strncmp(command, "QUIT", 4) == 0){
			break;
		}
		if (strncmp(command, "DATA", 4) == 0){
			printf("\n");
			while(1){
				char data[1024];
				fgets(data, sizeof(data), stdin);
				send(client_socket, data, strlen(data), 0);

				if (strncmp(data, ".\n", 2) == 0){
					break;
				}
			}
		}
	}


	close(client_socket);
	return 0;
}