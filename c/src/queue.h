#ifndef _QUEUE

#define _QUEUE
#include "c/src/rdv.h"

typedef struct queue
{
    int dist;
    Rdv *first;
    Rdv *last;
} Queue;

#endif
