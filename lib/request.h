#ifndef REQUEST_H
#define REQUEST_H
/** Request object is used in communication between liftCtrl and liftMng.
 * floorNumber: the floorNumber to be requested
 * isDone (bool): whether the request has been completed
 */

/** struct */
typedef struct Request{
    int floorNumber;
    int isDone;
} Request;

/** Request initializing function*/
Request createRequest(int floorNumber);

#endif