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
#include "server.h"

#include "shmsem.h"

key_t shmkey, semkey;
int shmid;
int semid;
int server_sock, client_sock;
node_t *nodes;

const char *REF_FILE = "./src/server.c";

void close_socket() {
	close(server_sock);
	close(client_sock);
}

void sig_handler(int sig) {
	ss_cleanup(shmid, semid);
	close_socket();
	exit(0);
}

void handle_error(int retcode, char* etxt) {
	if (retcode < 0) {
		perror(etxt);

	}
}

int main(int argc, char *argv[]) {
	int retcode;

	signal(SIGINT, sig_handler);

	FILE *fptr;
	fptr = fopen(REF_FILE, "rb+");
	if (fptr == NULL) {
		fptr = fopen(REF_FILE, "wb");
	}

	shmkey = ftok(REF_FILE, 9);
	handle_error(semkey, "ftok():");

	shmid = create_shm(shmkey, "create", "SHMGET FAILED!\n");
	handle_error(shmid, "create_shm():");

	nodes = (struct datei *) shmat(shmid, NULL, 0);

	semkey = ftok(REF_FILE, 2);
	handle_error(semkey, "ftok():");
	semid = create_sem(semkey, 10, "create", "SEMAPHOR FAILED!\n", IPC_CREAT);
	handle_error(semid, "create_sem():");

	struct sembuf sem_one, sem_all, sem_one_reset, sem_all_reset;

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

	int clntLen, portno;
	char buffer[256];
	struct sockaddr_in server_addr, clnt_addr;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	handle_error(server_sock, "socket():");

	bzero((char *) &server_addr, sizeof(server_addr));
	portno = 15000;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(portno);

	retcode = bind(server_sock, (struct sockaddr *) &server_addr,
			sizeof(server_addr));
	handle_error(retcode, "bind():");

	retcode = listen(server_sock, 100);
	handle_error(semid, "listen():");
	clntLen = sizeof(clnt_addr);

	while (1) {
		client_sock = accept(server_sock, (struct sockaddr *) &clnt_addr,
				&clntLen);
		handle_error(semid, "accept():");

		int pid;

		pid = fork();

		if (pid < 0) {
			close(client_sock);
			perror("ERROR ACCEPTING CONNECTION!\n");
			exit(1);
		} else if (pid > 0) {
			continue;
		} else {
			/* CHILD */
			while (1) {

				int result = read(client_sock, buffer, 255);
				handle_error(result, "write()");

				char delimiter[] = " ,;:\n";

				char *befehl = strtok(buffer, delimiter);
				char *dateiname = strtok(NULL, delimiter);
				char *groesse = strtok(NULL, delimiter);

				if (strcmp(befehl, "LIST") == 0) {
					int i = 0;
					int c = 0;
					char *temp;
					char message[5000];
					temp = (char*) malloc(sizeof(nodes));

					while ((int) strlen(nodes[i].name) != 0) {
						if (strcmp(nodes[i].name, "DELETED") == 0) {
							i++;
						} else {
							strcat(temp, nodes[i].name);
							strcat(temp, "\n");
							c++;
							i++;
						}
					}

					sprintf(message, "ACK %d\n%s", c, temp);

					result = write(client_sock, message, strlen(message));

					handle_error(result, "write()");

					temp[0] = '\0';
					message[0] = '\0';
				} else if (strcmp(befehl, "CREATE") == 0) {
					int i = 0;
					int status = 0;
					char *message;
					message = "FILEEXISTS\n";

					while ((int) strlen(nodes[i].name) != 0) {
						if (strcmp(nodes[i].name, dateiname) != 0) {
							i++;
						} else {
							status = 1;
							break;
						}
					}

					if (status != 1) {

						i = 0;
						while ((int) strlen(nodes[i].name) != 0) {
							if (strcmp(nodes[i].name, "DELETED") == 0) {
								break;
							}
							i++;
						}

						nodes[i].semval[0] = (short) 10;

						strcpy(nodes[i].name, dateiname);
						nodes[i].size = atoi(groesse);
						result = write(client_sock, "CONTENT:\n", 10);
						handle_error(result, "write()");
						result = read(client_sock, buffer, 255);
						handle_error(result, "read()");
						strcpy(nodes[i].content, buffer);
						message = "FILECREATED\n";
					}

					result = write(client_sock, message, strlen(message));

					handle_error(result, "write()");

				} else if (strcmp(befehl, "READ") == 0) {
					int i = 0;
					char message[256];
					sprintf(message,"NOSUCHFILE\n");

					while ((int) strlen(nodes[i].name) != 0) {
						if (strcmp(nodes[i].name, dateiname) == 0) {

							result = semctl(semid, 0, SETALL,
									&nodes[i].semval[0]);
							handle_error(result, "semctl()");

							result = semop(semid, &sem_one, 1);
							handle_error(result, "semop()");

							nodes[i].semval[0] = semctl(semid, 0,
							GETVAL, 0);

							sprintf(message, "FILECONTENT %s %d\n%s\n",
									nodes[i].name, nodes[i].size,
									nodes[i].content);

							if (semop(semid, &sem_one_reset, 1) < 0) {
								printf("ERROR SEMOP_UNDO FOR FILE\n");
							}

							nodes[i].semval[0] = semctl(semid, 0,
							GETVAL, 0);

							break;
						}
						i++;
					}

					result = write(client_sock, message, strlen(message));
					message[0] = '\0';
					handle_error(result, "write()");

				} else if (strcmp(befehl, "UPDATE") == 0) {
					int i = 0;
					char *message;
					message = "NOSUCHFILE\n";

					while ((int) strlen(nodes[i].name) != 0) {
						if (strcmp(nodes[i].name, dateiname) == 0) {
							result = semctl(semid, 0, SETALL,
									&nodes[i].semval[0]);
							handle_error(result, "semctl()");
							nodes[i].size = atoi(groesse);
							strcpy(nodes[i].content, "");
							message = "CONTENT:\n";

							result = semop(semid, &sem_all, 1);
							handle_error(result, "semop()");

							nodes[i].semval[0] = semctl(semid, 0,
							GETVAL, 0);
							result = read(client_sock, buffer, 256);

							handle_error(result, "write()");

							strcpy(nodes[i].content, buffer);

							result = write(client_sock, "UPDATED\n", 9);

							handle_error(result, "write()");

							result = semop(semid, &sem_all_reset, 1);
							handle_error(result, "semop()");

							nodes[i].semval[0] = semctl(semid, 0, GETVAL, 0);

							break;
						}

						i++;
					}

					result = write(client_sock, message, strlen(message));

					handle_error(result, "write()");

				} else if (strcmp(befehl, "DELETE") == 0) {
					int i = 0;
					char *message;
					message = "NOSUCHFILE\n";
					while ((int) strlen(nodes[i].name) != 0) {
						if (strcmp(nodes[i].name, dateiname) == 0) {
							result = semctl(semid, 0, SETALL,
									&nodes[i].semval[0]);
							handle_error(result, "write()");

							if (semop(semid, &sem_all, 1) < 0) {
								printf("ERROR SEMOP FOR FILE\n");
							}

							nodes[i].semval[0] = semctl(semid, 0,
							GETVAL, 0);
							strcpy(nodes[i].name, "DELETED");
							strcpy(nodes[i].content, "");
							nodes[i].size = 0;
							message = "DELETED\n";

							result = semop(semid, &sem_all_reset, 1);
							handle_error(result, "semop()");

							nodes[i].semval[0] = semctl(semid, 0,
							GETVAL, 0);

							break;
						}

						i++;
					}

					result = write(client_sock, message, strlen(message));
					handle_error(result, "write()");

				} else {
					retcode = write(client_sock, "CMDUNKNOWN\n", 19);
					handle_error(result, "write()");
				}

			}
		}
	}
}

