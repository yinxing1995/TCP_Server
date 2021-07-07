#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

using namespace std;
extern int errno;
int net_lfd = 0,net_cfd = 0;
void func(int arg)
{
	close(net_lfd);
	close(net_cfd);
	printf("closed\r\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		cout << "Using:./server port\nExample:./server 5005\n\n" << endl;
		return -1;
	}

	//step 1. Creat socket
	int listenfd;
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		cerr << errno << endl;
		perror("socket");
		return -1;
	}
	net_lfd = listenfd;

	//step 2. Bind server to socket
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	//servaddr.sin_addr.s_addr = inet_addr("130.61.250.117");
	servaddr.sin_port = htons(atoi(argv[1]));
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		cerr << errno << endl;
		perror("bind");
		close(listenfd);
		return -1;
	}

	//step 3. Set "listenfd" as monitor
	if((listen(listenfd, 5) != 0))
	{
		cerr << errno << endl;
		perror("listen");
		close(listenfd);
		return -1;
	}

	//step 4. Receive connection from client
	int clientfd;
	int socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	cout << "Waiting for Connection\n" << endl;
	clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, (socklen_t *)&socklen);
	net_cfd = clientfd;
	cout << "Client" << inet_ntoa(clientaddr.sin_addr) << " connected.\n" << endl;
	char buffer[1024];

	//step 5. Communicate with client
	signal(SIGINT,func);
	while(1)
	{
		int iret;
		memset(buffer, 0, sizeof(buffer));
		/*
		if((iret = recv(clientfd, buffer ,sizeof(buffer),0) <= 0))
		{
			cout << "iret = " << iret << endl;
			break;
		}
		else
		{
			cout << "recv:" << buffer << endl;
		}
		*/
		cout << "Try to send" << endl;
		if((iret = send(clientfd, "I am server" ,strlen("I am server"),0) <= 0))
		{
			cerr << errno << endl;
			perror("send");
		}
		sleep(2);
	}

	//step 6. Close socket
	cout << "Program exits" << endl;
	close(listenfd);
	close(clientfd);
	return 0;
}
