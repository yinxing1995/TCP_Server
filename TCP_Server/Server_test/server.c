#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include "ringbuffer_reusable.h"

extern int errno;

static void func(int arg)
{}

void *client_processing(void *sock_fd)
{
	int clientfd = *((int *)sock_fd);
	int recv,sret;
	fd_set rfds;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	char temp_buffer[300];
	char p[500];
	Ringbuf *ringbuf = BufferInit(p,sizeof(p));
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(clientfd,&rfds);
		sret = select(clientfd+1,&rfds,NULL,NULL,&timeout);
		if(sret)
		{
			//memset(buffer,0,sizeof(buffer));
			recv = read(clientfd,temp_buffer,sizeof(temp_buffer));
			if(recv > 0)
			{
				//printf("buffer:%s\r\n",buffer);
				BufferWrite(ringbuf, temp_buffer, recv);
				printf("Recv %d bytes",recv);
			}
			else
			{
				if(errno == EINTR)
					printf("Do not close socket\r\n");
				else
				{

					close(clientfd);
					printf("connection closed\r\n");
					break;
				}
			}
		}
	}
	BufferRelease(ringbuf);
}	

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Using:./server port\r\nExample:./server 5005\r\n");
		return -1;
	}

	//step 1. Creat socket
	int listenfd;
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		return -1;
	}

	//step 2. Bind server to socket
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	//servaddr.sin_addr.s_addr = inet_addr("130.61.250.117");
	servaddr.sin_port = htons(atoi(argv[1]));
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		perror("bind");
		close(listenfd);
		return -1;
	}

	//step 3. Set "listenfd" as monitor
	if((listen(listenfd, 5) != 0))
	{
		perror("listen");
		close(listenfd);
		return -1;
	}

	//step 4. Receive connection from client
	int clientfd;
	int socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	while(1)
	{
		printf("Waiting for connection\r\n");
		clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, (socklen_t *)&socklen);
		printf("Client %s connected\r\n",inet_ntoa(clientaddr.sin_addr));
		pthread_t tid;
		pthread_create(&tid, NULL, client_processing, (void *)(&clientfd));
	}	
	signal(SIGPIPE,func);
	/*
	char buffer[1024];

	//step 5. Communicate with client

	while(1)
	{
		int iret;
		memset(buffer, 0, sizeof(buffer));
		
		if((iret = recv(clientfd, buffer ,sizeof(buffer),0) <= 0))
		{
			cout << "iret = " << iret << endl;
			break;
		}
		else
		{
			cout << "recv:" << buffer << endl;
		}
		
		cout << "Try to send" << endl;
		if((iret = send(clientfd, "I am server" ,strlen("I am server"),0) <= 0))
		{
			cerr << errno << endl;
			perror("send");
		}
		sleep(2);
	}*/

	//step 6. Close socket
	printf("Program exit!\r\n");
	close(listenfd);
	//close(clientfd);
	return 0;
}
