#include "../sockets/sockets.h"
#include "../utils/utils.h"
#include <sys/time.h>

int main(int argc, char *argv[]){
	if(argc == 3){
		int sfd = create_socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in server_address = get_server_address(argv[1], argv[2]);

        connect_to_socket(sfd, &server_address, sizeof(server_address));

    struct timeval initial_time, final_time; 
		Package package;
    gettimeofday(&initial_time,NULL);
    
		send_message_to(sfd, &package, sizeof(Package), 0);
		printf("Package sent to server! Waiting for result...\n");

		get_message_from(sfd, &package, sizeof(Package), 0);
    gettimeofday(&final_time,NULL);
		printf("Package was sent from server at: %s", package.server_time);
		printf("Package was sent in %ld microseconds\n", final_time.tv_usec -
                                                     initial_time.tv_usec);
	} else {
		fprintf(stderr, "Wrong format. Try: \"%s [IP] [Port]\"\n", argv[0]);
		return 1;
	}

	return 0;
}
