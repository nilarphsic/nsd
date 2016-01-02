#include "nsd.h"

int main(int argc, char const *argv[])
{
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int s_Server;
	int status = 0;
	char buffer[1024];

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	s_Server = socket(AF_INET, SOCK_STREAM, 0);
	connect(s_Server, (struct sockaddr*)&server_addr, sizeof(server_addr));
	do{
		memset(buffer, '\0', sizeof(buffer));
		if(status){
			scanf("%[^\n]", buffer);
			fflush(stdin);
			send(s_Server, buffer, sizeof(buffer), 0);			
			status = !status;
		}else{
			recv(s_Server, buffer, sizeof(buffer), 0);
			if(!strcmp(buffer, "over")){
				status = !status;
				continue;
			}
			printf("%s", buffer);
			fflush(stdout);
		}		
	}while(strcmp(buffer, "quit"));
	close(s_Server);
	return 0;
}