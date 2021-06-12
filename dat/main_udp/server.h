#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
struct oPanelNode
{
    //int sockfd;
    int id;
    char ip_address[256];
    int port;
};

struct eRequest
{
    int destination_floor;
    int from_floor;
    int status; // 0-not handled , 1- handling , 2 - handled
    struct sockaddr_in from;
};

typedef struct eRequest eRequest;
typedef struct oPanelNode oPanelNode;

eRequest *newRequest(int from_floor,int dest_floor, struct sockaddr_in *cliad)
{
    eRequest *n = (eRequest *)malloc(sizeof(eRequest));
    n->from_floor = from_floor;
    n->destination_floor = dest_floor;
    n->status = 0;
    bzero(&n->from, sizeof(n->from));
    n->from.sin_family = AF_INET;
    n->from.sin_addr.s_addr = cliad->sin_addr.s_addr;
    n->from.sin_port = cliad->sin_port;
    return n;
}


oPanelNode *newNode(int id, char *ip, int port)
{
    oPanelNode *n = (oPanelNode *)malloc(sizeof(oPanelNode));
    strcpy(n->ip_address, ip);
    n->port=port;
    n->id = id;
    return n;
}
