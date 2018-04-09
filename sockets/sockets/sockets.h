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

void listen_to_socket(int sfd);

void connect_to_socket(int sfd, const struct sockaddr_in *server_address, socklen_t address_len);

int accept_client(int sfd, struct sockaddr_in *client_address, socklen_t address_len);

void send_message_to(int sfd, const Package *package, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);

void get_message_from(int sfd, Package *package, int flags, struct sockaddr *dest_addr, socklen_t dest_len);

#endif  // SOCKETS_H_
