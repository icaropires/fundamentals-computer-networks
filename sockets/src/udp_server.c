#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "../sockets/sockets.h"

int main(int argc, char *argv[]){
	if (argc == 3){
		int sfd = create_socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in server_address = get_server_address(argv[1], argv[2]);

		bind_socket(sfd, (struct sockaddr *) &server_address, sizeof(server_address));

		while(1){
			struct sockaddr_in client_address;

			printf("Listening to %s:%s. Waiting for messages...\n", argv[1], argv[2]);

			Package package;
			Header header = {sfd, &package, 0, (struct sockaddr*) &client_address, sizeof(Package)};
			get_message_from(&header);

			char server_time[8];
			strcpy(server_time, get_formated_time());

			strcpy(package.server_time, server_time);

			// To force timeout
			// int aux = 0; for(int i = 0; i < SEC_TO_TIMEOUT * 1e9; i++){++aux; --aux;}

			send_message_to(&header);

			printf("Answering to %s:%u at %s!\n\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), server_time);
		}
	} else {
		fprintf(stderr, "Wrong format. Try: \"%s [IP] [Port]\"\n", argv[0]);
		return 1;
	}

	return 0;
}
