#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>

#define TIME_SIZE 25
#define SEC_TO_TIMEOUT 1

typedef struct pdu {
	char server_time[TIME_SIZE];
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
