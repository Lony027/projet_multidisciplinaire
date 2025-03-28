#ifndef _RDV
#define _RDV

#include "src/place.h"

typedef struct appointment
{
    struct appointment *next;
    Place *lieu;
} Appointment;

#endif