/*
 * server.c
 *
 *  Created on: 06.05.2014
 *      Author: Stefan Hauenstein
 */

#include "socketpro.h"
#include "linkedlist.h"

#define BUF 1024
#define SHM_SIZE 5000

struct test_struct {
	char name[8];
	int size[1];
	char content;
	int sem;
	struct test_struct *next;
};

struct test_struct *head = NULL;
struct test_struct *curr = NULL;
socket_t server_sock, client_sock;

void server_start();

void server_stop();

int main(int argc, char **argv) {
	key_t key;
	int shmid;

	int addrlen;
	char *buffer = (char*) malloc(BUF);

	server_sock = create_socket(AF_INET, SOCK_STREAM, 0);
	atexit(cleanup);
	bind_socket(&server_sock, INADDR_ANY, 15000);
	listen_socket(&server_sock);
	addrlen = sizeof(struct sockaddr_in);

	while (1) {
		accept_socket(&server_sock, &client_sock);
		do {
			// TODO: Antwort/Hallo Senden

			TCP_send(&client_sock, buffer, strlen(buffer));
			TCP_recv(&client_sock, buffer, BUF - 1);

			// TODO: Befehl auslessen

		} while (strcmp(buffer, "quit\n") != 0);
		close_socket(&client_sock);
	}
	server_stop();
	return EXIT_SUCCESS;
}

void server_start(key_t key, int shmid) {
	/* Key für shm */
	if ((key = ftok("/server.c", 'R')) == -1) {
		error_exit("SHM-Key konnte nicht erstellt werden");
	};

	/* SHM */
	if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
		error_exit("SHM wurde nicht erstellt");
	}

}

void server_stop() {
	close_socket(&server_sock);
	delete_list();


}
