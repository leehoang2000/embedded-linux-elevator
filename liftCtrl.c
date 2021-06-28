#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#include "liftCtrl.h"
#include "lib/messageConfig.h"
#include "liftBody.h"

int isKeyboardInterrupted = 0;

int main()
{
    Sensor sensor;

    //delete msgQueue on keyboard interuption
    signal(SIGINT, terminateHandler);

    //loop: listen for sensors from liftSensor
    msgQueuesListeningLoop();
}

/* delete msgQueue on keyboard interuption */
void terminateHandler()
{
    isKeyboardInterrupted = 1;
}

/** get Sensor, request from message Queue */
void msgQueuesListeningLoop()
{
    MessageQueue msgQueueSensorToCtrl = getMessageQueue(KEY_FILE_PATH_SENSOR_TO_CTRL, SENSOR_TO_CTRL);
    MessageQueue msgQueueMngToCtrl = getMessageQueue(KEY_FILE_PATH_MNG_TO_CTRL, MNG_TO_CTRL);
    MessageQueue msgQueueBodyToCtrl = getMessageQueue(KEY_FILE_PATH_CTRL_TO_BODY, BODY_TO_CTRL);

    Sensor sensorReceived;
    Request requestReceived;

    Command *currentCommandPtr; //store current command
    CommandQueue commandQueue; //store queue of commands
    makeQueueEmpty(&commandQueue);

    //listening loop
    while (1)
    {
        //delelte message queue
        if (isKeyboardInterrupted)
        {
            //interupted, delete queues
            deleteMessageQueue(msgQueueSensorToCtrl);
            printf("Queue %d deleted\n", msgQueueSensorToCtrl.messageQueueID);
            deleteMessageQueue(msgQueueMngToCtrl);
            printf("Queue %d deleted\n", msgQueueMngToCtrl.messageQueueID);
            deleteMessageQueue(msgQueueBodyToCtrl);
            printf("Queue %d deleted\n", msgQueueBodyToCtrl.messageQueueID);
            break;
        }
        else
        {
            //listening for lift sensors
            if(getSensorFromMsgQueue(msgQueueSensorToCtrl, &sensorReceived) == 1){
                //check if sensor is ON FROM OFF of floor 1/requested floor
                if(processSensorSignal(sensorReceived, requestReceived) == 1){ 
                    //sendNextCommandInQueue(&commandQueue, &currentCommand);
                    currentCommandPtr = deQueue(&commandQueue);
                    sendCommandToBody(*currentCommandPtr);
                };
            }

            //listening for manager's requests
            // if(getRequestObjectFromMsgQueue(msgQueueMngToCtrl, &requestReceived) == 1 )
            //     processRequestFromManager(currentCommandPtr, &commandQueue);
            if(getRequestStringFromMsgQueue(msgQueueMngToCtrl, &requestReceived) == 1 )
                processRequestFromManager(currentCommandPtr, &commandQueue);

            //listening for completedStandbyRequest
            if( getCompletedCommandFromMsgQueue(msgQueueBodyToCtrl) == 1){
                if(!isQueueEmpty(commandQueue)){
                    currentCommandPtr = deQueue(&commandQueue);
                    sendCommandToBody(*currentCommandPtr);
                } else{
                    //TODO: arrived to 1st floor, send msg ARRIVE2 to liftMng to notify
                    printf("Completed request to floor %d\n", requestReceived.floorNumber);
                    MessageQueue queue = getMessageQueue(KEY_FILE_PATH_CTRL_TO_MNG, CTRL_TO_MNG);
                    sendStringThroughMessageQueue(queue, "ARRIVE2");
                }
            };
        }
    }
}

/** Get CompletedCommand from queue. Return 1 if completed standby command or 0  otherwise*/
int getCompletedCommandFromMsgQueue(MessageQueue messageQueue){
    Message messageReceived;
    if (receiveMessageFromQueue(messageQueue, &messageReceived) == -1) //nothing in queue
    {
        return 0;
    }
    else //message got
    {
        int completedCommand = getIntFromMessage(messageReceived);
        //printf("Got a completed command from body: %d\n", completedCommand);fflush(stdout); //DEBUG

        if(completedCommand == COMPLETED_STANDBY_COMMAND_MSG){
            return 1;
        }
        return 0;
    }
}

/** Get request object from queue. Return 1 if sucess or 0 if no request received */
/*
int getRequestObjectFromMsgQueue(MessageQueue messageQueue, Request* requestReceived){
    Message messageReceived;
    if (receiveMessageFromQueue(messageQueue, &messageReceived) == -1) //nothing in queue
    {
        return 0;
    }
    else //message got
    {
        *requestReceived = getRequestFromMessage(messageReceived);
        printf("Got a Request to floor %d\n", requestReceived->floorNumber);
        fflush(stdout); //DEBUG
        return 1;
    }
}
*/

/** Get request string from queue. Return 1 if sucess or 0 if no request received */
int getRequestStringFromMsgQueue(MessageQueue messageQueue, Request* requestReceived){
    char stringReceived[MSGSIZE];
    if(receiveStringFromQueue(messageQueue, stringReceived) == -1){
        return 0;
    } else{ //got a string
        int len = strlen(stringReceived);
        //printf("String %s has length %d\n", stringReceived, len);
        //atoi string, assign new request
        int requestFloorNumber = atoi(stringReceived);
        *requestReceived = createRequest(requestFloorNumber);
        printf("Got a Request to floor %d\n", requestFloorNumber);
        fflush(stdout); //DEBUG
        return 1;
    }
}

/** Get sensor from queue. Return 1 if sucess or 0 if no sensor received*/
int getSensorFromMsgQueue(MessageQueue messageQueue, Sensor *sensorReceived)
{
    Message messageReceived;
    //get message
    if (receiveMessageFromQueue(messageQueue, &messageReceived) == -1) //nothing in queue
    {
        return 0;
    }
    else //message got
    {
        *sensorReceived = getSensorFromMessage(messageReceived);
        //printf("Got a sensor signal on floor %d\n", sensorReceived->floorNumber);   fflush(stdout); //DEBUG        
        return 1;
    }
}

/** Process request upon receiving*/
void processRequestFromManager(Command *currentCommand, CommandQueue *commandQueue){
    //if command queue is not empty, then does not process the request
    if(!isQueueEmpty(*commandQueue)){
        printf("CommandQueue non empty.\n");
        return;
    }

    //command queue empty:
    printf("CommandQueue empty. Decoding new commands\n");
    
    //Decode request into {UP,STANDBY,DOWN,STANDBY}. Add to queue.
    Command* upCommand = createUpCommand();
    Command* standByCommand1 = createStandByCommand();
    Command* downCommand = createDownCommand();
    Command* standByCommand2 = createStandByCommand();

    enQueue(upCommand, commandQueue);
    enQueue(standByCommand1, commandQueue);
    enQueue(downCommand, commandQueue);
    enQueue(standByCommand2, commandQueue);

    //send first command
    currentCommand = deQueue(commandQueue);
    sendCommandToBody(*currentCommand);
}


/** Process sensor signal upon receiving*/
int processSensorSignal(Sensor sensor, Request request)
{
    //control evelator to stop when receive ceiling sensor
    if (sensor.floorNumber == RISING_SENSOR_FLOOR)
    {
        if (sensor.state == SENSOR_ON)
        { //ON FROM OFF
            sendStopCommandToBody();
        }
        return 0;
    }
    else
    {             
        //printf("Sensor processing. Floor requested: %d\n", request.floorNumber);   
        //if lift moves to floor 1 or requested floor
        if (sensor.floorNumber == request.floorNumber || sensor.floorNumber == 1)
        {
            //TODO: arrived to requested floor, send MSG ARRIVE1 to liftMng
            if(sensor.floorNumber == request.floorNumber && sensor.state == SENSOR_ON){
                MessageQueue queue = getMessageQueue(KEY_FILE_PATH_CTRL_TO_MNG, CTRL_TO_MNG);
                sendStringThroughMessageQueue(queue, "ARRIVE1");
            }
            
            //floor 1-5 signal: Only cares about ON FROM OFF signal
            if (sensor.state == SENSOR_ON)
            {
                return 1;
            }
            return 0;
        }
        return 0;
    }
}
/** Send command to body by command type. Return 0 or -1*/
int sendCommandToBodyByType(int commandType)
{
    Command* command = createCommand(commandType);
    return sendCommandToBody(*command);
}

int sendCommandToBody(Command command)
{   
    printf("Send command to body: %s\n", toString(&command)); //debug
    MessageQueue msgQueue = getMessageQueue(KEY_FILE_PATH_CTRL_TO_BODY, CTRL_TO_BODY);
    Message *message = createCommandMessage(command);
    return sendMessageToQueue(msgQueue, message);
}

int sendStopCommandToBody()
{
    printf("Send STOP command to liftBody\n");
    return sendCommandToBodyByType(COMMAND_LIFT_STOP);
}
int sendUpCommandToBody()
{
    printf("Send UP command to liftBody\n");
    return sendCommandToBodyByType(COMMAND_LIFT_UP);
}
int sendDownCommandToBody()
{
    printf("Send DOWN command to liftBody\n");
    return sendCommandToBodyByType(COMMAND_LIFT_DOWN);
}

//sleep for 1 sec and stop
int sendStandByCommandToBody()
{
    printf("Send STAND BY command to liftBody\n");
    return sendCommandToBodyByType(COMMAND_LIFT_STANDBY);
}