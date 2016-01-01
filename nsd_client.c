#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>

#define CONNECT_PORT 5555

void *thread_recv(void *server){
	char buffer[1024];
	do{
		bzero(&buffer, sizeof(buffer));
		recv((intptr_t)server, buffer, sizeof(buffer), 0);
		fputs(buffer, stdout);
	}while(strcmp(buffer, "quit\n"));
}

int main(int argc, char const *argv[]){
	struct sockaddr_in server_addr;
	int s_Server;
	int status = 0;
	pthread_t pthread_recv;
	char buffer[1024];

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(CONNECT_PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	s_Server = socket(AF_INET, SOCK_STREAM, 0);
	connect(s_Server, (struct sockaddr*)&server_addr, sizeof(server_addr));

	pthread_create(&pthread_recv, NULL, (void*)*thread_recv, (void*)(intptr_t)s_Server);
	do{
		bzero(&buffer, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		send(s_Server, buffer, sizeof(buffer), 0);
	}while(strcmp(buffer, "quit\n"));
	pthread_join(pthread_recv, NULL);

	close(s_Server);
	return 0;
}