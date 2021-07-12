#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "dataprocessing.h"

#define GATEWAYFLAG "Gateway:"
#define MONITORFLAG "Monitor:"
#define BIND "Bind:"
#define LEN_ID 3


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
				break;
			}
		case Init:
			memset(tbuf,'\0',sizeof(tbuf));
			if(BufferRead(pointer->Recv,tbuf,LEN_ID)<0)
				break;
			pointer->Deviceid = atoi(tbuf);
			printf("buf = %s\r\n",tbuf);
			printf("ID = %d\r\n",pointer->Deviceid);
			
			if(pointer->Device == Monitor)
			{
				pointer->State = Bind;
				printf("State has been set to Bind\r\n");
				break;
			}
			pointer->State = Datapro;
			break;
		case Bind:
			memset(tbuf,'\0',sizeof(tbuf));
			if(BufferRead(pointer->Recv,tbuf,strlen(BIND))<0)
				break;
			printf("tbuf = %s\r\n",tbuf);
			if(!strcmp(BIND,tbuf))
			{

				if(BufferRead(pointer->Recv,tbuf,LEN_ID)<0)
					break;
				pointer->Bindid = atoi(tbuf);
				printf("Gateway %d binded to Monitor %d\r\n", pointer->Bindid,pointer->Deviceid);
				pointer->State = Datapro;
			}
			else
				return -1;	
		case Datapro:
			break;
		default:
			printf("Unknown error!\r\n");
			return -1;			

	}
	return 0;
}
