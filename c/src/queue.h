#ifndef _QUEUE

#define _QUEUE
#include "src/appointment.h"

typedef struct queue
{
    int dist;
    Appointment *first;
    Appointment *last;
} Queue;

#endif
