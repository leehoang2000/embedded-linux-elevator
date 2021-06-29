#ifndef MESSAGE_CONFIG_H
#define MESSAGE_CONFIG_H

/** define Message Queue param */

#define KEY_FILE_PATH_BODY_TO_SENSOR "./liftSensor"
#define KEY_FILE_PATH_SENSOR_TO_CTRL "./liftCtrl"
#define KEY_FILE_PATH_CTRL_TO_BODY "./liftBody"
// #define KEY_FILE_PATH_MNG_TO_CTRL "keyfilepath"
// #define KEY_FILE_PATH_CTRL_TO_MNG "keyfilepath"
#define KEY_FILE_PATH_MNG_TO_CTRL "./liftCtrl"
#define KEY_FILE_PATH_CTRL_TO_MNG "./liftMng"

#define BODY_TO_SENSOR 1
#define SENSOR_TO_CTRL 2
#define CTRL_TO_BODY 3
#define BODY_TO_CTRL 4
#define MNG_TO_CTRL 0
#define CTRL_TO_MNG 0

#endif