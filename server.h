#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>
#include "mq.h"

#define little < 
#define bigger >
#define equal ==

int setup(int port);

int RunBasic(int port, void *(*clientHandle)(int));

int RunFork(int port, void *(*clientHandle)(int));

int RunThread(int port, void *(*clientHandle)(void*));

struct PoolArgs {
	struct MsgQueue *q;
	void *(*handle)(int);
};

void *PoolHandle(int fd);

void *PoolFunc(void *args);

int RunPool(int port, int poolSize, void *(clientHandle)(int));

void *BasicHandle(int fd);

void *ThreadHandle(void *args);
