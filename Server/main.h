//SERVER
#ifndef MAIN_H
#define	MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include<time.h>
#include <stdint.h>

int port, socket_fd, new_socket_fd ,pid;
struct sockaddr_in serv_addr, cli_addr;
socklen_t cli_len;
time_t t;
int bytesReceived;
uint16_t parameters[4];
int i;
int j;
#endif	/* MAIN_H */

