#ifndef SENSOR_H
#define SENSOR_H

//states of a sensor
#define SENSOR_ON 1
#define SENSOR_OFF 0

//rising sensor floor
#define RISING_SENSOR_FLOOR 0

typedef struct Sensor{
    int floorNumber;
    int state;
    int prevState;

    //[detectionHeightMin, detectionHeightMax] is the interval that turns the sensor on
    double detectionHeightMax; 
    double detectionHeightMin;
} Sensor;

Sensor createFloorSensor(int floorNumber);
Sensor createRiseErrorSensor();

/** Given height, operate all the sensors in a list */
void operateSensors(double height, Sensor* sensorList, int length);

/** 1. Check height against a sensor
 * 2. Change state of a sensor
 * 3. Broadcast to liftCtrl when state changes
*/
void operateSensor(double height, Sensor *sensor);

/** Boolean: check if height is within a sensor's range. Return 0(false) or 1(true)*/
int checkHeightSensor(double height, Sensor sensor);

/** Print state change of sensor: OFF from ON or ON from OFF */
void printSensorStateChange(Sensor sensor);

/** Send the sensor struct to liftCtrl via FIFO. Return 0 or -1*/
int sendSensorToLiftCtrl(Sensor *sensor);

void listenForHeightFromBody(double *height, Sensor *sensorList, int length);


#endif