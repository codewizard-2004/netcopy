#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<string.h>

/*
1) INITIALIZE THE FRAME STRUCTURE, SEND AND RECV FRAMES
2) INIT BASE=0 AND NEX_SEQ = 0, ACKED[]
3) INIT TIMEVAL
4) WHILE BASE < MAX:
    4.1) SEND ALL THE FRAMES IN WINDOW (NEXT_SEQ < BASE + WINDOW && NEXT_SEQ < MAX)
    4.2) INCREMENT NEXT_SEQ
    4.3) WAIT FOR DATA TO BE RECVED
    4.4) IF RECVED INCR BASE WHILE ACKED[BASE] IS TRUE
    4.5) IF NOT RECVED, THEN RETRANSMITT
*/

#define MAX_FRAMES 10
#define WINDOW_SIZE 4

typedef struct{
    int frame_kind;
    int seq;
    int ack;
}Frame;

int main(){
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t server_len = sizeof(server_addr);

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    int base = 0;
    int next_seq = 0;
    int acked[MAX_FRAMES];
    memset(acked, 0, sizeof(acked));

    Frame frame_send;
    Frame frame_recv;

    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    printf("CLIENT: SELECTIVE REPEAT STARTED\n");

    while(base < MAX_FRAMES){
        // send the entire window of frames
        while(next_seq < base + WINDOW_SIZE && next_seq < MAX_FRAMES){
            frame_send.frame_kind = 1;
            frame_send.seq = next_seq;

            printf("CLIENT: SENDING FRAME %d\n", next_seq);
            sendto(client_socket,&frame_send,sizeof(Frame),0,
                   (struct sockaddr*)&server_addr,server_len);
            next_seq++;
        }

        // wait for the ack 
        int data = recvfrom(client_socket,&frame_recv,sizeof(Frame),0,
                            (struct sockaddr*)&server_addr,&server_len);
        
        if (data > 0){
            int ack = frame_recv.ack;
            printf("CLIENT: ACK FOR FRAME %d\n", ack);
            acked[ack] = 1;

            // move to next base
            while(acked[base] && base < MAX_FRAMES){
                base++;
            }
        }else{
            printf("CLIENT: TIMEOUT!!! RESENDING UNACKED FRAMES\n");
            for(int i = base; i < next_seq; i++){
                if(!acked[i]){
                    frame_send.frame_kind = 1;
                    frame_send.seq = i;
                    printf("CLIENT: RESENDING FRAME %d\n", i);

                    sendto(client_socket,&frame_send,sizeof(Frame),0,
                   (struct sockaddr*)&server_addr,server_len);
                }
            }
        }
    }

    return 0;
}