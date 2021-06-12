#ifndef LIFT_CTRL_H
#define LIFT_CTRL_H

#include "lib/sensor.h"
#include "lib/messageQueue.h"
#include "lib/command.h"
#include "lib/request.h"
#include "lib/commandQueue.h"

/** get Sensor, request from message Queue */
void msgQueuesListeningLoop();

/** Get sensor from queue. Return 1 if sucess or 0 if no sensor received*/
int getSensorFromMsgQueue(MessageQueue messageQueue, Sensor *sensorReceived);

/** Get request object from queue. Return 1 if sucess or 0 if no request received */
// int getRequestObjectFromMsgQueue(MessageQueue messageQueue, Request* requestReceived);

/** Get request string from queue. Return 1 if sucess or 0 if no request received */
int getRequestStringFromMsgQueue(MessageQueue messageQueue, Request* requestReceived);

/** Get CompletedCommand from queue. Return 1 if completed standby command or 0  otherwise*/
int getCompletedCommandFromMsgQueue(MessageQueue messageQueue);

/* delete msgQueues on keyboard interuption */
void terminateHandler();

/** Process sensor signal upon receiving*/
int processSensorSignal(Sensor sensor, Request request);

/** Process request upon receiving*/
void processRequestFromManager(Command *currentCommand, CommandQueue *commandQueue);

/** Send commands to body. Return 0 or -1*/
int sendStandByCommandToBody(); //sleep for 1 sec and stop
int sendStopCommandToBody();
int sendUpCommandToBody();
int sendDownCommandToBody();

int sendCommandToBodyByType(int commandType);
int sendCommandToBody(Command command);
#endif