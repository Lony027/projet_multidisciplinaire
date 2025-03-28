#ifndef _RDV
#define _RDV

#include "src/place.h"

typedef struct rdv
{
    struct rdv *next;
    Place *lieu;
} Rdv;

#endif