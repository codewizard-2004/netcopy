#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<string.h>

typedef struct {
    char data[100];
}Packet;

typedef struct {
    int frame_kind;
    int seq_no;
    int ack;
    Packet packet;
}Frame;

int main(){
    printf("CLIENT: LOADING THE CLIENT\n");
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t server_len = sizeof(server_addr);

    Frame frame_send;
    Frame frame_recv;
    int frame_id = 0;

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    printf("CLIENT: STARTING TRANSMISSION\n");

    while(1){
        printf("CLIENT: ENTER DATA:");
        fgets(frame_send.packet.data, 100, stdin);

        if(strncmp(frame_send.packet.data, "EXIT", 4) == 0){
            break;
        }

        frame_send.frame_kind = 1;
        frame_send.seq_no = frame_id;
        frame_send.ack = 0;

        while(1){
            printf("CLIENT: SENDING FRAME %d\n", frame_id);

            sendto(client_socket,
                &frame_send,
                sizeof(Frame), 0,
                (struct sockaddr*)&server_addr,
                server_len   
                );
            
            int data = recvfrom(client_socket,
                &frame_recv,
                sizeof(Frame), 0,
                (struct sockaddr*)&server_addr,
                &server_len
            );

            if (data > 0 && frame_recv.frame_kind == 0 && frame_recv.ack == frame_id + 1){
                printf("CLIENT: ACKNOWLEDGEMENT RECIEVED FOR %d\n", frame_id);
                frame_id++;
                break;
            }else{
                printf("CLIENT: TIMEOUT... RESENDING FRAME %d\n", frame_id);
            }
        }
    }

    close(client_socket);

    return 0;
}