// Creating an SMTP server
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<arpa/inet.h>
#include<sys/socket.h>

int main(){
	printf("SERVER: WAKING THE SERVER UP\n");
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

	listen(server_socket, 5);
	printf("SERVER: LISTENING AT PORT 8080\n");

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
	if (client_socket < 0){
		perror("SERVER: FAILED TO CONNECT WITH CLIENT");
		exit(1);
	}

	printf("SERVER: CONNECTED WITH CLIENT\n");
	char welcome[] = "Welcome to AMAL's SMTP server. Use commanda:\n1)HELO\n2)MAIL FROM\n3)RCPT TO\n4)DATA\n5)QUIT\n\n";
	send(client_socket, welcome, strlen(welcome), 0);

	FILE *fp = fopen("mail.txt", "w");
	char ok[] = "OK\n";

	while(1){
		char command[1024];
		int bytes = recv(client_socket, command, sizeof(command)-1, 0);
		command[bytes] = '\0';
		printf("CLIENT: %s", command);

		if (strncmp(command, "HELO", 4) == 0){
			char msg[] = "Hello from Amal Server....\n";
			send(client_socket, msg, strlen(msg), 0);
		}
		else if(strncmp(command, "MAIL FROM", 9) == 0){
			fprintf(fp, "%s\n", command);
			send(client_socket, ok, strlen(ok), 0);
		}
		else if (strncmp(command, "RCPT TO", 7) == 0){
			fprintf(fp, "%s\n", command);
			send(client_socket, ok, strlen(ok), 0);
		}
		else if (strncmp(command, "DATA", 4) == 0){
			char info[] = "Enter the data of the email.\nUse . to complete....\n";
			send(client_socket, info, strlen(info), 0);
			while(1){
				char data[1024];
				int bytes = recv(client_socket, data, sizeof(data)-1, 0);
				data[bytes] = '\0';
				printf("DATA: %s", data);

				if (strncmp(data, ".\n", 2) == 0){
					char info[] = "Data successfully entered...\n";
					send(client_socket, info, strlen(info), 0);
					break;
				}
				fprintf(fp, "%s", data);
			}
		}
		else if (strncmp(command, "QUIT", 4) == 0){
			char quit[] = "DISCONNECTED FROM THE SERVER\n";
			send(client_socket, quit, strlen(quit), 0);
			fclose(fp);
			break;
		}

	}


	close(client_socket);
	close(server_socket);

	return 0;
}