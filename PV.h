#ifndef _PV_H_
#define _PV_H_
#include <semaphore.h>	//semaphore

void P(sem_t *s) {
	sem_wait(s);
}

void V(sem_t *s) {
	sem_post(s);
}
#endif
