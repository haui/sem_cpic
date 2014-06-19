/*
 * run.h
 *
 *  Created on: 19.06.2014
 *      Author: Stefan Hauenstein
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef RUN_H_
#define RUN_H_

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

typedef struct datei {
	char name[256];
	int size;
	char content[4096];
	short semval[1];
} node_t;

void close_socket();


void sig_handler(int sig);


void handle_error(int retcode, char* etxt);
void *handle_client(void * client_socket_ptr);

#endif /* RUN_H_ */
