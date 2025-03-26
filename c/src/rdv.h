#ifndef _RDV
#define _RDV

#include "src/lieu.h"

typedef struct rdv
{
    struct rdv *next;
    Lieu *lieu;
} Rdv;

#endif