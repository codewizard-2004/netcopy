#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<arpa/inet.h>
#include<sys/socket.h>
#include<dirent.h>


int main(){
	// CREATE SERVER SOCKET
	printf("SERVER: STARTING THE SERVER\n");
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	printf("SERVER: SOCKET CREATED\n");
	struct sockaddr_in server_addr;
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	
	bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	printf("SERVER: SOCKET BINDED TO ADDRESS\n");
	
	listen(server_socket, 5);
	printf("SERVER: SERVER RUNNING AT PORT 8080\n");
	
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
	
	printf("SERVER: CONNECTED TO CLIENT\n");
	char msg[] = "\nWelcome to AMAL FTP use commands:\n1)LIST\n2)STOR\n3)RETR\n5)QUIT";
	send(client_socket, msg, sizeof(msg), 0);
	
	
	while(1){
		char command[1024];
		recv(client_socket, command, sizeof(command), 0);
		printf("CLIENT: %s\n", command);
		
		// LIST command
		if (strncmp(command, "LIST", 4) == 0){
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			char buffer[4096] = "";
			
			while((dir = readdir(d)) != NULL){
				strcat(buffer, dir->d_name);
				strcat(buffer, "\n");
			}
			
			send(client_socket, buffer, strlen(buffer), 0);
			closedir(d);
		}
		else if(strncmp(command, "QUIT", 4) == 0){
			printf("SERVER: CLIENT DISCONNECTED\n");
			close(client_socket);
			break;
		}
		else if(strncmp(command, "RETR", 4) == 0){
			char filename[256];
			sscanf(command, "RETR %s", filename);
			FILE *fp = fopen(filename, "r");
			if (fp == NULL){
				send(client_socket, "FILE NOT FOUND", 15,0);
			}else{
				char buffer[1024];
				while (fgets(buffer, 1024, fp) != NULL){
					send(client_socket, buffer, strlen(buffer), 0);
				}
				fclose(fp);
				
			}
		}
		else if(strncmp(command, "STOR", 4) == 0){
			char filename[256];
			sscanf(command, "STOR %s", filename);
			FILE *fp = fopen(filename, "w");
			char rep1[] = "FILE CREATED\nENTER TEXT:\n";
			char rep3[] = "UPLOAD COMPLETE";
			
			send(client_socket, rep1, sizeof(rep1), 0);
			
			char buffer[1024];
			int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
			buffer[bytes] = '\0';
			fprintf(fp, "%s", buffer);
			fclose(fp);
			
			send(client_socket, rep3, sizeof(rep3), 0);
			
		}
	}
	

	return 0;
}
