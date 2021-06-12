#include <sys/ipc.h>
#include <sys/msg.h>


#define MSGSIZE 100
#define KEYFILE_PATH  "keyfilepath"
#define READ_ID 'R'
#define MSGQ_OK 0
#define MSGQ_NG -1


#define KEYFILE_PATH  "keyfilepath"
#define WRITE_ID 'W'



typedef struct msgbuff{
    long mtype;
    char mtext[MSGSIZE];
  }message;