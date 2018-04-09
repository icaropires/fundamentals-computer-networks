#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "../sockets/sockets.h"

void handle_interrupt(int signal);

int ACCEPTED_CLIENT;

int main(int argc, char *argv[]){
	signal(SIGINT, handle_interrupt);
	signal(SIGTERM, handle_interrupt);
	signal(SIGABRT, handle_interrupt);

	if (argc == 3){
		int sfd = create_socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in server_address = get_server_address(argv[1], argv[2]);

		bind_socket(sfd, (struct sockaddr *) &server_address, sizeof(server_address));

		while(1){
			struct sockaddr_in client_address;

			// printf("Listening to %s:%s. Waiting for messages...\n", argv[1], argv[2]);

			Package package;
			Header header = {sfd, &package, 0, (struct sockaddr*) &client_address, sizeof(Package)};
			get_message_from(&header);

			strcpy(package.server_time, get_formated_time());

			// To force timeout
			// int aux = 0; for(int i = 0; i < SEC_TO_TIMEOUT * 1e9; i++){++aux; --aux;}

			send_message_to(&header);

			// printf("Package sended back to client %s:%u!\n\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
		}
	} else {
		fprintf(stderr, "Wrong format. Try: \"%s [IP] [Port]\"\n", argv[0]);
		return 1;
	}

	return 0;
}

void handle_interrupt(int signal){
	fprintf(stderr, "\nClosing connection and exitting...\n");
	close(ACCEPTED_CLIENT);
}
