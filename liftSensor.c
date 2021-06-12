#include<sys/time.h>
#include<signal.h>
#include<unistd.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "liftSensor.h"
#include "lib/messageConfig.h"
#include "lib/messageQueue.h"

int isKeyboardInterrupted = 0;

/*
Height watch of lift
ON/OFF of the position sensing device in each floor and controlling the rise error detection sensor.
Notifying the ON/OFF change of each sensor to liftCtrl by interrupt.
*/
int main(){
    double height;

    //init sensors
    Sensor sensorList[6];
    int length = 6;

    initSensorList(sensorList, length);
    
    //delete MsgQueue on keyboard interuption
    signal(SIGINT, sigIntHandler);

    //start listening for FIFO from liftBody
    listenForHeightFromBodyLoop(&height, sensorList, length);  
}

/* delete msgQueue on keyboard interuption */
void sigIntHandler(){
    isKeyboardInterrupted = 1;
}

/* Create sensors for floors and rising sensor*/
void initSensorList(Sensor *sensorList, int length){
    for (int i = 0; i < length-1; i++)
    {
        sensorList[i] = createFloorSensor(i+1);
    }
    sensorList[length-1] = createRiseErrorSensor();
}

void listenForHeightFromBodyLoop(double *height, Sensor *sensorList, int length){
    MessageQueue queue = getMessageQueue(KEY_FILE_PATH_BODY_TO_SENSOR, BODY_TO_SENSOR);
    Message messageReceived;

    while(1){
        //MsgQueue
        if (isKeyboardInterrupted)
        {
            //interupted, delete queue
            deleteMessageQueue(queue);
            printf("Queue %d deleted\n", queue.messageQueueID);
            break;
        }
        else
        {
            //get message
            if (receiveMessageFromQueue(queue, &messageReceived) == -1) //nothing in queue
            {
                continue;
            }
            else //message got
            {
                *height = getDoubleFromMessage(messageReceived);
                //printf("Queue %d received height = %.2f\n", queue.messageQueueID, (*height) );
                operateSensors( (*height), sensorList, length);
            }
        }
    }
}