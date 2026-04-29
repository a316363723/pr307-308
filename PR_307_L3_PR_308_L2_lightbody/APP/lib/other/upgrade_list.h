#ifndef UPGRADE_LIST
#define UPGRADE_LIST

#include "hc32_ddl.h"

#define Data_Num 10
#define Data_length 512
#define MAXSIZE 10

typedef __packed struct Queue
{
	uint8_t front;
	uint8_t rear;
	uint8_t size_buf[Data_Num];
    uint8_t data[Data_Num][Data_length];
}Upgrade_Queue;

extern Upgrade_Queue queue;

int EnQueue(Upgrade_Queue* q,const uint8_t *data,uint16_t size);
int DeQueue(Upgrade_Queue* q, uint8_t *val);

#endif

