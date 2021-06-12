#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#define SERV_PORT 1255
#define MAXLINE 250

int sockfd, n, from_len;
struct sockaddr_in servaddr, from_socket;
socklen_t addrlen = sizeof(from_socket);
int this_floor = 2;
void signio_handler(int signo)
{
  char buff[MAXLINE];
  int n = recvfrom(sockfd, buff, MAXLINE, 0, (struct sockaddr *)&servaddr, &addrlen);
  //if (n>0) // if SIGIO is generated by a data arrival
  printf("Received from server (%d bytes), content: %s\n", n, buff);
  memset(buff, 0, sizeof(buff));
}

void genMsg(int floor, char *msg)
{
  int length = 11;
  char *buf = (char *)malloc(sizeof(char) * (length + 1));
  sprintf(buf, "%c %c", (floor + '0'), (floor + '0'));
  strcpy(msg, buf);
  free(buf);
}
int checkFloor(int floor)
{
  if (floor > this_floor && floor < 6)
  {
    return 1;
  }
  else
    return 0;
}
int main()
{
  //   int sockfd, n, from_len;
  //   struct sockaddr_in servaddr, from_socket;
  //  socklen_t addrlen = sizeof(from_socket);
  char sendline[MAXLINE], recvline[MAXLINE + 1];
  FILE *fp;
  char buf[10];
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  //  if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
  // 		perror("bind failed");
  // 		return 0;
  // 	}
  printf("creat socket\n");
  signal(SIGIO, signio_handler); // assign SIGIO to the handler

  if (fcntl(sockfd, F_SETFL, O_NONBLOCK | O_ASYNC))
    printf("Error in setting socket to async, nonblock mode");

  //set this process to be the process owner for SIGIO signal
  if (fcntl(sockfd, F_SETOWN, getpid()) < 0)
    printf("Error in setting own to socket");
  printf("=======================================================\n");
  printf("||            Lift Opanel Stimulation                ||\n");
  printf("=======================================================\n");
  printf("Current floor: %d\n", this_floor);
  printf("Please enter the floor you wish: ");
  while (fgets(sendline, MAXLINE, stdin) != NULL)
  {
    //printf("\n");
    genMsg(atoi(sendline), buf);
    if (checkFloor(atoi(sendline)))
    {
      printf("To Lift Manager: %s\n", buf);
      sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    }
    else
      printf("Please enter a valid floor : 2-5!\n");
    memset(buf, 0, 10);
    printf("Please enter the floor you wish: ");
  }
}