/*
 * server.c
 *
 *  Created on: 06.05.2014
 *      Author: Stefan Hauenstein
 */

#include <signal.h>
#include "socketpro.h"
#include "linkedlist.h"
#include "shmsem.h"

#define BUF 4096

struct list_struct {
	char name[256];
	int size;
	char content[4096];
	int semval[1];
	struct list_struct *next;
};

union semun {
	int val;
	struct semid_ds *buf;
	short * array;
} sem_val_cd;

struct list_struct *head = NULL;

struct list_struct *curr = NULL;

socket_t server_sock, client_sock;

key_t shmkey, semkey;
int shmid, semid, semid_cd;

struct sembuf sem_one, sem_all, sem_one_reset, sem_all_reset;

int server_start();

void server_stop();

void sig_handler(int sig) {
	server_stop();
	exit(0);
}

int main(int argc, char *argv[]) {

	int ADDRLen, port;
	port = 15000;

	/* Message and Buffer */

	char *buffer = (char*) malloc(BUF);

	if (server_start(port) != 0) {
		printf("ERROR: START FAILED\n");
		server_stop();
		exit(0);
	}

	accept_socket(&server_sock, &client_sock);
	int pid;

	pid = fork();

	if (pid == 0) {

		/* CHILD*/
		char msg[4098];
		sprintf(msg, "Welcome to File Server 5150.42\n");

		TCP_send(&client_sock, msg, strlen(buffer));

		while (1) {

			msg[0] = '\0';
			TCP_recv(&client_sock, buffer, BUF - 1);

			char delimiter[] = " ,;:\n";

			char *command;

			struct list_struct *tmp;

			strcpy(command, strtok(buffer, delimiter));
			strcpy(tmp->name, strtok(NULL, delimiter));
			tmp->size = atoi(strtok(NULL, delimiter));

			if (strcmp(command, "LIST")) {

				char *retstr = print_list();
				strcpy(msg, retstr);

			} else if (strcmp(command, "CREATE")) {
				struct list_struct *ptr = head;
				if (semctl(semid_cd, 0, SETALL, sem_val_cd) < 0) {
					printf("ERROR SEMCTL FOR FILE\n");
				}
				if (semop(semid_cd, &sem_one, 1) < 0) {
					printf("ERROR SEMOP FOR FILE\n");
				}

				if (search_in_list(tmp->name, &ptr) == 1) {
					sprintf(msg, "FILEEXISTS\n");
					TCP_send(&client_sock, msg, strlen(msg));
				} else if (add_to_list(tmp, shmid, semid) == 1) {
					printf("ERROR: add_to_list \n");
				} else {

					sprintf(msg, "FILECREATED\n");
					printf("%s\n", msg);
					TCP_send(&client_sock, msg, strlen(msg));
				}
				if (semop(semid_cd, &sem_one_reset, 1) < 0) {
					printf("ERROR SEMOP_RESET FOR FILE\n");
				}
				break;
			} else if (strcmp(command, "UPDATE")) {


			} else if (strcmp(command, "DELETE")) {
				struct list_struct *ptr = head;
				if (semctl(semid_cd, 0, SETALL, sem_val_cd) < 0) {
					printf("ERROR SEMCTL FOR SEARCH_DEL\n");
				}
				if (semop(semid_cd, &sem_one, 1) < 0) {
					printf("ERROR SEMOP FOR SEARCH_DEL\n");
				}

				if (search_in_list(tmp->name, &ptr) != 1) {
					if (semop(semid_cd, &sem_one_reset, 1) < 0) {
						printf("ERROR SEMOP_RESET SEARCH_DEL\n");
					}

					sprintf(msg, "FILENOTEXISTS\n");
					TCP_send(&client_sock, msg, strlen(msg));
				} else {

					// Lock File //
					if (semctl(semid, 0, SETALL, ptr->semval) < 0) {
						printf("ERROR SEMCTL FOR DEL FILE\n");
					}

					if (delete_from_list(ptr, semid) != 0) {
						printf("ERROR DELETE FILE\n");
					}

					if (semop(semid_cd, &sem_one_reset, 1) < 0) {
						printf("ERROR SEMOP_RESET SEARCH_DEL\n");
					}

				}

			} else if (strcmp(command, "READ")) {


			}

		}
		close(client_sock);

	}
	server_stop();
	return EXIT_SUCCESS;
}

//Server starten
int server_start(int port) {
	/* SIG Handler */
	signal(SIGINT, sig_handler);

	/* Shared-Memory */
	printf("Create SHM\n");
	shmid = create_shm();

	/* Semaphore */
	printf("Create Semaphore\n");
	semid = create_sem(10, IPC_CREAT);

	semid_cd = create_sem(1, IPC_CREAT);

	sem_one.sem_num = 0;
	sem_one.sem_op = -1;
	sem_one.sem_flg = SEM_UNDO;

	sem_one_reset.sem_num = 0;
	sem_one_reset.sem_op = 1;
	sem_one_reset.sem_flg = SEM_UNDO;

	sem_all.sem_num = 0;
	sem_all.sem_op = -10;
	sem_all.sem_flg = SEM_UNDO;

	sem_all_reset.sem_num = 0;
	sem_all_reset.sem_op = 10;
	sem_all_reset.sem_flg = SEM_UNDO;

	if (create_socket(AF_INET, SOCK_STREAM, 0) < 0) {
		exit(1);
	}

	atexit(server_stop);
	bind_socket(&server_sock, INADDR_ANY, port);

	listen_socket(&server_sock);

	exit(0);
}

//Server beenden
void server_stop() {
	close_socket(&server_sock);
	ss_cleanup(shmid, semid);

}
