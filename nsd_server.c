#include "nsd.h"

int main(int argc, char const *argv[])
{
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int s_Server;
	int s_Client;
	socklen_t len = sizeof(client_addr);
	pthread_t pthread_sub;

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	s_Server = socket(AF_INET, SOCK_STREAM, 0);
	bind(s_Server, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(s_Server, 5);

	while(1){
		s_Client = accept(s_Server,	(struct sockaddr*)&client_addr, &len);
		pthread_create(&pthread_sub, NULL, (void*)*gameFlow, (void*)(intptr_t)s_Client);
	}

	close(s_Client);
	close(s_Server);
	return 0;
}

void *gameFlow(void *client){
	struct desk_t deskCtrl;
	struct gamer_t pServer;
	struct gamer_t pClient;
	char buffer[1024];
	int gaming = 1;
	int serverRedeal = 0;

	buildDesk(&deskCtrl);
	buildGamer(&pServer);
	buildGamer(&pClient);

	while(1){
		gamerGain(&pServer, deskDeal(&deskCtrl));
		gamerGain(&pServer, deskDeal(&deskCtrl));
		gamerGain(&pClient, deskDeal(&deskCtrl));
		gamerGain(&pClient, deskDeal(&deskCtrl));

		if(gaming && pServer.pointer == 21){
			toString(buffer, &pServer, 0);
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			toString(buffer, &pClient, 0);
			strcat(buffer, "\nServer win,\n");
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			gaming = 0;
		}

		while(gaming && pClient.pointer < 21){
			toString(buffer, &pServer, 1);
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			toString(buffer, &pClient, 0);
			strcat(buffer, "\nDoes client need deal?[y|n]:");
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			memset(buffer, '\0', sizeof(buffer));
			inputS(buffer, client);
			if(buffer[0] == 'y'){
				gamerGain(&pClient, deskDeal(&deskCtrl));
				if(pClient.pointer > 21){
					toString(buffer, &pServer, 0);
					send((intptr_t)client, buffer, sizeof(buffer), 0);
					toString(buffer, &pClient, 0);
					strcat(buffer, "\nServer win,\n");
					send((intptr_t)client, buffer, sizeof(buffer), 0);
					gaming = 0;
				}else if(pClient.pointer == 21)	break;
			}else break;
		}

		while(gaming && pServer.pointer < 15){
			memset(buffer, '\0', sizeof(buffer));
			if(!serverRedeal){
				strcat(buffer, "Server deal...");
			}else{
				strcat(buffer, "...");
			}
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			serverRedeal = 1;
			gamerGain(&pServer, deskDeal(&deskCtrl));
			if(pServer.pointer >= 21){
				memset(buffer, '\0', sizeof(buffer));
				strcat(buffer, "\n");
				send((intptr_t)client, buffer, sizeof(buffer), 0);
				toString(buffer, &pServer, 0);
				send((intptr_t)client, buffer, sizeof(buffer), 0);
				toString(buffer, &pClient, 0);
				if(pServer.pointer == 21)
					strcat(buffer, "\nServer win.\n");
				else
					strcat(buffer, "\nClinet win.\n");
				send((intptr_t)client, buffer, sizeof(buffer), 0);
				gaming = 0;
			}
		}
		if(serverRedeal && gaming){
			memset(buffer, '\0', sizeof(buffer));
			strcat(buffer, "\n");
			send((intptr_t)client, buffer, sizeof(buffer), 0);
		}

		while(serverRedeal && gaming && pClient.pointer < 21){
			toString(buffer, &pServer, 1);
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			toString(buffer, &pClient, 0);
			strcat(buffer, "\nDoes client need deal?[y|n]:");
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			memset(buffer, '\0', sizeof(buffer));
			inputS(buffer, client);
			if(buffer[0] == 'y'){
				gamerGain(&pClient, deskDeal(&deskCtrl));
				if(pClient.pointer > 21){
					toString(buffer, &pServer, 0);
					send((intptr_t)client, buffer, sizeof(buffer), 0);
					toString(buffer, &pClient, 0);
					strcat(buffer, "\nServer win,\n");
					send((intptr_t)client, buffer, sizeof(buffer), 0);
					gaming = 0;
				}else if(pClient.pointer == 21)	break;
			}else break;
		}

		if(gaming){
			toString(buffer, &pServer, 0);
			send((intptr_t)client, buffer, sizeof(buffer), 0);
			toString(buffer, &pClient, 0);
			if(pServer.pointer >= pClient.pointer)
				strcat(buffer, "\nServer win.\n");
			else
				strcat(buffer, "\nClient win.\n");
			send((intptr_t)client, buffer, sizeof(buffer), 0);
		}

		memset(buffer, '\0', sizeof(buffer));
		strcat(buffer, "Replay again?[y|n]:");
		send((intptr_t)client, buffer, sizeof(buffer), 0);
		memset(buffer, '\0', sizeof(buffer));
		inputS(buffer, client);
		if(buffer[0] != 'y')	break;
		resetGamerInfo(&pServer);
		resetGamerInfo(&pClient);
		gaming = 1;
		serverRedeal = 0;
	}
}

void inputS(char *buffer, void *client){
	memset(buffer, '\0', sizeof(buffer));
	strcat(buffer, "over");
	send((intptr_t)client, buffer, sizeof(buffer), 0);
	memset(buffer, '\0', sizeof(buffer));
	recv((intptr_t)client, buffer, sizeof(buffer), 0);
}

void toString(char *buffer, struct gamer_t *object, int mask){
	int i;
	memset(buffer, '\0', sizeof(buffer));
	*buffer = '[';
	for(i = 0; i < (*object).pos; i++){
		if((mask) & (i == 0)){
			strcat(buffer, "X");
		}else{
			char temp[9];
			memset(temp, '\0', sizeof(temp));
			sprintf(temp, "%d", *((*object).desks + i));
			strcat(buffer, temp);
		}
		if(i + 1 != (*object).pos)	strcat(buffer, ",");
	}
	strcat(buffer, "]");
}

void gamerGain(struct gamer_t *object, int point){
	int aCount = (*object).pointer = 0, i;
	*((*object).desks + (*object).pos++) = point;
	for(i = 0; i < (*object).pos; i++)
		if(*((*object).desks + i) == 1){
			aCount++;
			(*object).pointer += 11;
		}else if(*((*object).desks + i) >= 10){
			(*object).pointer += 10;
		}else
			(*object).pointer += *((*object).desks + i);
	while((*object).pointer > 21 && aCount--)
		(*object).pointer -= 10;
}

void buildGamer(struct gamer_t *object){
	(*object).desks = (int*)malloc(22 * sizeof(int));
	resetGamerInfo(object);
}

void resetGamerInfo(struct gamer_t *object){
	memset((*object).desks, '\0', sizeof(22 * sizeof(int)));
	(*object).pointer = 0;
	(*object).pos = 0;
}

int deskDeal(struct desk_t *object){
	if((*object).pos >= DESKLEN)
		resetDesk(object);
	return *((*object).deks + (*object).pos++);
}

void buildDesk(struct desk_t *object){
	int i, j;
	srand(time(NULL));
	(*object).deks = (int*)malloc(DESKLEN * sizeof(int));
	for(i = 0; i < DESKLEN;)
		for(j = 1; j <= 13; j++)
			(*object).deks[i++] = j;
	resetDesk(object);
}

void resetDesk(struct desk_t *object){
	shuffle((*object).deks, DESKLEN);
	(*object).pos = 0;
}

void shuffle(int *object, int length){
	int i = length, j, x;
	while(i){
		j = rand() % i;
		x = object[--i];
		object[i] = object[j];
		object[j] = x;
	}
}
