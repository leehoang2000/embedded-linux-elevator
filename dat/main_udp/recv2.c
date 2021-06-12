#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGSIZE 100
#define KEYFILE_PATH  "keyfilepath"
#define ID 'M'
#define MSGQ_OK 0
#define MSGQ_NG -1

int main(void)
{
  struct msgbuff{
    long mtype;
    char mtext[MSGSIZE];
  }message;

  int           msqid;
  key_t         keyx;
  struct msqid_ds msq;
  
  printf("processA(MessageReceive)  : start\n");
    keyx = ftok(KEYFILE_PATH, (int)ID);

//  Message queue ID acquisition
  if((msqid = msgget(keyx, IPC_CREAT|0660)) == MSGQ_NG){
    perror("msgget:processA(MessageReceive)");
    exit(1);
  }
  printf("processA(MessageReceive)  : msqID = %d\n", msqid);
// Send a message.
//   message.mtype=1;
//   strcpy(message.mtext,"This is a test message!");
// 	if((msgsnd(msqid, &message, MSGSIZE, IPC_NOWAIT)) < 0){
// 		printf("%ld, %ld, %s, %d\n", msqid, message.mtype,message.mtext,  MSGSIZE);
// 		perror("msgsnd");
// 		exit(1);
// 	}
// 	else
// 		printf("Message: \"%s\" Sent\n", message.mtext);
//   printf("processA(MessageReceive)  : msqID = %d\n", msqid);
  // Receive message
//   if((msgrcv(msqid, &message, sizeof(message.mtext), 1, 0)) ==
//      MSGQ_NG){
//     perror("msgrcv:processA(MessageReceive)");
//     exit(1);
//   }
//   printf("processA(MessageReceive)  : received message = %s\n", message.mtext);
    printf("waiting for message!\n");
 while(1){
     if((msgrcv(msqid, &message, sizeof(message.mtext), 1, 0)) ==
     MSGQ_NG){
         continue;
     }
     else{
         printf("processA(MessageReceive)  : received message = %s\n", message.mtext);
         //break;
     }
 }
  //Control of message queue (message queue ID deletion)
  if(msgctl(msqid, IPC_RMID, NULL) == MSGQ_NG){
    perror("msgctl:processA(MessageReceive)");
  }

  printf("processA(MessageReceive)  : end\n");
  return 0;
}
