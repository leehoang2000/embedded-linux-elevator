#define maxQueueSize 10

#include <stdio.h>
#include <stdlib.h>
#include "commandQueue.h"

/** CommandQueue functions */
void makeQueueEmpty(CommandQueue *q)
{
    q->front = NULL;
    q->rear = NULL;
}

int isQueueEmpty(CommandQueue q)
{
    return (q.front == NULL && q.rear == NULL);
}

int isQueueFull(CommandQueue q)
{
    nodeQ *c = q.front;
    int i = 0;
    while (c != NULL)
    {
        c = c->nextQ;
        ++i;
    }
    return i == maxQueueSize;
}

void enQueue(CommandNode x, CommandQueue *q)
{
    if (isQueueFull(*q))
        printf("CommandQueue is full!\n");
    else
    {
        nodeQ *new_node = (nodeQ *)malloc(sizeof(nodeQ));
        new_node->elQ = x;
        if (isQueueEmpty(*q))
        {
            new_node->nextQ = NULL;
            new_node->prevQ = NULL;
            q->front = new_node;
            q->rear = new_node;
        }
        else
        {
            q->rear->nextQ = new_node;
            new_node->prevQ = q->rear;
            new_node->nextQ = NULL;
            q->rear = new_node;
        }
    }
}

CommandNode deQueue(CommandQueue *q)
{
    if (!isQueueEmpty(*q))
    {
        nodeQ *t;
        CommandNode x;
        t = q->front;
        x = t->elQ;
        if (q->front == q->rear)
        {
            q->front = NULL;
            q->rear = NULL;
        }
        else
        {
            q->front = q->front->nextQ;
            q->front->prevQ = NULL;
        }
        free(t);
        return x;
    }
    else
    {
        printf("CommandQueue is empty!\n");
        return NULL;
    }
}

