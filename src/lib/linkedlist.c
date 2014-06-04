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
extern struct sembuf sem_all, sem_all_reset;

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

		if (head->next != head) {

			tmp->next = head;
			tmp->prev = head->prev;
			tmp->prev->next = tmp;
			head->prev = tmp;

		} else {
			head->next = tmp;
			head->prev = tmp;
			tmp->next = head;
			tmp->prev = head;

		}

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

int delete_from_list(struct list_struct *search, int semid) {
	if (semop(semid, &sem_all, 1) < 0) {
		printf("ERROR SEMOP FOR DEL FILE\n");
	}

	if (search == head) {
		head = search->next;
		search->prev->next = head;
		head->prev = search->prev;

		shmdt(search);
		free(search);
	} else {
		search->prev->next = search->next;
		search->next->prev = search->prev;
		shmdt(search);
		if (semop(semid, &sem_all_reset, 1) < 0) {
			printf("ERROR RESET SEMOP FOR DEL FILE\n");
		}
		free(search);
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
