#ifndef _RDV

#define _RDV
#include "c/src/lieu.h"
typedef struct rd
{
    struct rdv *next;
    Lieu *lieu;
} Rdv;

#endif
