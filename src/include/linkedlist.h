/*
 * linkedlist.h
 *
 *  Created on: 06.05.2014
 *      Author: Stefan Hauenstein
 */
#ifndef LINKEDLIST_H_INCLUDED
#define LINKEDLIST_H_INCLUDED

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdint.h>
#include "linkedlist.h"

extern struct list_struct *tmp;

int create_list(struct list_struct *tmp, int shmid);

int add_to_list(struct list_struct *tmp, int shmid, int semid);

int search_in_list(char *name, struct list_struct **prev);

int delete_from_list(char *name);

char *print_list();

#endif /* LINKEDLIST_H_INCLUDED */
