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
#include "queue.h"
#include "message.h"
#define SERV_PORT 1255
#define MAXLINE 255
#define BUSY 1
#define AVAI 0

int sockfd, n;
socklen_t len;
char mesg[MAXLINE];
struct sockaddr_in servaddr, cliaddr, cliaddr_1, cliaddr_2;
int state = 0;
//cliaddr for receiving incomming package
//cliaddr_1 for saving oPanel1
//cliaddr_2 for saving oPanel2

Queue requestQueue;

int msqid_write, msqid_read;
key_t keyx_write, keyx_read;

void saveClient(struct sockaddr_in cliad, int op)
{
  if (op == 1)
  {
    bzero(&cliaddr_1, sizeof(cliaddr_1));
    cliaddr_1.sin_family = AF_INET;
    cliaddr_1.sin_addr.s_addr = cliad.sin_addr.s_addr;
    cliaddr_1.sin_port = cliad.sin_port;
    return;
  }
  else if (op == 2)
  {
    bzero(&cliaddr_2, sizeof(cliaddr_2));
    cliaddr_2.sin_family = AF_INET;
    cliaddr_2.sin_addr.s_addr = cliad.sin_addr.s_addr;
    cliaddr_2.sin_port = cliad.sin_port;
    return;
  }
};

int checkEmptyClient(struct sockaddr_in addr)
{
  if (strstr(inet_ntoa(addr.sin_addr), "0.0.0.0") != NULL)
  {
    return 1;
  }
  else
    return 0;
}

int checkBothEmptyClient()
{
  if (checkEmptyClient(cliaddr_1))
  {
    return 1;
  }
  if (checkEmptyClient(cliaddr_2))
  {
    return 2;
  }
  return -1;
}

int checkExistingClient(struct sockaddr_in addr)
{
  if (cliaddr_2.sin_port == addr.sin_port)
    return 1;
  else if (cliaddr_1.sin_port == addr.sin_port)
    return 1;
  else
    return 0;
}
void printClient(struct sockaddr_in address)
{
  /* data */
  printf("IP: %s - port: %d!\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
}

void handleCallRequest(int from_floor,int dest_floor, struct sockaddr_in *client)
{
  eRequest *request = newRequest(from_floor,dest_floor, client);
  enQueue(request, &requestQueue);
}
void replyRequest(char *message, struct sockaddr_in client)
{
  int length = strlen(message) + 1;
  char *buf = (char *)malloc(sizeof(char) * length);
  strcat(buf, message);
  buf[length] = '\0';
  len = sizeof(client);
  n = strlen(message);
  sendto(sockfd, message, n, 0, (struct sockaddr *)&client, len);
  printf("Replied to %s ---->%s\n", inet_ntoa(client.sin_addr), message);
  free(buf);
}

void broadcast(char *message)
{
  replyRequest(message, cliaddr_1);
  replyRequest(message, cliaddr_2);
}

int sendMsg(eRequest *req)
{
  

  message *msg_buff = (message *)malloc(sizeof(message));
  sprintf(msg_buff->mtext,"%c", (req->destination_floor + '0'));
  msg_buff->mtype = 2; //from Manager to Ctrl is type 2
  if ((msgsnd(msqid_write, msg_buff, sizeof(msg_buff->mtext), IPC_NOWAIT)) < 0)
  {
    printf("%d, %ld, %s, %d\n", msqid_write, msg_buff->mtype, msg_buff->mtext, MSGSIZE);
    perror("msgsnd");
    // free(buf);
    free(msg_buff);
    return -1;
  }
  else
  {
    // free(buf);
    printf("Sent message to liftControl: %s\n",msg_buff->mtext);
    free(msg_buff);
    return 1;
  }
}

int recvMsg(char* buffer)
{
  //char* msg=(char *)malloc(sizeof(char)*MAXLINE);
  message *msg_buff = (message *)malloc(sizeof(message));
  //strcpy(msg_buff->mtext, msg);

  while (1)
  {
    if ((msgrcv(msqid_read, msg_buff, sizeof(msg_buff->mtext), 1, 0)) == MSGQ_NG) //receive msg from liftControl type 1
    {
      continue;
    }
    else
    {
      printf("processA(MessageReceive)  : received message = %s\n", msg_buff->mtext);
      strcpy(buffer, msg_buff->mtext);
      break;
    }
  }
  return 1;
}

int processRequest(eRequest *req)
{ //process the next request in the queue
  state = BUSY;
  req->status = 1;
  char * mesg =(char *)malloc(sizeof(char)*MAXLINE);
  sendMsg(req);
  //printf("break to trace!\n");
  recvMsg(mesg);
  if (strstr(mesg, "ARRIVE1") != NULL)
  {
    if(req->from_floor!=1)
    replyRequest("Elevator arrived! Wait for 3 seconds!", req->from);
    else broadcast("Elevator arrived! Wait for 3 seconds!");
  }
  memset(mesg, 0, MAXLINE);
  recvMsg(mesg);
  if (strstr(mesg, "ARRIVE2") != NULL)
  {
   broadcast("Elevator arrived! Ready for unload!");
  }
  req->status = 2;
  state = 0;
  return 0;
}


void signio_handler(int signo)
{
  char buff[1024];
  len = sizeof(cliaddr);

  int n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
  int check = checkBothEmptyClient();
  int from,to;
  
  sscanf(mesg,"%d %d",&to,&from);
  //printf("Receive a call to floor: %d\n",to);
  //printf("Check is %d!\n", check);
  //printClient(cliaddr_1);
  //printClient(cliaddr_2);

  if (check != -1)
  {
    if (check == 1 && checkExistingClient(cliaddr)==0)
      saveClient(cliaddr, 1);
    if (check == 2&& checkExistingClient(cliaddr)==0)
    {
      saveClient(cliaddr, 2);
    }
  }
  //printf("===============\n");
  //printClient(cliaddr_1);
  //printClient(cliaddr_2);
  //printf("Check existing client: %d\n", checkExistingClient(cliaddr));
  if (check == -1 && checkExistingClient(cliaddr) == 0)
  {

    replyRequest("Full Client", cliaddr);
    return;
  }
  //printf("TRACE22===========\n");
  handleCallRequest(from,to, &cliaddr);
  //printf("TRACE33===========\n");
  //printf("Received from client %s - %d  (%d bytes), content: %s\n", inet_ntoa(cliaddr.sin_addr), htons(cliaddr.sin_port), n, mesg);
  printf("Created a request to floor %d !\n", to);
  memset(mesg, 0, sizeof(mesg));
}

int checkState()
{
  return state;
}

void termHandler(int signo)
{
    //Control of message queue (message queue ID deletion)
    printf("Remove message queue!\n");
  if(msgctl(msqid_read, IPC_RMID, NULL) == MSGQ_NG){
    perror("msgctl:processA(MessageReceive)");
  }
  printf("Bye\n");

  exit(0);
}
int main()
{
  //refresh address
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  bzero(&cliaddr_1, sizeof(cliaddr_1));
  bzero(&cliaddr_2, sizeof(cliaddr_2));
  //setting server port
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
  {
    printf("Server is running at port %d\n", SERV_PORT);
  }
  else
  {
    perror("bind failed");
    return 0;
  }
  //signal(SIGINT, catch_ctrl_c_and_exit);
  signal(SIGINT, termHandler);

  //create a null queue
  makeNullQueue(&requestQueue);
  eRequest *currentReq = NULL;

  //setting sockfd to async mode
  if (fcntl(sockfd, F_SETFL, O_NONBLOCK | O_ASYNC))
    printf("Error in setting socket to async, nonblock mode");

  signal(SIGIO, signio_handler); // assign SIGIO to the handler

  //set this process to be the process owner for SIGIO signal
  if (fcntl(sockfd, F_SETOWN, getpid()) < 0)
    printf("Error in setting own to socket");

  //creating 2 message queue, 1 for WRITE and 1 for READ
  keyx_read = ftok("keyfilepath", 0);

  //  Message queue ID acquisition
  if ((msqid_read = msgget(keyx_read, IPC_CREAT | 0660)) == MSGQ_NG)
  {
    perror("msgget:processA(MessageReceive)");
    exit(1);
  }

  keyx_write = ftok("keyfilepath", 0);

  //  Message queue ID acquisition
  if ((msqid_write = msgget(keyx_write, 0)) == MSGQ_NG)
  {
    perror("msgget:processA(MessageReceive)");
    exit(1);
  }
  printf("MsgqID READ:%d - write private:%d\n",msqid_read,msqid_write);
  // forever for loop
  printf("=======================================================\n");
  printf("||            Lift Manager Stimulation               ||\n");
  printf("=======================================================\n");
  printf("Ready to receive elevator call!\n");
  for (;;)
  {
    if (checkState() != BUSY)
    {
      if(!emptyQueue(requestQueue)){
      currentReq = deQueue(&requestQueue);
      processRequest(currentReq);
      currentReq =NULL;
      }
    }
  }
  close(sockfd);
  return 0;
}
