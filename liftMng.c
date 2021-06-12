#include "liftMng.h"
#include "lib/messageQueue.h"
#include "lib/messageConfig.h"
#include "lib/request.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>

int isKeyboardInterrupted = 0;

int main(){
    //sendWithMessageQueueObject();
    MessageQueue msgQueue = getMessageQueue(KEY_FILE_PATH_MNG_TO_CTRL, MNG_TO_CTRL);
    // sendStringThroughMessageQueue(msgQueue, "3");

    //delete msgQueue on keyboard interuption
    signal(SIGINT, terminateHandler);

    listeningLoop();
}

/** listen for ARRIVE1, ARRIVE2 from liftCtrl*/
void listeningLoop(){
    MessageQueue msgQueueCtrlToMng = getMessageQueue(KEY_FILE_PATH_CTRL_TO_MNG, CTRL_TO_MNG);
    char stringReceived[MSGSIZE];

    while(1){
        //delelte message queue
        if (isKeyboardInterrupted)
        {
            //interupted, delete queues
            deleteMessageQueue(msgQueueCtrlToMng);
            printf("Queue %d deleted\n", msgQueueCtrlToMng.messageQueueID);
            break;
        }
        else
        {
            if(receiveStringFromQueue(msgQueueCtrlToMng, stringReceived) == 0) {
                printf("Mng receives message from Ctrl:%s\n", stringReceived);
            }
        }
    }
}

void sendWithMessageQueueObject(){
     /** Mock liftMng to send request (by floor number) to liftCtrl */
    MessageQueue msgQueue = getMessageQueue(KEY_FILE_PATH_MNG_TO_CTRL, MNG_TO_CTRL);
    //printf("liftMng init message queue id %d\n", msgQueue.messageQueueID); //debug
    Request requestToFloor3 = createRequest(3);
    Message *messageToFloor3 = createRequestMessage(requestToFloor3);
    sendMessageToQueue(msgQueue, messageToFloor3);
}

/* delete msgQueue on keyboard interuption */
void terminateHandler()
{
    isKeyboardInterrupted = 1;
}