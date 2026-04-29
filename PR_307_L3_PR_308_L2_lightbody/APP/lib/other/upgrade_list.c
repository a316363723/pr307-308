#include "stdlib.h"
#include "stdio.h"
#include "upgrade_list.h"

#define TRUE  1
#define FALSE 0

Upgrade_Queue queue;

int QueueFull(Upgrade_Queue* q){
    if(q == NULL){
        return FALSE;
    }
    return (q->rear + 1) % MAXSIZE == q->front;
}

int QueueEmpty(Upgrade_Queue* q){
    if(q == NULL){
        return FALSE;
    }
    return q->front == q->rear;
}

int QueueLength(Upgrade_Queue* q){
    if(q == NULL){
        return FALSE;
    }
    return (q->rear - q->front + MAXSIZE) % MAXSIZE;
}

int EnQueue(Upgrade_Queue* q,const uint8_t *data,uint16_t size){
    if(QueueFull(q)){
        return FALSE;
    }
    // 队尾入队
    memcpy(q->data[q -> rear],data,size);
	q->size_buf[q -> rear] = size;
    // 更新队尾指针
    q->rear = (q->rear + 1) % MAXSIZE;
    return TRUE;
}

int DeQueue(Upgrade_Queue* q, uint8_t *val){
    if(QueueEmpty(q)){
        return FALSE;
    }
    // 队头元素出队
    memcpy(val,q->data[q->front],q->size_buf[q->front]);
    // 更新队头指针
    q->front = (q->front + 1) % MAXSIZE;
    return TRUE;
}

