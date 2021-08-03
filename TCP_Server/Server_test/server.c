#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include "ringbuffer_reusable.h"
#include "dataprocessing.h"
extern int errno;
pthread_mutex_t mutex_link;

static void func()
{
	printf("PIPE error occurs!!\r\n");
	return;
}

void *client_processing(void *sock_fd)
{
	int recv,sret;
	fd_set rfds;
	struct timeval timeout = {0,0};
	char temp_buffer[300];
	char p[500];
	ClientInfo *pointer = (ClientInfo *)malloc(sizeof(ClientInfo));
	pointer->Clientfd = *((int *)sock_fd);
	pointer->Recv = BufferInit(p,sizeof(p));
	pointer->State = Recog;
	pointer->Device = Unknown;
	pointer->Bind = NULL;
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(pointer->Clientfd,&rfds);
		sret = select(pointer->Clientfd+1,&rfds,NULL,NULL,&timeout);
		if(sret)
		{
			recv = read(pointer->Clientfd,temp_buffer,sizeof(temp_buffer));
			if(recv > 0)
			{
				BufferWrite(pointer->Recv, temp_buffer, recv);
				if(pointer->Device == Gateway)
					printf("[Gateway]");
				else
					printf("[Client]");
				printf("Recv %d bytes from device %03d\r\n",recv,pointer->Deviceid);
			}
			else
			{
				if(errno == EINTR)
					printf("Do not close socket\r\n");
				else
				{
					pthread_mutex_lock(&mutex_link);
					close(pointer->Clientfd);
					break;
				}
			}
		}
		if(Statemachine(pointer))
		{
			pthread_mutex_lock(&mutex_link);
			close(pointer->Clientfd);
			break;
		}
	}
	printf("connection closed\r\n");
	BufferRelease(pointer->Recv);
	if(pointer->Bind)
		pointer->Bind->Bind = NULL;
	pthread_mutex_unlock(&mutex_link);
	pthread_mutex_lock(&mutex_structure);
	if(pointer->Device == Gateway)DeleteGateway(pointer);
	pthread_mutex_unlock(&mutex_structure);
	free(pointer);
	pointer = NULL;
}	

int main(int argc, char *argv[])
{
	pthread_mutex_init(&mutex_link,NULL);
	pthread_mutex_init(&mutex_structure,NULL);

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
	int clientfd,connectfd;
	int socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	signal(SIGPIPE,func);
	while(1)
	{
		printf("Waiting for connection\r\n");
		connectfd = accept(listenfd, (struct sockaddr *)&clientaddr, (socklen_t *)&socklen);
		if(connectfd <= 0)
		{
			perror("connect");
			printf("connection failed\r\n");
			continue;
		}
		pthread_mutex_lock(&mutex_link);
		clientfd = connectfd;
		pthread_mutex_unlock(&mutex_link);
		printf("Client %s connected\r\n",inet_ntoa(clientaddr.sin_addr));
		pthread_t tid;
		pthread_create(&tid, NULL, client_processing, (void *)(&clientfd));
	}	
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
