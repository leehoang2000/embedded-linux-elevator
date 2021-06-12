#include "command.h"
#include <stdlib.h>

/** Command initializing functions*/
Command* createCommand(int commandType)
{
    Command* cmd = (Command *)malloc(sizeof(Command));
    cmd->commandType = commandType;
    cmd->isDone = 0;
    return cmd;
}

Command* createStopCommand()
{
    return createCommand(COMMAND_LIFT_STOP);
}
Command* createUpCommand()
{
    return createCommand(COMMAND_LIFT_UP);
}
Command* createDownCommand()
{
    return createCommand(COMMAND_LIFT_DOWN);
}
Command* createStandByCommand()
{
    return createCommand(COMMAND_LIFT_STANDBY);
}

char* toString(Command* command){
    switch (command->commandType)
    {
    case COMMAND_LIFT_UP:
        return "UP";
        break;
    
    case COMMAND_LIFT_DOWN:
        return "DOWN";
        break;

    case COMMAND_LIFT_STOP:
        return "STOP";
        break;

    case COMMAND_LIFT_STANDBY:
        return "STANDBY";
        break;

    default:
        return "";
        break;
    }
}