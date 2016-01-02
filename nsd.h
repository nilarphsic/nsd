#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define DESKLEN 2*52

#define SERVER_PORT 5555

struct gamer_t{
	int *desks;
	int pointer, pos;
};
struct desk_t{
	int *deks;
	int pos;
};

void *gameFlow(void *client);
void inputS(char *buffer, void *client);
void toString(char *buffer, struct gamer_t *object, int mask);

void gamerGain(struct gamer_t *object, int point);
void buildGamer(struct gamer_t *object);
void resetGamerInfo(struct gamer_t *object);

int deskDeal(struct desk_t *object);
void buildDesk(struct desk_t *object);
void resetDesk(struct desk_t *object);
void shuffle(int *object, int length);