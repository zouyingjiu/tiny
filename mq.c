#include "mq.h"
#include "PV.h"

struct MsgQueue *NewMQ(int n) {
	struct MsgQueue *pMQ = (struct MsgQueue*)malloc(sizeof(struct MsgQueue));
	if (NULL == pMQ) {
		perror("malloc");
		exit(errno);
	}
	pMQ->buf = (int **)malloc(sizeof(int*) * n);
	if (pMQ->buf == NULL) {
		perror("malloc buf");
		exit(errno);
	}
	pMQ->n = n;
	pMQ->front = pMQ->rear = 0;
	sem_init(&pMQ->mutex, 0, 1);
	sem_init(&pMQ->slots, 0, n);
	sem_init(&pMQ->items, 0, 0);

	return pMQ;
}

void DeletePMQ(struct MsgQueue *this) {
	free(this->buf);
	free(this);
}

void put(struct MsgQueue *this, int *item) {
	P(&this->slots);
	P(&this->mutex);
	this->buf[(++this->rear)%(this->n)]  = item;
	V(&this->mutex);
	V(&this->items);
}

int *get(struct MsgQueue *this) {
	int *item;
	P(&this->items);
	P(&this->mutex);
	item = this->buf[(++this->front) %(this->n)];
	V(&this->mutex);
	V(&this->slots);

	return item;
}
