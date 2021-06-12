#include "request.h"
#include <stdio.h>

Request createRequest(int floorNumber){
    if(floorNumber <= 0 || floorNumber > 5){
        printf("Failed to create request: Floor number must be in range [1,5]\n");
        return;
    }

    Request request;
    request.floorNumber = floorNumber;
    request.isDone = 0;
    return request;
}