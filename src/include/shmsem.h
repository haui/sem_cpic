/*
 * shmsem.h
 *
 *  Created on: 19.06.2014
 *      Author: Stefan Hauenstein
 */
#ifndef SHMSEM_H_
#define SHMSEM_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>
#include "shmsem.h"

void ss_cleanup(int shmid, int semid);
int create_shm(key_t key, const char *txt, const char *etxt);
int create_sem(key_t key, const int sem_size, const char *txt, const char *etxt,
		int flags);

int lock_sem(int counter, struct sembuf *semb);

#endif /* SHMSEM_H_ */
