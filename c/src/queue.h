#ifndef _QUEUE

#define _QUEUE
#include "src/rdv.h"

typedef struct queue
{
    int dist;
    Rdv *first;
    Rdv *last;
} Queue;

#endif
