#include <stdio.h>
#include <sys/fcntl.h>
#include<unistd.h>

#include "sensor.h"
#include "messageConfig.h"
#include "messageQueue.h"

/** Sensors for 1-5th floor */
Sensor createFloorSensor(int floorNumber){
    Sensor sensor;
    sensor.floorNumber = floorNumber;

    //set height: first floor sensor can detect [0.5,1.5], then add 3m each floor
    sensor.detectionHeightMin = 0.5 + 3*(floorNumber - 1);
    sensor.detectionHeightMax = 1.5 + 3*(floorNumber - 1);

    //set state
    if(floorNumber == 1){
        sensor.state = SENSOR_ON;
        sensor.prevState = SENSOR_ON;
    } else{
        sensor.state = SENSOR_OFF;
        sensor.prevState = SENSOR_OFF;
    }

    return sensor;
}

/** Given height, operate all the sensors in a list */
void operateSensors(double height, Sensor* sensorList, int length){
    //printf("Operating sensors with height: %.2f\n", height); //DEBUG
    for (int i = 0; i < length; i++)
    {
        operateSensor(height, &sensorList[i]);
    }
    
}

/** 1. Check height against a sensor
 * 2. Change state of a sensor
 * 3. Broadcast to liftCtrl when state changes
*/
void operateSensor(double height, Sensor* sensor){
    int isOn = checkHeightSensor(height, *sensor);

    //store prevState
    sensor->prevState = sensor->state;

    if(isOn){
        sensor->state = SENSOR_ON;
        if(sensor->prevState == SENSOR_OFF){
            printSensorStateChange(*sensor);            
            sendSensorToLiftCtrl(sensor);
        }
    } else{
        sensor->state = SENSOR_OFF;
        if(sensor->prevState == SENSOR_ON){
            printSensorStateChange(*sensor);
            sendSensorToLiftCtrl(sensor);
        }
    }
}

/** Print state change of sensor: OFF from ON or ON from OFF */
void printSensorStateChange(Sensor sensor){
    //print sensor number
    switch (sensor.floorNumber)
    {
    case RISING_SENSOR_FLOOR:
        printf("Rising floor sensor: ");
        break;
    
    default:
        printf("Sensor floor %d: ", sensor.floorNumber);
        break;
    }
    //print state change
    if(sensor.prevState == SENSOR_OFF) //ON FROM OFF
        printf("ON from OFF\n");
    else if (sensor.prevState == SENSOR_ON) //OFF FROM ON
        printf("OFF from ON\n");
    
    fflush(stdout);
}

/** Send the sensor struct to liftCtrl via msgQueue*/
int sendSensorToLiftCtrl(Sensor *sensor){
    MessageQueue messageQueue = getMessageQueue(KEY_FILE_PATH_SENSOR_TO_CTRL, SENSOR_TO_CTRL);
    Message *message = createSensorMessage(*sensor);

    sendMessageToQueue(messageQueue, message);
}

/** Boolean: check if height is within a sensor's range. Return 0(false) or 1(true)*/
int checkHeightSensor(double height, Sensor sensor){
    //printf("Checking sensor %d with height %.2f\n", sensor.floorNumber, height); fflush(stdout); //DEBUG
    
    if( (sensor.detectionHeightMax >= height) && (height >= sensor.detectionHeightMin))
        return 1;
    else
        return 0;
}

/** Sensors for rising error */
Sensor createRiseErrorSensor(){
    Sensor sensor;
    sensor.floorNumber = RISING_SENSOR_FLOOR;

    sensor.detectionHeightMax = 14;
    sensor.detectionHeightMin = 13.5;

    sensor.state = SENSOR_OFF;
    sensor.prevState = SENSOR_OFF;

    return sensor;
}