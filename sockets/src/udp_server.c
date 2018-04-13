#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include "../sockets/sockets.h"
#include <netdb.h>
#include <stdlib.h>

#define IP_INDEX 1
#define PORT_INDEX 2
#define FILE_NAME_INDEX 0


void rtt(Header *header);
void resolve(Header *header);

int main(int argc, char *argv[]){
	if (argc == 3){
		int sfd = create_socket(AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in server_address = get_server_address(argv[IP_INDEX], argv[PORT_INDEX]);

		bind_socket(sfd, (struct sockaddr *) &server_address, sizeof(server_address));

		while(1){
			printf("Listening to %s:%s. Waiting for messages...\n", argv[IP_INDEX], argv[PORT_INDEX]);

			struct sockaddr_in client_address;

			Package package;
			Header header = {sfd, &package, 0, (struct sockaddr*) &client_address, sizeof(struct sockaddr_in)};
			header.package = (Package *) malloc(sizeof(Package));

			get_message_from(&header);

			if(header.package->operation == RTT_OPERATION){
				rtt(&header);
				printf("Answering to %s:%u at %s!\n\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), header.package->server_time);
			}
			else if(header.package->operation == RSV_OPERATION){
				resolve(&header);
			}

			send_message_to(&header);
			free(header.package);
		}
	} else {
		fprintf(stderr, "Wrong format. Try: \"%s [IP] [Port]\"\n", argv[FILE_NAME_INDEX]);

		return 1;
	}

	return 0;
}

void rtt(Header* header){
	strcpy(header->package->server_time, get_formated_time());

	// To force timeout
	// int aux = 0; for(int i = 0; i < SEC_TO_TIMEOUT * 1e9; i++){++aux; --aux;}
}

void resolve(Header *header){
	struct addrinfo hints, *res;

	hints.ai_socktype = 0;
	hints.ai_family = AF_INET;
	hints.ai_flags = 0;

	int error = getaddrinfo(header->package->url, NULL, &hints, &res);

	if(error != 0){
		if(error != EAI_NONAME){
			strcpy(header->package->ip, gai_strerror(error));
			fprintf(stderr, "%s\n", gai_strerror(error));
		}
		else{
			strcpy(header->package->ip,"X");
		}

		send_message_to(header);
		exit(1);
	}

	strcpy(header->package->ip, inet_ntoa(((struct sockaddr_in *)(res->ai_addr))->sin_addr));
	freeaddrinfo(res);
}

