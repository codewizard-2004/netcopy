#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
// #include<time.h>

typedef struct{
    char data[100];
}Packet;

typedef struct{
    int frame_kind;
    int seq_no;
    int ack;
    Packet packet;
}Frame;

int main(){
    // srand(time(0));
    printf("SERVER: WAKING UP THE SERVER\n");
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("SERVER: FAILED TO BIND SERVER");
        exit(1);
    }
    printf("SERVER: SERVER RUNNING ON PORT 8080\n");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int frame_id = 0;
    Frame frame_send;
    Frame frame_recv;

    while(1){
        int data = recvfrom(server_socket, &frame_recv, sizeof(Frame), 0, (struct sockaddr*)&client_addr, &client_len);
        if (data > 0 && frame_recv.frame_kind == 1 && frame_recv.seq_no == frame_id){
            printf("SERVER: FRAME RECEIVED: %s", frame_recv.packet.data);
            int drop = rand() % 3;
            if (drop == 0){
                printf("SERVER: FRAME LOST\n");
                continue;
            }
            frame_send.seq_no = 0;
            frame_send.frame_kind = 0;
            frame_send.ack = frame_recv.seq_no + 1;

            drop = rand()%3;
            if(drop == 0){
                printf("SERVER: ACKNOWLEDGEMENT LOST\n");
                continue;
            }
            sendto(server_socket, &frame_send, sizeof(Frame), 0, (struct sockaddr*)&client_addr, client_len);
            printf("SERVER: ACKNOWLEDGEMENT SENT\n");
            frame_id++;
        }else{
            printf("SERVER: FRAME NOT RECEIVED!!!!\n");
        }
    }

    close(server_socket);

}