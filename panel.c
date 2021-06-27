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

int sockfd;
int MNG_PORT = 8080;
int MAXLINE = 5;
time_t t;


int readFloor(char*msg) {
	int floor = -1;
	sscanf( msg, "%3d", &floor);
	return floor;
}

int getRandomNumber() {
	return rand() % (int)pow(10, 9);
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


int main() {
	// handle force close
	signal(SIGINT, signal_handle);
	// create udp connection


	char buffer[MAXLINE];
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));


	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(MNG_PORT);

	int len = sizeof(servaddr);


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
			memset(buffer, 0, MAXLINE);
			sprintf(buffer, "%03d\n", target_floor);
			printf("Result: %s\n", buffer);
			sendto(sockfd, buffer, strlen(buffer),
			       MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			       len);
		} else {
			printf("not ok %d\n", target_floor);
		}
	}
}