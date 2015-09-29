//CLIENT
#ifndef MAIN_H
#define	MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <math.h>
#include <stdint.h>
int socket_fd, port;
struct sockaddr_in serv_addr;
struct hostent *server;
char filename[50];
//char hostname[50] = "student-VirtualBox";
char hostname[50];
int i;
unsigned char char_from_file;
int readed;
char bin[9];
int parameter;
int count1=3;
int count2=3;
char hamming_1[7];
char hamming_2[7];
int hamming_matrix_1[7] = {0,0,0,0,0,0,0};
int hamming_matrix_2[7] = {0,0,0,0,0,0,0};
int hamming_bin_1;
int hamming_bin_2;
uint16_t parameters[4];
char out;
int packet_size;
int error_generation_code;
int correcting_code;
int parameter;
int n_read;
unsigned long int sent_crc = 0xffffffff;
unsigned long int received_crc = 0xffffffff;
FILE *file;

#endif	/* MAIN_H */

