#ifndef LIFT_SENSOR_H
#define LIFT_SENSOR_H

/*
Height watch of lift
ON/OFF of the position sensing device in each floor and controlling the rise error detection sensor.
Notifying the ON/OFF change of each sensor to liftCtrl by interrupt.
*/

#include "lib/sensor.h"

/* delete msqQueue on keyboard interuption */
void sigIntHandler();

/* Create sensors for floors and rising sensor*/
void initSensorList(Sensor *sensorList, int length);

void listenForHeightFromBodyLoop(double *height, Sensor *sensorList, int length);

#endif