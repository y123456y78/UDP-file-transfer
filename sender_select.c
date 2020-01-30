#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>

#define buf_size 1024	

struct package {
	int num;
	int length;
	char data[buf_size];
};
int ipCheck(char *ipAddr) {
    struct sockaddr_in tmp;
    int res = inet_pton(AF_INET, ipAddr, &(tmp.sin_addr));
    return res != 0;
}

static void erroHandle(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}


int main(int argc, char **argv)
{
	if (argc!=4) 
	{
		printf("Usage: ./sender_sockopt <filename> <host> <port>.\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in sendAddr, recvAddr;
	struct stat status;
	struct package packet;
	struct timeval timeSet;

	char *fileName;
    char buffer[buf_size];
	char *ipAddr;

	ssize_t addrLen = 0;
    off_t f_size;
    int sockfd;
	FILE *fptr;
	fd_set r_set;
	int select_n;

	memset(&sendAddr, 0, sizeof(sendAddr));
	memset(&recvAddr, 0, sizeof(recvAddr));

	if (ipCheck(argv[2])) 
	{
        ipAddr = argv[2];
    }
    else 
    {
        struct hostent *tmp;
        tmp = gethostbyname(argv[2]);
        ipAddr = inet_ntoa(*((struct in_addr*) tmp->h_addr_list[0]));
    }
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_port = htons(atoi(argv[3]));
	sendAddr.sin_addr.s_addr = inet_addr(ipAddr);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		erroHandle("CLient: socket");


		fileName = argv[1];
	
	
	int totalNum = 0, resend = 0, over_flag = 0 , ackNum=-1 ,drop = 0; ////記得drop
	int i = 0;

	stat(fileName, &status);
	f_size = status.st_size;	//Size of the file

	timeSet.tv_sec = 0;
	timeSet.tv_usec = 100000;

	fptr = fopen(fileName, "rb");	//Open the file to be sent

	if ((f_size % buf_size) != 0)
		totalNum = (f_size / buf_size) + 1;	//Total number of packets to be sent
	else
		totalNum = (f_size / buf_size);

	memset(&packet, 0, sizeof(packet));
	packet.num = -1;
	sprintf(packet.data,"%d",totalNum);
	
	while(ackNum!=totalNum)
	{
		printf("totalNum %d ackNum %d\n", totalNum,ackNum);

		sendto(sockfd, &(packet), sizeof(packet), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr));  //send the packet

		timeSet.tv_sec = 2;
		timeSet.tv_usec = 0;
		FD_ZERO(&r_set);
	    FD_SET(sockfd,&r_set);
	    if((select_n = select(sockfd+1,&r_set,NULL,NULL,&timeSet))>0)
	    {
          			printf("recv something\n");
       	 recvfrom(sockfd, &(ackNum), sizeof(ackNum), 0, (struct sockaddr *) &recvAddr,(socklen_t *) &addrLen);	
	    }
	}
	printf("totalNum %d\n", totalNum);

	/*transmit data packets sequentially followed by an acknowledgement matching*/
	for (i = 1; i <= totalNum; i++)
	{
		memset(&packet, 0, sizeof(packet));
		ackNum = 0;
		packet.num = i;
		packet.length = fread(packet.data, 1, buf_size, fptr);
	
		sendto(sockfd, &(packet), sizeof(packet), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr));  //send the packet

		timeSet.tv_sec = 0;
		timeSet.tv_usec = 100000;
		FD_ZERO(&r_set);
	    FD_SET(sockfd,&r_set);
	    if((select_n = select(sockfd+1,&r_set,NULL,NULL,&timeSet))>0)
	    {
       		recvfrom(sockfd, &(ackNum), sizeof(ackNum), 0, (struct sockaddr *) &recvAddr,(socklen_t *) &addrLen);	
	    }

		while (ackNum != packet.num)
		{
			sendto(sockfd, &(packet), sizeof(packet), 0, (struct sockaddr *) &sendAddr, sizeof(sendAddr));
			timeSet.tv_sec = 0;
			timeSet.tv_usec = 100000;
			FD_ZERO(&r_set);
	   		FD_SET(sockfd,&r_set);
	 	    if((select_n = select(sockfd+1,&r_set,NULL,NULL,&timeSet))>0)
	 	    {
       			recvfrom(sockfd, &(ackNum), sizeof(ackNum), 0, (struct sockaddr *) &recvAddr,(socklen_t *) &addrLen);	
	        }
	          printf("[package.num] ---> %d dropped, %d times\n", packet.num, ++drop);
			resend++;

			/*Enable timeout flag after 200 tries*/
			if (resend == 50 ) 
			{
				if(packet.num == totalNum)
				{
					over_flag = 0;
				}
				else
				{
					over_flag = 1;
				}
					break;
			}
		}
		resend = 0;
		drop = 0;

			/*File transfer fails if timeout occurs*/
		if (over_flag == 1) 
		{
			printf("File not sent\n");
			break;
		}

		if (totalNum == ackNum)
			printf("transport done\n");
	}
		fclose(fptr);
		
		printf("Disable the timeout\n");
		timeSet.tv_sec = 0;
	
		
		
	close(sockfd);

	exit(EXIT_SUCCESS);
}
