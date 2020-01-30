#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>

#define buf_size 1024
static void sigAlrm(int sig)
{
    return;
}
struct package {
    int num;
    int length;
    char data[buf_size];
};

static void erroHandle(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./receiver_sockopt [fileName] [bind port]\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in recvAddr, sendAddr;
    struct package packet;
    struct timeval timeSet;

    struct sigaction act; 
    act.sa_handler = sigAlrm; 
    act.sa_flags = 0; 

    char buf[buf_size];
    char *fileName;

    ssize_t addrLen;
    int sockfd;

    FILE *fptr;

    sigaction(SIGALRM, &act, NULL);


    /* clear sockaddr_in */
    memset(&recvAddr,0,sizeof(recvAddr));

    /* set receiver info */
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvAddr.sin_port = htons(atoi(argv[2]));

    /* set fileName */
    fileName = argv[1];
    /* socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        erroHandle("socket() error");

    /* bind */
    if (bind(sockfd, (const struct sockaddr *) &recvAddr, sizeof(recvAddr)) < 0)
        erroHandle("bind() error");

    for(;;) {
        printf("[Receiver] Waiting for connection\n");

        memset(buf, 0, sizeof(buf));

        addrLen = sizeof(sendAddr);

        int totalNum = -1, i = 0;

        // Enable timeout
        ualarm(100000,0);
        // Receive packet num
        memset(&packet, 0, sizeof(packet));
        recvfrom(sockfd, &(packet), sizeof(packet), 0, (struct sockaddr *) &sendAddr, (socklen_t *) &addrLen);
        totalNum = atoi(packet.data);
        
        ualarm(0,0);


        if(totalNum > 0) {
            // Resend packet num as ACK
            printf("totalNum %d\n", totalNum);
            sendto(sockfd, &(totalNum), sizeof(totalNum), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr)); 
            fptr = fopen(fileName, "w");

            for(i = 1; i <= totalNum; i++) {
                memset(&packet, 0, sizeof(packet));

                // receive packet
                recvfrom(sockfd, &(packet), sizeof(packet), 0, (struct sockaddr *) &sendAddr, (socklen_t *) &addrLen);
                // resend ack number
                sendto(sockfd, &(packet.num), sizeof(packet.num), 0, (struct sockaddr*) &sendAddr, sizeof(sendAddr));
                printf("Receive packet num: %d\n", packet.num);
                if(packet.num != i) {
                    i--;
                    if(packet.num == -1)
                    {
                         sendto(sockfd, &(totalNum), sizeof(totalNum), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr)); 
                    }
                }
                else {
                    fwrite(packet.data, sizeof(char), packet.length, fptr);
                    printf("Packet ID: %d. Packet length: %d\n", packet.num, packet.length);
                    
                }

                if(i == totalNum)
                    printf("File received\n");
            }
            //printf("Total bytes received: %ld\n", bytes_recv);
            fclose(fptr);
            break;
        }
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}
