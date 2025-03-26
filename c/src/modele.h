#ifndef _MODELE

#define _MODELE

#include "src/queue.h"

typedef struct modele
{
    int dist_tot;
    int size; // number of truck
    Queue *list_truck;
} Modele;

#endif