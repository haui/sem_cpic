/*
 * linkedlist.c
 *
 *  Created on: 06.05.2014
 *      Author: Stefan Hauenstein
 */

#include "linkedlist.h"

struct list_struct {
	char name[8];
	int size;
	char content[4096];
	int semval[1];
	struct list_struct *prev;
	struct list_struct *next;
};

extern struct list_struct *head;
extern struct sembuf sem_one, sem_all, sem_one_reset, sem_all_reset;

int create_list(struct list_struct *tmp, int shmid) {

	if (NULL == tmp) {
		printf("ERROR: add_to_list \n");
		return 1;
	} else {
		tmp = (struct list_struct *) shmat(shmid, NULL, 0);

	}

	head = tmp;
	tmp->next = tmp->prev = head;
	return 0;
}

int add_to_list(struct list_struct *tmp, int shmid, int semid) {

	if (NULL == head) {
		if (create_list(tmp, shmid) != 0) {
			printf("ERROR: add_to_list");
			return 1;
		}
	}

	if (NULL == tmp) {
		printf("ERROR: add_to_list");
		return 1;
	} else {
		tmp = (struct list_struct *) shmat(shmid, NULL, 0);

		/*block tmp*/
		semctl(semid, 0, SETALL, tmp->semval[0]);
		semop(semid, &sem_all, 1);

		/*block head*/
		semctl(semid, 0, SETALL, head->semval[0]);
		semop(semid, &sem_all, 1);

		if (head->next != head) {
			/*block prev*/
			semctl(semid, 0, SETALL, head->prev->semval[0]);
			semop(semid, &sem_all, 1);

			tmp->next = head;
			tmp->prev = head->prev;
			tmp->prev->next = tmp;
			head->prev = tmp;

			/*Unblock prev*/
			semctl(semid, 0, SETALL, tmp->prev->semval[0]);
			semop(semid, &sem_all_reset, 1);

		} else {
			head->next = tmp;
			head->prev = tmp;
			tmp->next = head;
			tmp->prev = head;

		}
		/*Unblock head*/
		semctl(semid, 0, SETALL, head->semval[0]);
		semop(semid, &sem_all_reset, 1);

		/*Unblock ptr*/
		semctl(semid, 0, SETALL, tmp->semval[0]);
		semop(semid, &sem_all_reset, 1);

		return 0;
	}
}

int search_in_list(char *name, struct list_struct **ptr) {
	if (head == NULL) {
		return 0;
	}
	int found = 0;
	struct list_struct *search = (struct list_struct *) ptr;
	while (search != NULL) {
		if (strcmp(search->name, name)) {
			*ptr = search;
			found = 1;
			break;
		} else {
			search = search->next;
		}
	}
	if (found == 1) {
		return 1;
	} else {
		return 0;
	}
}

int delete_from_list(char *name) {
	struct list_struct *search = head;
	int del = 0;

	del = search_in_list(name, &search);
	if (del == 0) {
		return -1;
	} else {
		if (search == head) {
			head = search->next;
			search->prev = head->prev;
			shmdt(search);
			free(search);
		} else {
			search->prev->next = search->next;
			search->next->prev = search->prev;
			shmdt(search);
			free(search);
		}

	}

	return 0;
}

char *print_list() {
	struct list_struct *ptr = head;
	char *str, *message = '\0';
	int count = 0;

	while (ptr != NULL) {
		str = '\0';
		sprintf(str, "\n %s \n", ptr->name);
		strcat(message, str);
		ptr = ptr->next;
		count++;
	}
	sprintf(message, "ACK %d\n%s", count, message);
	return message;
}
