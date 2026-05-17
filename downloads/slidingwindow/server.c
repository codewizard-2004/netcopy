// we will get one frame 
// verify if the frame is the expected frame
// if yes try to send ack
// if no discard the frame
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define MAX_FRAMES 10

typedef struct{
    int frame_type;
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
        perror("SERVER: FAILED TO BIND SERVER");
        exit(1);
    }
    printf("SERVER: RUNNING AT PORT 8080!!!\n");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    Frame frame_recv;
    Frame frame_send;
    int expected_frame = 0;

    while(expected_frame < MAX_FRAMES){
        int bytes = recvfrom(
            server_socket,
            &frame_recv,
            sizeof(Frame), 0,
            (struct sockaddr*)&client_addr,
            &client_len
        );

        if(bytes > 0 && frame_recv.frame_type == 1){
            //THis means we received the frame and it isdata frame
            printf("SERVER: FRAME RECEIVED: %d\n", frame_recv.seq);

            int drop = rand()%4;
            if (drop == 0){
                printf("SERVER: FRAME %d LOST\n", frame_recv.seq);
                continue;
            }

            if (frame_recv.seq == expected_frame){
                printf("SERVER: FRAME %d RECEIVED CORRECTLY\n", frame_recv.seq);

                drop = rand()%4;
                if (drop == 4){
                    printf("SERVER: ACKNOWLEDGEMENT LOST FOR FRAME %d\n", frame_recv.seq);
                    continue;
                }
                
                expected_frame++;

                frame_send.frame_type = 0;
                frame_send.ack = expected_frame;
                frame_send.seq = 0;

                sendto(
                    server_socket,
                    &frame_send,
                    sizeof(Frame),0,
                    (struct sockaddr*)&client_addr,
                    client_len
                );

                printf("SERVER: ACK SENT FOR %d\n", expected_frame);

                
            }
        }
    }

    close(server_socket);


}