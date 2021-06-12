#ifndef COMMAND_H
#define COMMAND_H
/** Command object is used in communication between liftCtrl and liftBody 
 * commandType: specify movement for the lift (UP, DOWN, STOP, STANDBY)
 * isDone (bool): whether the command has been done
*/

/** COMMAND type for structure command*/
#define COMMAND_LIFT_STOP 0
#define COMMAND_LIFT_UP 1
#define COMMAND_LIFT_DOWN 2
#define COMMAND_LIFT_STANDBY 3

/** struct */
typedef struct Command{
    int commandType;
    int isDone;
} Command;

/** Command initializing functions*/
Command* createStopCommand();
Command* createUpCommand();
Command* createDownCommand();
Command* createStandByCommand();
Command* createCommand(int commandType);

char* toString(Command* command);
#endif