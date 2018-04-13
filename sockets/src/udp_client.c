#include <sys/time.h>
#include "../sockets/sockets.h"
#include "../utils/utils.h"

void get_rtt(Header *header);
void resolve(char* name, Header *header);

int main(int argc, char *argv[]){
	int const IP_INDEX = argc - 2;
	int const PORT_INDEX = argc - 1;
	int const FILE_NAME_INDEX = 0;
	int const OPERATION_INDEX = 1;

	int sfd = create_socket(AF_INET, SOCK_DGRAM, 0);
	set_timeout(sfd, SEC_TO_TIMEOUT);

	struct sockaddr_in server_address = get_server_address(argv[IP_INDEX], argv[PORT_INDEX]);

	Package package;
	Header header = {sfd, &package, 0, (struct sockaddr*) &server_address, sizeof(struct sockaddr_in)};

	if(strcmp(argv[OPERATION_INDEX],"rtt")==0){
		if(argc == 4){
			for(int i = 0; i < 10;i++){
				get_rtt(&header);
			}
		}
		else{
			fprintf(stderr, "Tente: %s [operacao] [ip] [porta]\n", argv[FILE_NAME_INDEX]); 
			exit(1);
		}
	}
	else if(strcmp(argv[OPERATION_INDEX], "rsv") == 0){
		if(argc == 5){
			resolve(argv[2], &header);		
		}
		else{
			fprintf(stderr, "Tente: %s [operacao] [url] [ip] [porta]\n", argv[FILE_NAME_INDEX]); 
			exit(1);
		}
	}

	return 0;
}


void get_rtt(Header *header){
	header->package->operation = RTT_OPERATION;

	struct timeval initial_time, final_time; 

	gettimeofday(&initial_time, NULL);
	send_message_to(header);

	strcpy(header->package->server_time, "#");

	get_message_from(header);
	gettimeofday(&final_time, NULL);

	if(strcmp(header->package->server_time, "#") == 0){
		strcpy(header->package->server_time, "Couldn't get the server time!");
	}

	long long total_sec = final_time.tv_sec * 1e6 + final_time.tv_usec - initial_time.tv_sec * 1e6 + initial_time.tv_usec;
	printf("%lld ms: %s\n", total_sec / (long long) 1e3, header->package->server_time);
}

void resolve(char* name, Header *header){
	header->package->operation = RSV_OPERATION;

	strcpy(header->package->url, name);
	send_message_to(header);
	get_message_from(header);

	if(strcmp(header->package->ip,"X")!=0){
		printf("%s: %s\n", name, header->package->ip);	
	}
	else{
		fprintf(stderr, "%s\n", "Rrsv: 1 Nao encontrado");
	}

}
