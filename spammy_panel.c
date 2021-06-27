#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define LIMIT 6
#define MAX_PORT_RETRY 10

int sockfd;
int MNG_PORT = 8080;
int MAXLINE = 5;
time_t t;


int getHash(int floor) {
	return floor;
}

int msg_store[LIMIT];

void cleanupEntry() {
	for (int i = 0; i < LIMIT; ++i)
	{
		msg_store[i] = -1;
	}
}

void addEntry(int floor) {
	int new_id = getHash(floor);
	if (msg_store[new_id] == -1)
	{
		msg_store[new_id] = floor;
		printf(" add %d %d\n", new_id, floor);
	}

}

void removeEntry(int floor) {
	msg_store[floor] = -1;
}


int readFloor(char*msg) {
	int floor = -1;
	if (msg)
	{
		sscanf( msg, "%3d", &floor);
	}
	return floor;
}



int isValidFloor(int floor) {
	return floor >= 0 && floor <= 5;
}


void signal_handle(int sign) {
	if (sockfd) {
		close(sockfd);
	}
	printf("Closing panel\n");
	exit(sign);
}

int getPort() {
	return rand() % 30000 + 8000;
}


void *listenToReply(void*_sendaddr) {
	const struct sockaddr * sendaddr = (const struct sockaddr *)_sendaddr;
	// struct sockaddr_in sendaddr = &(_sendaddr);

	int n;
	char buffer[MAXLINE];
	int received_floor = -1;

	struct sockaddr_in servaddr;
	int len = sizeof(servaddr);
	int _len = sizeof(*sendaddr);


	while (1) {
		memset(buffer, 0, MAXLINE);
		n = recvfrom(sockfd, (char *) buffer, MAXLINE,
		             MSG_WAITALL, (struct sockaddr *) &servaddr,
		             (&len));
		received_floor = readFloor(buffer);

		if (isValidFloor(received_floor))
		{
			printf("done %d\n", received_floor);
			removeEntry(received_floor);
		}

		for (int i = 0; i < LIMIT; ++i)
		{
			if (msg_store[i] != -1)
			{
				memset(buffer, 0, MAXLINE);
				sprintf(buffer, "%03d\n", msg_store[i]);
				sendto(sockfd, buffer, strlen(buffer),
				       MSG_CONFIRM, sendaddr,
				       len);
			}
		}
	}
}


int main() {
	srand(time(NULL));
	cleanupEntry();

	// handle force close
	signal(SIGINT, signal_handle);
	// create udp connection


	char buffer[MAXLINE];
	struct sockaddr_in servaddr, selfaddr;
	memset(&servaddr, 0, sizeof(servaddr));


	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(MNG_PORT);
	int len = sizeof(servaddr);


	int rPort = getPort();
	int status = -1;

	selfaddr.sin_family = AF_INET; // IPv4
	selfaddr.sin_addr.s_addr = INADDR_ANY;
	selfaddr.sin_port = htons(rPort);

	for (int i = 0; i < MAX_PORT_RETRY; ++i)
	{
		status = bind(sockfd, (const struct sockaddr *) &selfaddr,
		              sizeof(selfaddr));
		if ( status < 0) {
			printf("Bind to %d failed, retry count %d\n", rPort, i);
			rPort = getPort();
		} else {
			printf("Binded to %d\n", rPort);
			break;
		}
	}

	if (status < 0)
	{
		perror("Failed binding to port");
		exit(1);
	}

// set timeout
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = pow(10, 5);

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		perror("Error");
	}

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, listenToReply, (void*) &servaddr);


	int target_floor = -1;


	while (1) {
		target_floor = -1;
		fflush(stdin);

		printf("Input a floor: ");
		if (scanf("%d", &target_floor) == 0) {
			printf("Invalid input\n");
			break;
		}

		if (isValidFloor(target_floor))
		{
			printf("OK %d\n", target_floor);
			addEntry(target_floor);
		} else {
			printf("not ok %d\n", target_floor);
		}
	}

	return 0;
}