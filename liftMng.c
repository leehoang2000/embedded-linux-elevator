#include "liftMng.h"
#include "lib/messageQueue.h"
#include "lib/messageConfig.h"
#include "lib/request.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

int isKeyboardInterrupted = 0;

int MNG_PORT = 8080;
int MAXLINE = 5;
int sockfd;

MessageQueue msgQueueCtrlToMng;

int main()
{
    //sendWithMessageQueueObject();
    // MessageQueue msgQueue = getMessageQueue(KEY_FILE_PATH_MNG_TO_CTRL, MNG_TO_CTRL);
    // sendStringThroughMessageQueue(msgQueue, "3");

    //delete msgQueue on keyboard interuption
    signal(SIGINT, terminateHandler);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servaddr, selfaddr;

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(MNG_PORT);

    int status = -1;
    status = bind(sockfd, (const struct sockaddr *)&servaddr,
                  sizeof(servaddr));
    if (status < 0)
    {
        printf("Bind to %d failed\n", MNG_PORT);
        close(sockfd);
    }
    else
    {
        printf("Binded to %d\n", MNG_PORT);
    }

    listeningLoop();
}

/** listen for requested floor from panel*/

void terminateHandler()
{
    isKeyboardInterrupted = 1;
    //interrupted, delete queues
    deleteMessageQueue(msgQueueCtrlToMng);
    printf("Queue %d deleted\n", msgQueueCtrlToMng.messageQueueID);

    //Close the socket
    close(sockfd);

    exit(0);
}

int readFloor(char *msg)
{
    int floor = -1;
    sscanf(msg, "%3d", &floor);
    return floor;
}

int isValidFloor(int floor)
{
    return floor >= 0 && floor <= 5;
}

void listeningLoop()
{
    msgQueueCtrlToMng = getMessageQueue(KEY_FILE_PATH_CTRL_TO_MNG, CTRL_TO_MNG);
    char stringReceived[MSGSIZE];

    while (1)
    {
        int n;
        char buffer[MAXLINE];
        int received_floor = -1;

        struct sockaddr_in servaddr;
        int len = sizeof(servaddr);

        while (1)
        {
            memset(buffer, 0, MAXLINE);
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                         MSG_WAITALL, (struct sockaddr *)&servaddr,
                         (&len));
            received_floor = readFloor(buffer);

            if (isValidFloor(received_floor))
            {
                printf("Request to floor %d\n", received_floor);
                sendWithMessageQueueObject(received_floor);
            }
        }
    }
}

void sendWithMessageQueueObject(int floorNumber)
{
    /** Mock liftMng to send request (by floor number) to liftCtrl */
    MessageQueue msgQueue = getMessageQueue(KEY_FILE_PATH_MNG_TO_CTRL, MNG_TO_CTRL);
    //printf("liftMng init message queue id %d\n", msgQueue.messageQueueID); //debug
    Request request= createRequest(floorNumber);
    Message *message = createRequestMessage(request);
    printf("pop: %d\n", message->value.request.floorNumber);
    sendMessageToQueue(msgQueue, message);
}

/* delete msgQueue on keyboard interuption */
