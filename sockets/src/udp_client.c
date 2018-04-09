#include <sys/time.h>
#include "../sockets/sockets.h"
#include "../utils/utils.h"

int main(int argc, char *argv[]){
	if(argc == 3){
		int sfd = create_socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in server_address = get_server_address(argv[1], argv[2]);

		Package package;
		struct timeval initial_time, final_time; 
		gettimeofday(&initial_time, NULL);

        send_message_to(sfd, &package, 0, (struct sockaddr *) &server_address, sizeof(Package));
		printf("Package sent to server! Waiting for result...\n");

        get_message_from(sfd, &package, 0, (struct sockaddr *) &server_address, sizeof(Package));
		gettimeofday(&final_time, NULL);

		printf("Package was sent from server at: %s", package.server_time);
		printf("Package was sent in %ld microseconds\n", final_time.tv_usec - initial_time.tv_usec);
	} else {
		fprintf(stderr, "Wrong format. Try: \"%s [IP] [Port]\"\n", argv[0]);
		return 1;
	}

	return 0;
}
