#ifndef LIFT_BODY_H
#define LIFT_BODY_H

#include "lib/commandQueue.h"

/* Height maintenance of lift
Movement (up/down/stop) control of main body of lift
*/

#define VELO 1.5 //constant speel 0.5m/s
#define FPS 30 //frames per second of the simulation
#define PRINT_PER_FRAME 6 //print n times per second

/** States of the lift */
#define STATE_WORKING 0
#define STATE_WAITING_TO_STOP 1
#define STATE_STANDING_BY 2

/** get time*/
double getCurrentTime();

/* maintain FPS */
void mainLoop(double *height, double *velocity); 

/* Set velocity*/
void up(double *velocity); 
void down(double *velocity);
void stop(double *velocity);

/** Process signal got from liftCtrl*/
void processControlCommand(Command controlCommand, double* velocity);

/** Print height. If velocity if 0, don't print*/
void printHeightPerNFrame(int* iterations, double height, double velocity);

/* Send height to sensor. Return 0 or -1*/
int sendHeightToSensor(double height);

/** send back data to liftCtrl about standby command*/
int sendCompletedStandByCommandMessage();

/* delete msqQueue on keyboard interuption */
void terminateHandler();

/* process standby signal */
void sigAlarmHandler();
#endif