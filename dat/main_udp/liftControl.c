#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "message.h"

int main(void)
{
  struct msgbuff
  {
    long mtype;
    char mtext[MSGSIZE];
  } message;

  int msqid_write, msqid_read;
  key_t keyx_read, keyx_write;
  struct msqid_ds msq;
  //gennerate key
  printf("processA(MessageReceive)  : start\n");
  keyx_read = ftok("keyfilepath2", (int)99);

  //  Message queue ID acquisition
  if ((msqid_read = msgget(keyx_read, IPC_CREAT | 0660)) == MSGQ_NG)
  {
    perror("msgget:processA(MessageReceive)");
    exit(1);
  }
    printf("processA(MessageRead Private)  : msqID = %d\n", msqid_read);

  // ========================================
  //write message queue
  keyx_write = ftok("xxx", 19);

  //  Message queue ID acquisition
  if ((msqid_write = msgget(keyx_write | IPC_PRIVATE,  IPC_CREAT | 0660)) == MSGQ_NG)
  {
    perror("msgget:processA(MessageReceive)");
    exit(1);
  }
  printf("processA(MessageWrite)  : msqID = %d\n", msqid_write);
  // Send a message.
  //=============================================
  message.mtype = 1;
  //strcpy(message.mtext,"This is a test message!");
  while (1)
  {
    if ((msgrcv(msqid_read, &message, sizeof(message.mtext), 2, 0)) ==
        MSGQ_NG)
    {
      continue;
    }
    else
    {
      printf("processA(MessageReceive)  : received message = %s\n", message.mtext);
      //break;

      scanf("%s", message.mtext);
      message.mtype = 1;
      if ((msgsnd(msqid_write, &message, MSGSIZE, IPC_NOWAIT)) < 0)
      {
        printf("%d, %ld, %s, %d\n", msqid_write, message.mtype, message.mtext, MSGSIZE);
        perror("msgsnd");
        exit(1);
      }
      else
        printf("Message: \"%s\" Sent\n", message.mtext);

      //printf("processA(MessageSent)  : received message = %s\n", message.mtext);
      //break;
      memset(message.mtext, 0, sizeof(message.mtext));
      scanf("%s", message.mtext);
      message.mtype = 1;
      if ((msgsnd(msqid_write, &message, MSGSIZE, IPC_NOWAIT)) < 0)
      {
        printf("%d, %ld, %s, %d\n", msqid_write, message.mtype, message.mtext, MSGSIZE);
        perror("msgsnd");
        exit(1);
      }
      else
        printf("Message: \"%s\" Sent\n", message.mtext);
      memset(message.mtext, 0, sizeof(message.mtext));
      //printf("processA(MessageSent)  : msqID = %d\n", msqid_write);
    }
  }
  //   // Receive message
  //   if((msgrcv(msqid, &message, sizeof(message.mtext), 1, 0)) ==
  //      MSGQ_NG){
  //     perror("msgrcv:processA(MessageReceive)");
  //     exit(1);
  //   }
  //   printf("processA(MessageReceive)  : received message = %s\n", message.mtext);

  //   //Control of message queue (message queue ID deletion)
  //   if(msgctl(msqid, IPC_RMID, NULL) == MSGQ_NG){
  //     perror("msgctl:processA(MessageReceive)");
  //   }

  //   printf("processA(MessageReceive)  : end\n");
  return 0;
}
