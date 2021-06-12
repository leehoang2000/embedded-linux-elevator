#define MaxQ 2
#include "server.h"
typedef eRequest* EltypeQ;

typedef struct nodeQ {
  EltypeQ elQ;
  struct nodeQ *nextQ;
  struct nodeQ *prevQ;
}nodeQ;

//typedef struct nodeQ nodeQ;

typedef struct {
  nodeQ *front,*rear;
}Queue;

void makeNullQueue(Queue *q) {
  q->front=NULL;
  q->rear=NULL;
}

int emptyQueue(Queue q) {
  return (q.front==NULL && q.rear==NULL);
}

int fullQueue(Queue q) {
  nodeQ *c=q.front;
  int i=0;
  while (c!=NULL) {
    c=c->nextQ;
    ++i;
  }
  return i==MaxQ;
}

void enQueue(EltypeQ x,Queue *q) {
  if (fullQueue(*q)) printf("Queue is full!\n");
  else {
    nodeQ *new_node=(nodeQ*)malloc(sizeof(nodeQ));
    new_node->elQ=x;
    if (emptyQueue(*q)) {
      new_node->nextQ=NULL;new_node->prevQ=NULL;
      q->front=new_node;q->rear=new_node;
    } else {
      q->rear->nextQ=new_node;
      new_node->prevQ=q->rear;
      new_node->nextQ=NULL;
      q->rear=new_node;
    }
  }
}

EltypeQ deQueue(Queue *q) {
  if (!emptyQueue(*q)) {
    nodeQ* t;
    EltypeQ x;
    t=q->front;x=t->elQ;
    if (q->front==q->rear) {
      q->front=NULL;q->rear=NULL;
    } else {
      q->front=q->front->nextQ;
      q->front->prevQ=NULL;
    }
    free(t);
    return x;
  }
  else {
    printf("Queue is empty!\n");
    return NULL;
  }
}

