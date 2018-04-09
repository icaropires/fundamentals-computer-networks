#include <sys/time.h>
#include "../sockets/sockets.h"
#include "../utils/utils.h"

void get_rtt(Header *header){

	struct timeval initial_time, final_time; 

	gettimeofday(&initial_time, NULL);
	send_message_to(header);
	printf("Package sent to server! Waiting for result...\n\n");

	get_message_from(header);
	gettimeofday(&final_time, NULL);

	printf("%ld μs: %s\n",final_time.tv_usec - initial_time.tv_usec, header->package->server_time);

}

int main(int argc, char *argv[]){
	if(argc == 3){
		int sfd = create_socket(AF_INET, SOCK_DGRAM, 0);
		struct sockaddr_in server_address = get_server_address(argv[1], argv[2]);

		Package package;
		Header header = {sfd, &package, 0, (struct sockaddr*) &server_address, sizeof(Package)};
		for(int i = 0;i < 10;i++){
			get_rtt(&header);
		}
	} else {
		fprintf(stderr, "Wrong format. Try: \"%s [IP] [Port]\"\n", argv[0]);
		return 1;
	}

	return 0;
}
