#include <sys/time.h>
#include "../sockets/sockets.h"
#include "../utils/utils.h"

void get_rtt(Header *header);

int main(int argc, char *argv[]){
	if(argc == 3){
		int sfd = create_socket(AF_INET, SOCK_DGRAM, 0);
		set_timeout(sfd, SEC_TO_TIMEOUT);

		struct sockaddr_in server_address = get_server_address(argv[1], argv[2]);

		Package package;
		Header header = {sfd, &package, 0, (struct sockaddr*) &server_address, sizeof(Package)};

		for(int i = 0; i < 10;i++){
			get_rtt(&header);
		}
	} else {
		fprintf(stderr, "Wrong format. Try: \"%s [IP] [Port]\"\n", argv[0]);
		return 1;
	}

	return 0;
}

void get_rtt(Header *header){
	struct timeval initial_time, final_time; 

	gettimeofday(&initial_time, NULL);
	send_message_to(header);
	// printf("Package sent to server! Waiting for result...\n\n");

	strcpy(header->package->server_time, "#");

	get_message_from(header);
	gettimeofday(&final_time, NULL);

	if(strcmp(header->package->server_time, "#") == 0){
		strcpy(header->package->server_time, "Couldn't get the server time!");
	}

	long long total_sec = final_time.tv_sec * 1e6 + final_time.tv_usec - initial_time.tv_sec * 1e6 + initial_time.tv_usec;
	printf("%lld ms: %s\n\n", total_sec / (long long) 1e3, header->package->server_time);
}
