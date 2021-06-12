#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

/* Template for a Message Queue. More convinient to use than default msgQueue of UNIX */

#include "sensor.h"
#include "commandQueue.h"
#include "command.h"
#include "request.h"

typedef struct MessageQueue{
    int messageQueueID;
} MessageQueue;

typedef struct Message{
    long mtype; //1 if forward client-server; 2 is backwards server-client
    
    enum {
      typeFloat,
      typeInt,
      typeDouble,
      typeSensor,
      typeCommand,
      typeRequest,
    } dataType;

    union
    {
        int n;
        float f; 
        double d;
        Sensor sensor;
        Command command;
        Request request;
    } value;

} Message;

#define MSGSIZE 100

#define MSGQ_OK 0
#define MSGQ_NG -1

/** Create/get the messageQueue with a specific file path and ID*/
MessageQueue getMessageQueue(char* keyFilePath, int ID);


/** Send a string through a message queue, return 0 or -1*/
int sendStringThroughMessageQueue(MessageQueue queue, char* string);

/** Receive a string from a message queue, return 0 or -1*/
int receiveStringFromQueue(MessageQueue queue, char* stringReceived);

/** Send an object through the queueonce. Return 0 or -1*/
int sendMessageToQueue(MessageQueue queue, Message* message);

/** Get an object from the queue once. Return 0 or -1*/
int receiveMessageFromQueue(MessageQueue queue, Message* message);

/** Delete a MessageQueue. Return 0 or -1*/
int deleteMessageQueue(MessageQueue queue);

/** Creating Messages */
Message* createIntMessage(int number);
Message* createFloatMessage(float f);
Message* createDoubleMessage(double d);
Message* createSensorMessage(Sensor sensor);
Message* createCommandMessage(Command command);
Message* createRequestMessage(Request request);

/** Get Message value. Return -1 if error */
int getIntFromMessage(Message message);
float getFloatFromMessage(Message message);
double getDoubleFromMessage(Message message);
Sensor getSensorFromMessage(Message message);
Command getCommandFromMessage(Message message);
Request getRequestFromMessage(Message message);


/** Trick: completedStanbyCommand as an int*/
#define COMPLETED_STANDBY_COMMAND_MSG -100

#endif