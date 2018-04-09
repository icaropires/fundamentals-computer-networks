#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../utils/utils.h"

#define QUEUE_LENGHT 5

struct sockaddr_in get_server_address(char *ip, char *port);

struct sockaddr_in get_client_address();

int create_socket(int domain, int type, int protocol);

void bind_socket(int socket, const struct sockaddr *address, socklen_t address_len);

void connect_to_socket(int sfd, const struct sockaddr_in *server_address, socklen_t address_len);

void send_message_to(Header *header);

void get_message_from(Header *header);

#endif  // SOCKETS_H_
