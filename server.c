#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>
#include "mq.h"
#include "server.h"

#define little < 
#define bigger >

int setup(int port) {
	int fd = -1, ret = -1;
	struct sockaddr_in addr;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons((unsigned short) port);

	ret = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		perror("bind");
		return -2;
	}
	if (listen(fd, 5) < 0) {
		perror("listen");
		return -3;
	}
	return fd;
}

int RunBasic(int port, void *(*clientHandle)(int)){
	int fd = setup(port);
	while(1) {
		int client = accept(fd, NULL, NULL);
		if (client < 0) {
			perror("accept");
			continue;
		}
		clientHandle(client);
		close(client);
	}
}

int RunFork(int port, void *(*clientHandle)(int)) {
	int fd = setup(port);
	while(1) {
		int client = accept(fd, NULL, NULL);
		if (client < 0) {
			perror("accept");
			continue;
		}
		if(fork() == 0) {
			close(fd);
			clientHandle(client);
			close(client);
			exit(0);
		}
		close(client);

	}
}

int RunThread(int port, void *(*clientHandle)(void*)) {
	int fd = setup(port);
	while (1) {
		int *client = (int*)malloc(sizeof(int));
		*client = accept(fd, NULL, NULL);
		if (client < 0) {
			perror("accept");
			continue;
		}
		pthread_t tid;
		pthread_create(&tid, NULL, clientHandle, (void *)client);
	}
}


int RunPool(int port, int poolSize, void *(clientHandle)(int)) {
	struct MsgQueue *mq = NewMQ(1024);
	int i = 0;
	pthread_t tid;
	for (i = 0; i < poolSize; i++)	 {
		struct PoolArgs args;
		args.q = mq;
		args.handle = clientHandle;
		pthread_create(&tid, NULL, PoolFunc, (void*)&args);
	}
	int fd = setup(port);
	if (fd < 0) {
		printf("network setup error");
		exit(errno);
	}
	while(1) {
		int *client = (int *)malloc(sizeof(int));
		*client = accept(fd, NULL, NULL);
		if (client < 0) {
			perror("accept");
			continue;
		}
		put(mq, client);
	}
}

//do not need to close connect fd
void *BasicHandle(int fd) {
	printf("basic handle client %d\n", fd);
	return NULL;
}

//ATTEMTION: remember to close fd in the end
void *ThreadHandle(void *args) {
	int fd = *(int*) args;
	free(args);
	pthread_detach(pthread_self());
	printf("thread handle client %d\n", fd);
	close(fd);	// remember to close the fd in thread model
	return NULL;
}

//ATTENTION；remember to close the fd in the end.
void *PoolHandle(int fd) {
	printf("pool handle client %d\n", fd);
	close(fd);
}

void *PoolFunc(void *args) {
	struct PoolArgs *param= (struct PoolArgs*)(args);
	struct MsgQueue *q = param->q;
	while (1) {
		int *pfd = get(q);
		int fd = *pfd;
		free(pfd);
		param->handle(fd);
	}
	return NULL;
}


int main(int argc, char **argv) {
	//RunBasic(5678, BasicHandle);
	//RunFork(5679, BasicHandle);
	//RunThread(5680, ThreadHandle);
	RunPool(5681, 4,  PoolHandle);
	//RunSelect(5682, SelectHandle);
	//RunEpoll(5683, EpollHandle);
	return 0;
}

