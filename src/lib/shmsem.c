/*
 * shmsem.c
 *
 *  Created on: 06.05.2014
 *      Author: Stefan Hauenstein
 */

#include "shmsem.h"

extern key_t shmkey, semkey;

extern struct sembuf sem_one, sem_all, sem_one_reset, sem_all_reset;

const char *REF = "./ref.txt";

int create_shm() {
	FILE *fd;
	fd = fopen(REF, "rb+");
	if (fd == NULL) {
		printf("CREATING REF\n");
		fd = fopen(REF, "wb");
	}

	shmkey = ftok(REF, 1);
	if (shmkey < 0) {
		perror("ERROR: FTOK SHM\n");
		exit(-1);
	}

	printf("Setting up SHM!\n");
	int result = shmget(shmkey, 50000, IPC_CREAT | IPC_EXCL | 0600);
	if (result < 0) {
		printf("ERROR: SHMGET FAILED\n");
		exit(-1);
	}
	return result;
}

void ss_cleanup(int shmid, int semid) {
	if (shmid > 0) {
		int retcode = shmctl(shmid, IPC_RMID, NULL);

		if (retcode < 0) {
			printf("ERROR: SHMCTL\n");
		}
	}

	if (semid > 0) {
		int retcode = semctl(semid, 0, IPC_RMID, NULL);

		if (retcode < 0) {
			printf("ERROR: SEMCTL\n");
		}
	}
}

int sem_create(const int sem_size, int flags) {
	semkey = ftok(REF, 7);
	if (semkey < 0) {
		perror("ERROR FTOK SEMAPHOR!\n");
		exit(1);
	}

	int result = semget(semkey, sem_size, flags | 0600);
	if (result < 0) {
		printf("ERROR: SEMAPHOR FAILED\n");
		exit(1);
	}
	printf("SemID=%d, key=%ld\n", result, (long) semkey);

	return result;
}
