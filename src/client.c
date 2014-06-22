/*
 * test.c
 *
 *  Created on: 16.06.2014
 *      Author: Stefan Hauenstein
 *
 *
 *     Client um den Server zu testem
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sock;

void handle_error(int retcode, char* etxt);

void sig_handler(int sig) {
	close(sock);
	exit(0);
}

int main(int argc, char **argv) {

	int port, retcode;
	struct sockaddr_in serv_addr;
	char *serverIP;

	char buffer[256];

	port = 15000;
	serverIP = "127.0.0.1";

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	handle_error (retcode,"ERROR SOCKET()\n");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(serverIP);
	serv_addr.sin_port = htons(port);

	retcode = connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	handle_error(retcode, "ERROR CONNECT()\n");

	while (1) {
		fgets(buffer, 256, stdin);

		retcode = write(sock, buffer, strlen(buffer));

		handle_error(retcode, "ERROR WRITE()\n");

		bzero(buffer, 500);

		retcode = read(sock, buffer, 256);
		handle_error(retcode, "ERROR READ()\n");

		printf("%s\n", buffer);
	}

}

void handle_error(int retcode, char* etxt) {
	if (retcode < 0) {
		perror(etxt);
	}
}
