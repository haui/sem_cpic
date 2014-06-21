/*
 * shmsem.c
 *
 *  Created on: 19.06.2014
 *      Author: Stefan Hauenstein
 */

#include <stdio.h>
#include <stdlib.h>

#include "shmsem.h"

void ss_cleanup(int shmid, int semid) {
	if (shmid > 0) {
		int retcode = shmctl(shmid, IPC_RMID, NULL);

		if (retcode < 0) {
			printf("Error cleaning Shared-Memory!\n");
		}
	}

	if (semid > 0) {
		int retcode = semctl(semid, 0, IPC_RMID, NULL);
		semid = 0;

		if (retcode < 0) {
			printf("Error cleaning Semaphore!\n");
		}
	}

}

int create_shm(key_t key) {
	int result = shmget(key, 1000000, IPC_CREAT | IPC_EXCL | 0600);
	handle_error(result, "shmget():", 1);
	return result;
}

int create_sem(key_t key, const int sem_size,int flags) {
	int result = semget(key, sem_size, flags | 0600);
	handle_error(result, "semget():", 1);
	return result;
}

