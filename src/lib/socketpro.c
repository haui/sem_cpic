/*
 * socketpro.c
 *
 *  Created on: 06.05.2014
 *      Author: Stefan Hauenstein
 */

#include "socketpro.h"

/* Error Handling und Exit. */
void error_exit(char *error_message) {
	fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
	exit(EXIT_FAILURE);
}

/** Socket erstellen **/
int create_socket(int af, int type, int protocol) {
	socket_t sock;
	const int y = 1;
	sock = socket(af, type, protocol);
	if (sock < 0)
		error_exit("ERROR: CREATE SOCKET");

	setsockopt(sock, SOL_SOCKET,
	SO_REUSEADDR, &y, sizeof(int));
	return sock;
}

/* Bindung an die Serveradresse */
void bind_socket(socket_t *sock, unsigned long adress, unsigned short port) {
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(adress);
	server.sin_port = htons(port);
	if (bind(*sock, (struct sockaddr*) &server, sizeof(server)) < 0)
		error_exit("ERROR: BIND SOCKET");
}

/* auf "listen" setzen */
void listen_socket( socket_t *sock) {
	if (listen(*sock, 100) == -1)
		error_exit("ERROR: LISTEN SOCKET");
}

/* Client Verbindung annehmen */
void accept_socket( socket_t *socket, socket_t *new_socket) {
	struct sockaddr_in client;
	unsigned int len;

	len = sizeof(client);
	*new_socket = accept(*socket, (struct sockaddr *) &client, &len);
	if (*new_socket == -1)
		error_exit("ERROR: ACCEPT SOCKET");
}

/* Verbindung zum Server */
void connect_socket(socket_t *sock, char *serv_addr, unsigned short port) {
	struct sockaddr_in server;
	struct hostent *host_info;
	unsigned long addr;

	memset(&server, 0, sizeof(server));
	if ((addr = inet_addr(serv_addr)) != INADDR_NONE) {

		/* numerische IP-Adresse */
		memcpy((char *) &server.sin_addr, &addr, sizeof(addr));
	} else {
		/* Servernamen bspw. "localhost" */
		host_info = gethostbyname(serv_addr);
		if (NULL == host_info)
			error_exit("ERROR: SERVERNAME or IP");
		memcpy((char *) &server.sin_addr, host_info->h_addr_list,
		host_info->h_length);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	/* Baue Verbindung auf. */
	if (connect(*sock, (struct sockaddr *) &server, sizeof(server)) < 0)
		error_exit("ERROR: CONNECT SERVER");
}

/* Daten versenden via TCP */
void TCP_send( socket_t *sock, char *data, size_t size) {
	if (send(*sock, data, size+1, 0) == -1)
		error_exit("ERROR: TCP_SEND");
}

/* Daten empfangen via TCP */
void TCP_recv( socket_t *sock, char *data, size_t size) {
	unsigned int len;
	len = recv(*sock, data, size, 0);
	if (len > 0 || len != -1)
		data[len] = '\0';
	else
		error_exit("ERROR: TCP_RECV");
}

/* Daten senden via UDP */
void UDP_send( socket_t *sock, char *data, size_t size, char *addr,
		unsigned short port) {
	struct sockaddr_in addr_sento;
	struct hostent *h;
	int rc;

	/* IP-Adresse des Servers überprüfen */
	h = gethostbyname(addr);
	if (h == NULL)
		error_exit("Unbekannter Host?");

	addr_sento.sin_family = h->h_addrtype;
	memcpy((char *) &addr_sento.sin_addr.s_addr, h->h_addr_list[0],
			h->h_length);
	addr_sento.sin_port = htons(port);

	rc = sendto(*sock, data, size, 0, (struct sockaddr *) &addr_sento,
			sizeof(addr_sento));
	if (rc < 0)
		error_exit("Konnte Daten nicht senden - sendto()");
}

/* Daten empfangen via UDP */
void UDP_recv( socket_t *sock, char *data, size_t size) {
	struct sockaddr_in addr_recvfrom;
	unsigned int len;
	int n;

	len = sizeof(addr_recvfrom);
	n = recvfrom(*sock, data, size, 0, (struct sockaddr *) &addr_recvfrom,
			&len);
	if (n < 0) {
		printf("Keine Daten empfangen ...\n");
		return;
	}
}

/* Socket schließen */
void close_socket( socket_t *sock) {
	close(*sock);
}

void write_eot(int client_socket) {
  write_string(client_socket, "", 0);
}

