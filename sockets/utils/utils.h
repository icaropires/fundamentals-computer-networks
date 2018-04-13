#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>

#define TIME_SIZE 40 
#define URL_SIZE 2049 
#define IP_SIZE 16
#define SEC_TO_TIMEOUT 1

#define RTT_OPERATION  1
#define RSV_OPERATION  0 

typedef struct pdu {
  int operation;
	char server_time[TIME_SIZE];
	char ip[IP_SIZE];
	char url[URL_SIZE];
} Package;

typedef struct header {
	int sfd;
	Package *package;
	int flags;
	struct sockaddr *address;
	socklen_t dest_len;
} Header;

struct tm* get_local_time();

char* format_timeinfo(struct tm* timeinfo);

char* get_formated_time();

#endif  // UTILS_H_
