#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>

/*
1) CREATE FRAME STRCUTRE
2) INITIALIZE SEND AND RECV FRAMES
3) INITIALIZE base AND received[MAX]
4) WHILE BASE < MAX: RECEV THE FRAME
5) CHECK IF THE FRAME IS IN ORDER
    5.1) IF YES TRY DROP FRAME AND DROP ACK
    5.2) IF PASSED SEND ACK FRAME AND UPDATE RECEIVED[]
    5.3) INCREMENT BASE WHILE RECEIVED[BASE] IS TRUE
6)IF NOT IN ORDER DROP THE FRAME
*/

#define MAX_FRAMES 10
#define WINDOW 4

typedef struct{
    int frame_kind;
    int seq;
    int ack;
}Frame;

int main(){
    srand(time(0));
    printf("SERVER: WAKING UP THE SERVER\n");
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("SERVER: FAILED TO BIND SERVER...");
        exit(1);
    }

    printf("SERVER: SERVER RUNNING AT PORT 8080\n");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    Frame frame_send;
    Frame frame_recv;

    int base = 0;
    int received[MAX_FRAMES];
    memset(received, 0, sizeof(received));

    while(base < MAX_FRAMES){
        recvfrom(server_socket, &frame_recv, sizeof(Frame), 0, (struct sockaddr*)&client_addr, &client_len);
        if (frame_recv.frame_kind == 1 && frame_recv.seq >= base && frame_recv.seq < base + WINDOW){
            printf("SERVER: FRAME RECEIVED %d\n", frame_recv.seq);
            int drop = rand()%4;
            if (drop == 0){
                printf("SERVER: FRAME %d LOST\n", frame_recv.seq);
                continue;
            }

            drop = rand()%4;
            if(drop == 0){
                printf("SERVER: ACK FOR FRAME %d LOST\n", frame_recv.seq);
                continue;
            }

            received[frame_recv.seq] = 1;

            frame_send.frame_kind = 0;
            frame_send.seq = 0;
            frame_send.ack = frame_recv.seq;

            sendto(server_socket,&frame_send,sizeof(Frame),0,
                   (struct sockaddr*)&client_addr,client_len);
            printf("SERVER: ACK SENT FOR FRAME %d\n", frame_recv.seq);

            while(received[base]){
                base++;
            }
        }else{
            printf("SERVER: FRAME %d OUT OF ORDER\n", frame_recv.seq);
        }

    }


}