#include <time.h>
#include <stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/time.h>
#include <sys/fcntl.h>

#include "liftBody.h"
#include "lib/messageConfig.h"
#include "lib/messageQueue.h"

int isKeyboardInterrupted = 0;
int liftState = STATE_WORKING;
int hasAlarmed = 0; //has there any alarm?

/* Height maintenance of lift
Movement (up/down/stop) control of main body of lift
*/

// PRINT_PER_FRAME should be FRAME_PER_PRINT 
int main(){
    double height; //height of lift: distance from lift's bottom to the ground
    // Value range: [0;14] due to the height of 15m of the building; and lift's own height is 1m

    double velocity;
    
    //default: lift is in first floor, standby
    height = 1;
    velocity = 0;
    printf("Height: 1 m, standing by\n");

    //bind signals to handler 
    signal(SIGINT, terminateHandler);
    signal(SIGALRM, sigAlarmHandler);

    //run Loop
    mainLoop(&height, &velocity);
}

//main event loop: pass height, velocity by reference to modify its value
void mainLoop(double *height, double *velocity){
    double time;
    double timeSinceLastFrame = 0;
    int iterations = 0;

    MessageQueue queueCtrlToBody = getMessageQueue(KEY_FILE_PATH_CTRL_TO_BODY, CTRL_TO_BODY);
    Message messageReceived;

    while(1){
        //loop for every 1/FPS        
        while(timeSinceLastFrame < 1.0/FPS){
            time = getCurrentTime();  
            time = getCurrentTime() - time; 
            timeSinceLastFrame += time;
        }
        //the following statements are executed per frame

        //print height. Increase iteration. 
        printHeightPerNFrame(&iterations, (*height), (*velocity));

        //change height
        (*height) += ((*velocity) * timeSinceLastFrame);

        //process STANDBY command
        if(liftState == STATE_STANDING_BY && hasAlarmed == 0){ //just from waiting to stop to standing by
            stop(velocity);
            hasAlarmed = 1;
            printf("Standing by for 3 seconds\n");
            alarm(3);
        }

        //reset time frame
        timeSinceLastFrame = 0;

        //send height to sensor
        sendHeightToSensor(*height);

        //listen for signal from Ctrl
        if (isKeyboardInterrupted){
            //interupted, delete queue
            deleteMessageQueue(queueCtrlToBody);
            printf("Queue %d deleted\n", queueCtrlToBody.messageQueueID);
            break;
        }
        else{
            //get message control to body
            if (receiveMessageFromQueue(queueCtrlToBody, &messageReceived) == -1) 
            { 
                continue;
            }
            else //get the command from message
            {
                Command controlCommand = getCommandFromMessage(messageReceived);
                //printf("Queue %d received signal = %d\n", queueCtrlToBody.messageQueueID, controlCommand);
                processControlCommand(controlCommand, velocity);
            }
        }

    }   

}

/** Process signal got from liftCtrl*/
void processControlCommand(Command controlCommand, double* velocity){
    printf("Received %s command\n", toString(&controlCommand));

    switch (controlCommand.commandType)
    {
    case COMMAND_LIFT_UP:
        up(velocity);
        break;
    
    case COMMAND_LIFT_DOWN:
        down(velocity);
        break;

    case COMMAND_LIFT_STOP:
        stop(velocity);
        break;

    case COMMAND_LIFT_STANDBY: //wait for 1 second then stop
        alarm(1); 
        liftState = STATE_WAITING_TO_STOP;
        break;

    default:
        break;
    }
}

/* Send height to sensor. Return 0 or -1*/
int sendHeightToSensor(double height){
    //msgQueue
    MessageQueue msgQueue = getMessageQueue(KEY_FILE_PATH_BODY_TO_SENSOR, BODY_TO_SENSOR);
    Message *message;

    message = createDoubleMessage(height);
    return sendMessageToQueue(msgQueue, message);
}

/* Get curremt timestamp in seconds using gettimeofday*/
double getCurrentTime(){
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    double result = (tv.tv_sec)  + (tv.tv_usec) / 1000000.0;
    return result;
}

/* Set velocity*/
void up(double *velocity){
    printf("Evelator moving up\n");
    *velocity = VELO;
}; 
void down(double *velocity){
    printf("Evelator moving down\n");
    *velocity = -VELO;
};
void stop(double *velocity){
    printf("Evelator stops\n");
    *velocity = 0;
};

/** Print height. If velocity = 0, don't print */
void printHeightPerNFrame(int* iterations, double height, double velocity){
    (*iterations)++;
    if( (*iterations) % PRINT_PER_FRAME == 0)
        if(velocity != 0)
            printf("Height: %.2f m\n", height);
}

/* delete msgQueue on keyboard interuption */
void terminateHandler(){
    isKeyboardInterrupted = 1;
}

/* process standby signal */
void sigAlarmHandler(){
    hasAlarmed = 0;

    switch (liftState){
    case STATE_WAITING_TO_STOP :
        //stop();
        liftState = STATE_STANDING_BY;
        break;
    case STATE_STANDING_BY :
        sendCompletedStandByCommandMessage();
        liftState = STATE_WORKING;
        break;
    default:
        break;
    }
}

/** send back data to liftCtrl about standby command*/
int sendCompletedStandByCommandMessage(){
    MessageQueue msgQueue = getMessageQueue(KEY_FILE_PATH_CTRL_TO_BODY, BODY_TO_CTRL);
    Message* msg = createIntMessage(COMPLETED_STANDBY_COMMAND_MSG);
    return sendMessageToQueue(msgQueue,msg);
}