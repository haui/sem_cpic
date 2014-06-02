/*
 * shmsem.h
 *
 *  Created on: 01.06.2014
 *      Author: Stefan Hauenstein
 */
#ifndef SHMSEM_H_
#define SHMSEM_H_


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include "shmsem.h"


int create_shm();

int create_sem(const int sem_size, int flags);

void ss_cleanup(int shmid, int semid);

void init_sem ();

#endif /* SHMSEM_H_ */
