#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct pdu {
    char server_time[30];
} Package;

char* get_local_time();

#endif  // UTILS_H_
