/*
 * client.c
 *
 *  Created on: 06.05.2014
 *      Author: Stefan Hauenstein
 */

#include <string.h>
#include "socketpro.h"
#define BUF 1024

socket_t sock;

void client_stop();

int main (int argc, char *argv[]) {
  char *buffer = (char *)malloc (BUF);

  if( argc < 2 ){
     printf("Usage: %s ServerAdresse\n", *argv);
     exit(EXIT_FAILURE);
  }

  sock = create_socket(AF_INET, SOCK_STREAM, 0);
  atexit(client_stop);

  connect_socket(&sock, argv[1], 15000);

  do {
      buffer[0] = '\0';
      TCP_recv (&sock, buffer, BUF-1);
      printf ("%s\n", buffer);
      printf ("---");
      fgets (buffer, BUF, stdin);
      TCP_send (&sock, buffer, strlen (buffer));
  } while (strcmp (buffer, "quit\n") != 0);
  close_socket (&sock);
  return EXIT_SUCCESS;
}

void client_stop(){
	write_eot(sock);
	close_socket(&sock);

}
