#ifndef _MQ_H_
#define _MQ_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>	//semaphore

struct MsgQueue {
	int **buf;
	int n;
	int front, rear;
	sem_t mutex;
	sem_t slots;
	sem_t items;
};

struct MsgQueue *NewMQ(int n);
void DeleteMQ(struct MsgQueue *this);
void put(struct MsgQueue *this, int *item);
int *get(struct MsgQueue *this);
#endif
