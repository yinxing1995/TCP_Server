#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include "dataprocessing.h"

#define GATEWAYFLAG "Gateway:"
#define MONITORFLAG "Monitor:"
#define BIND "Bind:"
#define LEN_ID 3

Gatewaylist *G_head = NULL;

static void BindDevice(ClientInfo *pointer, int Gatewayid)
{
	Gatewaylist *current = G_head;
	if(!current)
	{
		printf("No Gateway\r\n");
		return;
	}
	while(current->Gateway->Deviceid != Gatewayid)
	{
		current = current->next;
		if(!current)
		{
			printf("ID not found\r\n");
			return;
		}
	}
	current->Gateway->Bind = pointer;
	pointer->Bind = current->Gateway;
	printf("Monitor %d binded to Gateway %d\r\n",current->Gateway->Bind->Deviceid,pointer->Bind->Deviceid);
}

int Statemachine(ClientInfo *pointer)
{
	switch(pointer->State)
	{
		case Recog:
			;
			char tbuf[20];
			memset(tbuf,0,sizeof(tbuf));
			if(!BufferSeek(pointer->Recv,tbuf,strlen(GATEWAYFLAG)))				
			{
				if(!strcmp(GATEWAYFLAG,tbuf))
				{
					printf("Gateway connected\r\n");
					pointer->Device = Gateway;
					pointer->State = Init;
					BufferRead(pointer->Recv,tbuf,strlen(GATEWAYFLAG));		
					break;
				}
			}
			if(!BufferSeek(pointer->Recv,tbuf,strlen(MONITORFLAG)))
			{
				if(!strcmp(MONITORFLAG,tbuf))
				{
					printf("Monitor connected\r\n");
					pointer->Device = Monitor;
					pointer->State = Init;
					BufferRead(pointer->Recv,tbuf,strlen(GATEWAYFLAG));
					break;
				}
				else
				{
					printf("Recognization failed\r\n");
					return -1;
				}
			}
			else
			{
				//printf("Buffer not ready\r\n");
				if(0)
				{
					printf("Overtime\r\n");
					return -1;
				}
			}
			break;
		case Init:
			memset(tbuf,'\0',sizeof(tbuf));
			if(BufferRead(pointer->Recv,tbuf,LEN_ID)<0)
				break;
			pointer->Deviceid = atoi(tbuf);
			//printf("buf = %s\r\n",tbuf);
			//printf("ID = %d\r\n",pointer->Deviceid);
			
			if(pointer->Device == Monitor)
			{
				pointer->State = Bind;
				printf("State has been set to Bind\r\n");
				break;
			}
			else//Gateway
			{
				pthread_mutex_lock(&mutex_structure);
				AddGateway(pointer);
				pthread_mutex_unlock(&mutex_structure);
				pointer->State = Datapro;
				break;
			}
		case Bind:
			memset(tbuf,'\0',sizeof(tbuf));
			if(BufferRead(pointer->Recv,tbuf,strlen(BIND))<0)
				break;
			//printf("tbuf = %s\r\n",tbuf);
			if(!strcmp(BIND,tbuf))
			{

				if(BufferRead(pointer->Recv,tbuf,LEN_ID)<0)
					break;
				int Bindid = atoi(tbuf);
				printf("Gateway %d binded to Monitor %d\r\n", Bindid, pointer->Deviceid);
				BindDevice(pointer,Bindid);
				pointer->State = Datapro;
				break;
			}
			else
				return -1;	
		case Datapro:
			;
			uint16_t size = BufferShowRest(pointer->Recv);
			if(pointer->Bind)
			{
				char *p = (char *)malloc(size);
				BufferRead(pointer->Recv,p,size);
				if(write(pointer->Bind->Clientfd,p,size)<=0)
				{
					if(errno == EINTR)
						printf("Do not close socket\r\n");
					else
					{
						if(size)
							printf("Device might be offline, please rebind\r\n");
					}
				}
				free(p);
			}
			else
			{
				printf("No bind on this device\r\n");
				sleep(1);
				BufferReset(pointer->Recv);
				if(pointer->Device == Monitor)
					return -1;
			}
			break;
		default:
			printf("Unknown error!\r\n");
			return -1;			

	}
	return 0;
}

void AddGateway(ClientInfo *pointer)
{
	Gatewaylist *current = G_head;
	Gatewaylist *temp = (Gatewaylist *)malloc(sizeof(Gatewaylist));
	temp->Gateway = pointer;
	temp->next = NULL;
	if(!current)
	{
		current = temp;
		G_head = current;
	}
	else
	{
		while(current->next)
		{
			current = current->next;
			//unique ID check here
		}
		current->next = temp;
	}
	printf("HEAD gateway is %d\r\n",G_head->Gateway->Deviceid);
	return;
}

void DeleteGateway(ClientInfo *pointer)
{
	Gatewaylist *current = G_head;
	if(!current)
		return;
	if(current->Gateway == pointer)
	{
		G_head = current->next;
		free(current);
		return;
	}
	while(current->next->Gateway != pointer)
	{
		current = current->next;
		if(!current->next)
			return;
	}
	Gatewaylist *temp = current->next;
	current->next = current->next->next;
	free(temp);
	return;
}
