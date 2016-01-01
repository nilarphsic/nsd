#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>

#define CONNECT_PORT 5555

void *thread_send(void *client){
	char buffer[1024];
	do{
		bzero(&buffer, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		send((intptr_t)client, buffer, sizeof(buffer), 0);
	}while(strcmp(buffer, "quit\n"));
}

void *thread_recv(void *client){
	char buffer[1024];
	do{
		bzero(&buffer, sizeof(buffer));
		recv((intptr_t)client, buffer, sizeof(buffer), 0);
		fputs(buffer, stdout);
	}while(strcmp(buffer, "quit\n"));
}

int main(int argc, char const *argv[]){
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int s_Server;
	int s_Client;
	pthread_t pthread_send;
	pthread_t pthread_recv;
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(CONNECT_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	s_Server = socket(AF_INET, SOCK_STREAM, 0);
	bind(s_Server, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(s_Server, 5);
	
	while(1){
		s_Client = accept(s_Server,	(struct sockaddr*)&client_addr, (socklen_t*)sizeof(client_addr));
		pthread_create(&pthread_send, NULL, (void*)*thread_send, (void*)(intptr_t)s_Client);
		pthread_create(&pthread_recv, NULL, (void*)*thread_recv, (void*)(intptr_t)s_Client);
	}
	
	close(s_Client);
	close(s_Server);
	return 0;
}