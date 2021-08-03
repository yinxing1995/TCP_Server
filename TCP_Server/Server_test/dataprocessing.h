#ifndef DARAPROCESSING_H
#define DARAPROCESSING_H

#include <sys/time.h>
#include <stdint.h>
#include "ringbuffer_reusable.h"
#include <pthread.h>
//typedef struct timer_list timer;
pthread_mutex_t mutex_structure;

enum States
{
    Recog = 0x00,
    Init = 0x01,
    Bind = 0x02,
    Datapro = 0x03,
};

enum Device
{
    Unknown = 0x00,
    Gateway = 0x01,
    Monitor = 0x02,
};

enum FrameType
{
    REPORT = 0x00,
    CHECK = 0x01,
    NETSTATUS = 0x02,
};

enum DataType
{
    _INT = 0x00,
    _FLOAT = 0x01,
};

typedef struct Client
{
    int Device;
    int Deviceid;
    //int Bindid;
    int State;
    int Clientfd;
    Ringbuf *Recv;
    struct Client *Bind;
}ClientInfo;

typedef struct ClusterStructure
{
    //timer * Timer;
    uint8_t NodeID;
    uint8_t ClusterID;
    //char * Description;
    uint8_t Endpoint;
    uint8_t DataType;
    uint8_t DataLength;
    void * Data;
    uint8_t ControlFlag;
    struct ClusterStructure *next;
}ClusterArray;

typedef struct GatewayInfo
{
    ClientInfo *Gateway;
    struct GatewayInfo *next;
}Gatewaylist;


int Statemachine(ClientInfo *);
void AddGateway(ClientInfo *);
void DeleteGateway(ClientInfo *);

#endif

