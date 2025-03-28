#ifndef _APP
#define _APP

#include "src/place.h"

typedef struct appointment
{
    struct appointment *next;
    Place *place;
} Appointment;

#endif