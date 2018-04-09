#include "sockets.h"

struct sockaddr_in get_server_address(char *ip, char *port){
	struct sockaddr_in socket_address;

	socket_address.sin_family = AF_INET;
	socket_address.sin_addr.s_addr = inet_addr(ip);
	socket_address.sin_port = htons(atoi(port));

	return socket_address;
}

struct sockaddr_in get_client_address(){
	struct sockaddr_in socket_address;

	socket_address.sin_family = AF_INET;
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_address.sin_port = htons(0);

	return socket_address;
}

int create_socket(int domain, int type, int protocol){
	int sfd = socket(domain, type, protocol);

	if(sfd < 0){
		perror("Couldn't create socket");
		exit(1);
	}

	return sfd;
}

void bind_socket(int socket, const struct sockaddr *address, socklen_t address_len){
	int error = bind(socket, address, address_len);

	if(error){
		perror("Couldn't bind socket to name");
		exit(1);
	}
}

void connect_to_socket(int sfd, const struct sockaddr_in *server_address, socklen_t address_len){
    if (connect(sfd, (const struct sockaddr *) server_address, address_len) < 0) {
        perror("Couldn't connect with server");
        exit(1);
    }
}

void send_message_to(Header *header){
    int bytes_sended = sendto(header->sfd, header->package, sizeof(Package), header->flags, header->address, header->dest_len);

    if (bytes_sended < 0)
        perror("Couldn't send package");
}

void get_message_from(Header *header){
  int bytes_received = recvfrom(header->sfd, header->package, sizeof(Package), header->flags, header->address, &(header->dest_len));

	if (bytes_received < 0){
		perror("Couldn't receive package");	
	}
}
