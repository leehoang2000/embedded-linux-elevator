#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

/** Command queue object is stored in liftCtrl, to manage commands 
 * Instruction: must call makeQueueEmpty(queue); before use.
*/

#include "command.h"

/** Structures */
typedef Command* CommandNode;

typedef struct nodeQ {
  CommandNode elQ;
  struct nodeQ *nextQ;
  struct nodeQ *prevQ;
}nodeQ;

typedef struct {
  nodeQ *front,*rear;
}CommandQueue;


/** CommandQueue functions */
void makeQueueEmpty(CommandQueue *q);
int isQueueEmpty(CommandQueue q);
int isQueueFull(CommandQueue q);
void enQueue(CommandNode x,CommandQueue *q);
CommandNode deQueue(CommandQueue *q);

#endif