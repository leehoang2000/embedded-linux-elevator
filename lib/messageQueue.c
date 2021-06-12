#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "messageQueue.h"

/** Create/get the messageQueue with a specific file path and ID*/
MessageQueue getMessageQueue(char *keyFilePath, int ID)
{
    key_t keyx = ftok(keyFilePath, ID);
    int msqid;

    //Message queue ID acquisition
    if ((msqid = msgget(keyx, IPC_CREAT | 0660)) == MSGQ_NG) //err
    {
        perror("msgget:processA(MessageReceive)");
        return;
    }
    else
    { //success
        MessageQueue queue;
        queue.messageQueueID = msqid;
        return queue;
    }
}


/** Send a string through a message queue, return 0 or -1*/
int sendStringThroughMessageQueue(MessageQueue queue, char* string){
    struct msgbuff
    {
        long mtype;
        char mtext[MSGSIZE];
    } message;
    
    message.mtype = 1;
    strcpy(message.mtext, string);

     if ((msgsnd(queue.messageQueueID, &message, sizeof(message.mtext), IPC_NOWAIT)) < 0) //err
    {
        perror("msgsnd");
        return -1;
    }
    else
        return 0;
}

/** Receive a string from a message queue, return 0 or -1*/
int receiveStringFromQueue(MessageQueue queue, char* stringReceived){
    struct msgbuff
    {
        long mtype;
        char mtext[MSGSIZE];
    } message;

    if (msgrcv(queue.messageQueueID, &message, sizeof(message.mtext), 0, IPC_NOWAIT) == MSGQ_NG){
        return -1;
    } else{
        //printf("message.mtext: %s\n", message.mtext);
        int len = strlen(message.mtext);
        //printf("len:%d\n",len);
        strcpy(stringReceived, message.mtext);
        //printf("strcpy\n");
        stringReceived[len] = '\0'; //terminated
        //printf("string terminated\n");
        return 0;
    }
}
/** Send an object through the queueonce. Return 0 or -1*/
int sendMessageToQueue(MessageQueue queue, Message* message)
{
    if ((msgsnd(queue.messageQueueID, message, sizeof(Message), IPC_NOWAIT)) < 0) //err
    {
        perror("msgsnd");
        return -1;
    }
    else
        return 0;
}


/** Get an object from the queue once. Return 0 or -1*/
int receiveMessageFromQueue(MessageQueue queue, Message* message)
{
    if (msgrcv(queue.messageQueueID, message, sizeof(Message), 1, IPC_NOWAIT) == MSGQ_NG) //NO WAIT
    //if (msgrcv(queue.messageQueueID, message, sizeof(Message), 1, 0) == MSGQ_NG) //BLOCKING
    { //err
        return -1;
    }
    else{
        //receive message
        return 0;
    }
}

/** Delete a MessageQueue. Return 0 or -1*/
int deleteMessageQueue(MessageQueue queue)
{
    //Control of message queue (message queue ID deletion)
    if (msgctl(queue.messageQueueID, IPC_RMID, NULL) == MSGQ_NG)
    {
        perror("msgctl");
        return -1;
    }
    else
        return 0;
}
/** Creating Messages */
Message* createIntMessage(int number){
    Message* msg = malloc(sizeof(Message));
    msg->mtype = 1;
    msg->dataType = typeInt;
    msg->value.n = number;
    return msg;
}

Message* createFloatMessage(float f){
    Message* msg = malloc(sizeof(Message));
    msg->mtype = 1;
    msg->dataType = typeFloat;
    msg->value.f = f;
    return msg;
}

Message* createDoubleMessage(double d){
    Message* msg = malloc(sizeof(Message));
    msg->mtype = 1;
    msg->dataType = typeDouble;
    msg->value.d = d;
    return msg;
}

Message* createSensorMessage(Sensor sensor){
    Message* msg = malloc(sizeof(Message));
    msg->mtype = 1;
    msg->dataType = typeSensor;
    msg->value.sensor = sensor;
    return msg;
}

Message* createCommandMessage(Command command){
    Message* msg = malloc(sizeof(Message));
    msg->mtype = 1;
    msg->dataType = typeCommand;
    msg->value.command = command;
    return msg;
}

Message* createRequestMessage(Request request){
    Message* msg = malloc(sizeof(Message));
    msg->mtype = 1;
    msg->dataType = typeRequest;
    msg->value.request = request;
    return msg;
}
/** Get Message value. Return -1 if error*/
int getIntFromMessage(Message message){
    if(message.dataType == typeInt)
        return message.value.n;
    else return -1;
}
float getFloatFromMessage(Message message){
    if(message.dataType == typeFloat)
        return message.value.f;
    else return -1;
}
double getDoubleFromMessage(Message message){
    if(message.dataType == typeDouble)
        return message.value.d;
    else return -1;
}

Sensor getSensorFromMessage(Message message){
    if(message.dataType == typeSensor)
        return message.value.sensor;
    else return;
}

Command getCommandFromMessage(Message message){
    if(message.dataType == typeCommand)
        return message.value.command;
    else return;
}

Request getRequestFromMessage(Message message){
    if(message.dataType == typeRequest)
        return message.value.request;
    else return;
}