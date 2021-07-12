#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "dataprocessing.h"

#define GATEWAYFLAG "Gateway:"
#define MONITORFLAG "Monitor:"
#define LEN_ID 3

//static int State = Recog;
//static int Device = Gateway;

int Statemachine(int Socketfd,ClientInfo *pointer)
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
			//printf("Device Recognized\r\n");
			sleep(1);
			break;
		default:
			printf("Unknown error!\r\n");
			return -1;			

	}
	return 0;
}
