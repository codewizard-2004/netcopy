#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>

#define MAX_FRAME 10
#define WINDOW_SIZE 4

typedef struct{
    int frame_type;
    int seq;
    int ack;
}Frame;

int main(){
    printf("CLIENT: WAKING UP THE CLIENT\n");
    int client_socket = socket(AF_INET, SOCK_DGRAM,0);

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t server_len = sizeof(server_addr);

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    Frame frame_send;
    Frame frame_recv;

    int base = 0;
    int next_seq = 0;

    while(base < MAX_FRAME){
        while(next_seq < base + WINDOW_SIZE && next_seq < MAX_FRAME){
            frame_send.frame_type = 1;
            frame_send.seq = next_seq;
            printf("CLIENT: SENDING FRAME %d\n", next_seq);

            sendto(client_socket, &frame_send, sizeof(Frame), 0,
                    (struct sockaddr*)&server_addr, server_len
            );
            next_seq++;
        }
        int data = recvfrom(client_socket,&frame_recv,sizeof(Frame),0,
                            (struct sockaddr*)&server_addr,&server_len);
        
        if (data > 0){
            printf("CLIENT: ACK RECEIVED FOR %d\n", frame_recv.ack);
            base = frame_recv.ack;
        }else{
            printf("CLIENT: TIMEOUT → Resending from frame %d\n",base);
            next_seq = base;
        }
    }
    printf("CLIENT: ALL FRAMES SENT SUCCESSFULLY\n");

    close(client_socket);

    return 0;
}